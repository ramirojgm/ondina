/*
	Copyright (C) 2018 Ramiro Jose Garcia Moraga

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ondina.h>
#include <string.h>

struct _OdnApplicationPrivate
{
  GMutex static_mutex;
  GMutex dynamic_mutex;

  GType	session_type;

  GThreadedSocketService * socket_service;

  GList * content;
  GList * view;
  GList * layout;
  GList * controller;
  GList * session;
};

G_DEFINE_TYPE_WITH_PRIVATE(OdnApplication,odn_application,G_TYPE_APPLICATION);

typedef struct
{
  gchar * key;
  gpointer object;
}OdnKeyObject;

typedef struct
{
  gchar * key;
  gchar * layout_content;
  goffset layout_offset;
  gssize layout_size;
}OdnLayout;

static OdnLayout *
odn_layout_new(const gchar * key,const gchar * filename)
{
  OdnLayout * layout = NULL;
  gchar * layout_content = NULL;
  gsize layout_size = 0;
  if(g_file_get_contents(filename,&layout_content,&layout_size,NULL))
    {
      layout = g_new(OdnLayout,1);
      layout->key = g_strdup(key);
      layout->layout_content = layout_content;
      layout->layout_size = layout_size;

      const gchar * offset = g_strrstr(layout_content,"<!--LayoutContent-->");
      if(offset)
	layout->layout_offset = (offset - layout_content);
      else
	layout->layout_offset = -1;
    }
  return layout;
}

static OdnLayout *
odn_layout_find(GList * list,const gchar * key)
{
  for(GList * iter = g_list_first(list); iter; iter = g_list_next(iter))
    {
      OdnLayout * layout = (OdnLayout*)iter->data;
      if(g_strcmp0(layout->key,key) == 0)
	return layout;
    }
  return NULL;
}

static gchar *
odn_layout_apply(OdnLayout * layout,const gchar * content)
{
  if(layout->layout_offset >= 0)
    {
      gchar * buffer =
	  g_malloc0((layout->layout_size - 20) + g_utf8_strlen(content,G_MAXUINT32) + 1);

      strncat(buffer,
	      layout->layout_content,
	      layout->layout_offset);
      strcat(buffer,content);
      strcat(buffer,layout->layout_content + layout->layout_offset + 20);
      return buffer;
    }
  else
    {
      return g_strdup(layout->layout_content);
    }
}

static void
odn_layout_free(OdnLayout * layout)
{
  g_free(layout->key);
  g_free(layout->layout_content);
  g_free(layout);
}

static OdnKeyObject *
odn_key_object_new(const gchar * key,
		    gpointer object)
{
  OdnKeyObject * key_object = g_new(OdnKeyObject,1);
  key_object->key = g_strdup(key);
  key_object->object = object;
  return key_object;
}

static gpointer
odn_key_object_find(GList * list,
		    const gchar * key)
{
  for(GList * iter = g_list_first(list);iter;iter = g_list_next(iter))
    {
      OdnKeyObject *
      key_object = (OdnKeyObject*)iter->data;
      if(g_strcmp0(key_object->key,key) == 0)
	  return key_object->object;
    }
  return NULL;
}

static void
odn_key_object_free(OdnKeyObject * key_object)
{
  g_object_unref(key_object->object);
  g_free(key_object->key);
  g_free(key_object);
}

static gboolean
odn_application_static_content(OdnApplication * self,
			       HttpRequest * request,
			       HttpResponse * response,
			       GInputStream * input,
			       GOutputStream * output)
{
  gboolean found = FALSE;

  g_mutex_lock(&self->priv->static_mutex);

  gconstpointer content = NULL;
  gsize content_length = 0;

  OdnCache * file_cached = ODN_CACHE(odn_key_object_find(
      self->priv->content,
      http_request_get_query(request)));

  if(file_cached)
    {
      found = TRUE;

      const gchar * accept_encoding = "";
      const gchar * content_type = odn_cache_get_mime_type(file_cached);

      if(http_package_is_set(HTTP_PACKAGE(request),"Accept-Encoding"))
	accept_encoding = http_package_get_string(
	    HTTP_PACKAGE(request),
	    "Accept-Encoding",
	    NULL);

      if(g_strrstr(accept_encoding,"deflate"))
	{
	  http_package_set_string(
	      HTTP_PACKAGE(response),
	      "Content-Encoding",
	      "deflate",
	      7);

	  content = odn_cache_get_content(
	      file_cached,
	      ODN_COMPRESSION_DEFLATE,
	      &content_length);
	}
      else if(g_strrstr(accept_encoding,"gzip"))
	{
	  http_package_set_string(
	      HTTP_PACKAGE(response),
	      "Content-Encoding",
	      "gzip",
	      4);

	  content = odn_cache_get_content(
	      file_cached,
	      ODN_COMPRESSION_GZIP,
	      &content_length);
	}
      else
	{
	  content = odn_cache_get_content(
	      file_cached,
	      ODN_COMPRESSION_NONE,
	      &content_length);
	}

      http_package_set_string(
	  HTTP_PACKAGE(response),
	  "Content-Type",
	  content_type,
	  g_utf8_strlen(content_type,256));

      http_package_set_int(
	  HTTP_PACKAGE(response),
	  "Content-Length",
	  content_length);

      http_package_write_to_stream(
	  HTTP_PACKAGE(response),
	  output,
	  NULL,
	  NULL,
	  NULL);

      g_output_stream_write_all(
	  output,
	  content,
	  content_length,
	  NULL,
	  NULL,
	  NULL);
    }

  g_mutex_unlock(&self->priv->static_mutex);

  return found;
}

static gchar *
odn_application_generate_uuid()
{
  gchar charset[] = "0123456789abcdef";
  gchar uuid[37] = {0,};
  for(guint index = 0;index <= 35;index ++) {
      if(index == 8 || index == 13 || index == 18 || index == 23)
	uuid[index] = '-';
      else
	uuid[index] = charset[g_random_int_range(0,15)];
  }
  return g_strdup(uuid);
}

static gpointer
odn_application_bind_session(OdnApplication * self,
			     HttpRequest * request,
			     HttpResponse * response)
{
  const gchar * session_id = NULL;
  gpointer * session = NULL;

  if(http_package_is_set(HTTP_PACKAGE(request),"Cookie"))
    {
      const gchar * cookie = http_package_get_string(
	  HTTP_PACKAGE(request),
	  "Cookie",
	  NULL);

      session_id = g_strrstr(cookie,"ondina_id=");
    }

  if(session_id && g_utf8_strlen(session_id,50) >= 46)
    {
      gchar * uuid = g_strndup(session_id + 10,36);
      session = odn_key_object_find(self->priv->session,uuid);
      g_free(uuid);
    }

  if(!session)
    {
      gchar * uuid = NULL;
      session = g_object_new(self->priv->session_type,NULL);
      do
	{
	  if(uuid)
	    g_free(uuid);
	  uuid = odn_application_generate_uuid();
	}
      while(odn_key_object_find(self->priv->session,uuid));

      self->priv->session = g_list_append(
	  self->priv->session,
	  odn_key_object_new(uuid,session));

      gchar * cookie_value =
	  g_strdup_printf("ondina_id=%s; httponly; Path=/",uuid);

      http_package_set_string(
	  HTTP_PACKAGE(response),
	  "Set-Cookie",
	  cookie_value,
	  g_utf8_strlen(cookie_value,128));

      g_free(uuid);
      g_free(cookie_value);
    }

  return session;
}

static gboolean
odn_application_dynamic_content(OdnApplication * self,
			       HttpRequest * request,
			       HttpResponse * response,
			       GInputStream * input,
			       GOutputStream * output)
{
  gboolean found = FALSE;
  g_mutex_lock(&self->priv->dynamic_mutex);

  const gchar * query = http_request_get_query(request);
  const gchar * query_action = g_strrstr(query,"/");

  gchar * vpath = NULL;

  if(g_strcmp0(query,"/") == 0)
    {
      vpath = g_strdup("/default");
      query_action = "/index";
    }
  else if(query != query_action)
    {
      vpath = g_strndup(query,query_action - query);
    }
  else
    {
      vpath = g_strdup(query);
      query_action = "/index";
    }

  OdnController * controller = odn_key_object_find(self->priv->controller,
						   vpath);
  if(controller)
    {
      if(odn_controller_has(controller,query_action))
	{
	  found = TRUE;
	  GError * error = NULL;
	  gpointer session = odn_application_bind_session(self,
							 request,
							 response);
	  gchar * body = NULL;
	  gsize body_size = 0;

	  if(http_package_is_set(HTTP_PACKAGE(request),"Content-Length"))
	    {
	      body_size = http_package_get_int(
		  HTTP_PACKAGE(request),
		  "Content-Length");
	      body = (gchar*)g_malloc(body_size + 1);
	      g_input_stream_read_all(input,body,body_size,NULL,NULL,NULL);
	    }

	  OdnContext * context = odn_context_new(self,
						 request,
						 "",
						 body,
						 body_size,
						 session);
	  if(body)
	    g_free(body);

	  OdnResult * result = odn_controller_invoke(controller,
						     query_action,
						     context,
						     &error);
	  odn_context_free(context);
	  if(!error)
	    {
	      if(result)
		{
		  gconstpointer result_content = NULL;
		  gsize result_content_size = 0;
		  odn_result_prepare(result,response,self);
		  result_content = odn_result_get_content(result,
							  &result_content_size);
		  http_package_set_int(HTTP_PACKAGE(response),
				       "Content-Length",
				       result_content_size);

		  http_package_write_to_stream(HTTP_PACKAGE(response),
					       output,
					       NULL,
					       NULL,
					       NULL);
		  if(result_content)
		    {
		      g_output_stream_write_all(output,
					      result_content,
					      result_content_size,
					      NULL,
					      NULL,
					      NULL);
		    }
		  odn_result_free(result);
		}
	      else
		{
		  /* empty response */
		  http_response_set_code(response,
				       HTTP_RESPONSE_NO_CONTENT);
		  http_package_write_to_stream(HTTP_PACKAGE(response),
							 output,
							 NULL,
							 NULL,
							 NULL);
		}
	    }
	  else
	    {
	      /* error response */
	      gsize error_message_size = g_utf8_strlen(error->message,512);
	      http_response_set_code(response,
				     HTTP_RESPONSE_INTERNAL_SERVER_ERROR);
	      http_package_set_int(HTTP_PACKAGE(response),
				   "Content-Length",
				   error_message_size);

	      http_package_write_to_stream(HTTP_PACKAGE(response),
					   output,
					   NULL,
					   NULL,
					   NULL);

	      g_output_stream_write_all(output,
					error->message,
					error_message_size,
					NULL,
					NULL,
					NULL);
	      g_error_free(error);
	    }

	}
    }

  g_free(vpath);
  g_mutex_unlock(&self->priv->dynamic_mutex);
  return found;
}


static gboolean
odn_application_service_run(OdnApplication * self,
			    GSocketConnection * connection,
			    GObject * source,
			    gpointer data)
{

  HttpRequest * request = http_request_new(HTTP_REQUEST_METHOD_GET,"/",1.1);
  HttpResponse * response = http_response_new(HTTP_RESPONSE_OK,1.1);

  GInputStream * input = g_io_stream_get_input_stream(
      G_IO_STREAM(connection));

  GOutputStream * output = g_io_stream_get_output_stream(
      G_IO_STREAM(connection));

  GDataInputStream * data_input = http_data_input_stream(input,NULL,NULL,NULL);

  if(data_input)
    {
      http_package_read_from_stream(
	  HTTP_PACKAGE(request),
	  data_input,
	  NULL,
	  NULL,
	  NULL);

      g_object_unref(data_input);

      gboolean content_found = FALSE;
      if(http_request_get_method(request) == HTTP_REQUEST_METHOD_GET)
	{
	  content_found = odn_application_static_content(self,
	  					      request,
	  					      response,
	  					      input,
	  					      output);
	}

      if(!content_found)
	{
	  content_found = odn_application_dynamic_content(self,
							request,
							response,
							input,
							output);
	}

      if(!content_found)
	{
	  http_response_set_code(response,HTTP_RESPONSE_NOT_FOUND);
	  http_package_write_to_stream(
	      HTTP_PACKAGE(response),
	      output,
	      NULL,
	      NULL,
	      NULL);
	}

      g_output_stream_flush(output,NULL,NULL);
    }

  g_object_unref(request);
  g_object_unref(response);

  g_io_stream_close(G_IO_STREAM(connection),NULL,NULL);
  return TRUE;
}

static void
odn_application_init(OdnApplication * self)
{
  self->priv = odn_application_get_instance_private(self);
  g_mutex_init(&self->priv->dynamic_mutex);
  g_mutex_init(&self->priv->static_mutex);

  self->priv->session_type = G_TYPE_OBJECT;
  self->priv->socket_service = G_THREADED_SOCKET_SERVICE(
      g_threaded_socket_service_new(128));

  g_signal_connect_swapped(
      G_OBJECT(self->priv->socket_service),
      "run",
      G_CALLBACK(odn_application_service_run),
      self);
}

static void
odn_application_dispose(GObject * object)
{
  OdnApplication * self = ODN_APPLICATION(object);
  g_mutex_clear(&self->priv->dynamic_mutex);
  g_mutex_clear(&self->priv->static_mutex);
  g_object_unref(self->priv->socket_service);
  g_list_free_full(self->priv->content,(GDestroyNotify)odn_key_object_free);
  g_list_free_full(self->priv->controller,(GDestroyNotify)odn_key_object_free);
  g_list_free_full(self->priv->session,(GDestroyNotify)odn_key_object_free);
  g_list_free_full(self->priv->layout,(GDestroyNotify)odn_layout_free);
  g_list_free_full(self->priv->view,(GDestroyNotify)odn_key_object_free);
  G_OBJECT_CLASS(odn_application_parent_class)->dispose(object);
}

static void
odn_application_startup(GApplication * app)
{
  OdnApplication * self = ODN_APPLICATION(app);
  g_socket_service_start(G_SOCKET_SERVICE(self->priv->socket_service));
  G_APPLICATION_CLASS(odn_application_parent_class)->startup(app);
  g_application_hold(app);
}

static void
odn_application_shutdown(GApplication * app)
{
  OdnApplication * self = ODN_APPLICATION(app);
  g_socket_service_stop(G_SOCKET_SERVICE(self->priv->socket_service));
  G_APPLICATION_CLASS(odn_application_parent_class)->shutdown(app);
}

static void
odn_application_class_init(OdnApplicationClass * klass)
{
  G_APPLICATION_CLASS(klass)->startup = odn_application_startup;
  G_APPLICATION_CLASS(klass)->shutdown = odn_application_shutdown;
  G_OBJECT_CLASS(klass)->dispose = odn_application_dispose;
}

OdnApplication *
odn_application_new(const gchar * application_id)
{
  return ODN_APPLICATION(
      g_object_new(ODN_TYPE_APPLICATION,
		   "application-id",application_id,
		   "flags",G_APPLICATION_IS_SERVICE,
		   NULL));
}

void
odn_application_bind_inet_port(OdnApplication * app,
			       guint16 inet_port)
{
  g_socket_listener_add_inet_port(
      G_SOCKET_LISTENER(app->priv->socket_service),
      inet_port,
      NULL,
      NULL);
}

GType
odn_application_get_session_type(OdnApplication * app)
{
  return app->priv->session_type;
}

void
odn_application_set_session_type(OdnApplication * app,
				 GType session_type)
{
  app->priv->session_type = session_type;
}

void
odn_application_bind_dynamic(OdnApplication * app,
			      const gchar * path)
{
  if(g_file_test(path,G_FILE_TEST_IS_DIR))
    {
      GDir * content_dir = g_dir_open(path,0,NULL);
      const gchar * node_name = NULL;
      while((node_name = g_dir_read_name(content_dir)))
	{
	  gchar * new_path = g_build_filename(path,node_name,NULL);
	  if(g_file_test(new_path,G_FILE_TEST_IS_REGULAR))
	    {
	      if(g_str_has_suffix(new_path,".layout"))
		{
		  OdnLayout * layout = odn_layout_new(node_name,new_path);
		  if(layout)
		    {
		      g_mutex_lock(&app->priv->dynamic_mutex);
		      app->priv->layout = g_list_append(app->priv->layout,
							layout);
		      g_mutex_unlock(&app->priv->dynamic_mutex);
		    }
		}
	      else if(g_str_has_suffix(new_path,".view"))
		{
		  OdnView * view = odn_view_new();
		  if(odn_view_load_from_file(view,new_path,NULL))
		    {
		      g_mutex_lock(&app->priv->dynamic_mutex);
		      app->priv->view = g_list_append(
			  app->priv->view,
			  odn_key_object_new(node_name,
					     view));
		      g_mutex_unlock(&app->priv->dynamic_mutex);
		    }
		  else
		    {
		      g_object_unref(view);
		    }
		}
	    }
	  g_free(new_path);
	}
      g_dir_close(content_dir);
    }
}

void
odn_application_bind_content(OdnApplication * app,
			    const gchar * vpath,
			    const gchar * path)
{
  if(g_file_test(path,G_FILE_TEST_IS_DIR))
    {
      GDir * content_dir = g_dir_open(path,0,NULL);
      const gchar * node_name = NULL;
      while((node_name = g_dir_read_name(content_dir)))
	{
	  gchar * new_vpath = g_build_filename(vpath,node_name,NULL);
	  gchar * new_path = g_build_filename(path,node_name,NULL);
	  if(g_file_test(new_path,G_FILE_TEST_IS_DIR))
	    {
	      odn_application_bind_content(app,new_vpath,new_path);
	    }
	  else
	    {
	      OdnCache * file_cached = odn_cache_new_from_file(new_path,
							       NULL);
	      if(file_cached)
		{
		  g_mutex_lock(&app->priv->static_mutex);
		  app->priv->content = g_list_append(
		      app->priv->content,
		      odn_key_object_new(new_vpath,file_cached));
		  g_mutex_unlock(&app->priv->static_mutex);
		}
	    }
	  g_free(new_vpath);
	  g_free(new_path);
	}
      g_dir_close(content_dir);
    }
}

void
odn_application_bind_controller(OdnApplication * app,
				const gchar * vpath,
				GType type)
{
  g_mutex_lock(&app->priv->dynamic_mutex);
  app->priv->controller = g_list_append(
      app->priv->controller,
      odn_key_object_new(vpath,g_object_new(type,NULL)));
  g_mutex_unlock(&app->priv->dynamic_mutex);
}

gchar *
odn_application_apply_layout(OdnApplication * app,
			     const gchar * layout_name,
			     const gchar * content)
{
  OdnLayout * layout = odn_layout_find(app->priv->layout,layout_name);
  if(layout)
      return odn_layout_apply(layout,content);
  else
      return NULL;
}

gboolean
odn_application_render_view(OdnApplication * app,
			   const gchar * view_name,
			   OdnViewRenderContext * context)
{
  OdnView * view = odn_key_object_find(app->priv->view,view_name);
  if(view)
    {
      odn_view_render(view,context);
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

const gchar *
odn_application_get_layout_for_view(OdnApplication * app,
				    const gchar * view_name)
{
  OdnView * view = odn_key_object_find(app->priv->view,view_name);
  if(view)
    return odn_view_get_layout(view);
  else
    return NULL;
}
