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

void
odn_result_prepare(OdnResult * result,
		   HttpResponse * response,
		   OdnApplication * app)
{
  if(result->klass->prepare)
    result->klass->prepare(result,response,app);
}

gconstpointer
odn_result_get_content(OdnResult * result,
		       gsize * size)
{
  if(result->klass->get_content)
    return result->klass->get_content(result,size);

  *size = 0;
  return NULL;
}

OdnResult *
odn_result_copy(OdnResult * result)
{
  result->refcount ++;
  return result;
}

void
odn_result_free(OdnResult * result)
{
  if(result->refcount > 0)
    {
      result->refcount --;
    }
  else
    {
      result->klass->dispose(result);
      g_free(result);
    }
}

/* result class */

static void
odn_json_result_prepare(OdnResult * result,
		       HttpResponse * response,
		       OdnApplication * app)
{
  OdnJSONResult * self = (OdnJSONResult*)result;
  http_package_set_string(HTTP_PACKAGE(response),
			  "Content-Type",
			  "application/json",
			  15);
  if(self->is_list)
    {
      GString * buffer = g_string_new("[");
      for(GList * iter = g_list_first(self->data); iter; iter = g_list_next(iter))
	{
	  if(iter->data)
	    {
	      gchar * str = odn_model_to_string(iter->data);
	      g_string_append(buffer,str);
	      g_free(str);
	    }
	  else
	    {
	      g_string_append(buffer,"null");
	    }
	}
      g_string_append(buffer,"]");
      self->result = g_string_free(buffer,FALSE);
    }
  else
    {
      if(self->data)
	  self->data = odn_model_to_string(self->data);
      else
	  self->data = g_strdup("null");
    }
}

static gconstpointer
odn_json_result_get_content(OdnResult * result,
			       gsize * size)
{
  OdnJSONResult * self = (OdnJSONResult*)result;
  *size = g_utf8_strlen(self->result,G_MAXINT32);
  return self->result;
}

static void
odn_json_result_dispose(OdnResult * result)
{
  OdnJSONResult * self = (OdnJSONResult*)result;
  g_free(self->result);
  if(self->is_list)
    g_list_free_full(self->data,odn_model_free);
  else
    odn_model_free(self->data);
}


static void
odn_content_result_prepare(OdnResult * result,
			   HttpResponse * response,
			   OdnApplication * app)
{
  OdnContentResult * self = (OdnContentResult*)result;
  http_package_set_string(HTTP_PACKAGE(response),
			  "Content-Type",
			  self->mime_type,
			  g_utf8_strlen(self->mime_type,128));
}

static gconstpointer
odn_content_result_get_content(OdnResult * result,
			       gsize * size)
{
  OdnContentResult * self = (OdnContentResult*)result;
  *size = self->content_size;
  return self->content;
}

static void
odn_content_result_dispose(OdnResult * result)
{
  OdnContentResult * self = (OdnContentResult*)result;
  g_free(self->content);
  g_free(self->mime_type);
}

static void
odn_redirect_result_prepare(OdnResult * result,
			   HttpResponse * response,
			   OdnApplication * app)
{
  OdnRedirectResult * self = (OdnRedirectResult*)result;
  http_response_set_code(response,HTTP_RESPONSE_TEMPORARY_REDIRECT);

  http_package_set_string(HTTP_PACKAGE(response),
			  "Location",
			  self->url,
			  g_utf8_strlen(self->url,1024));
}

static gconstpointer
odn_redirect_result_get_content(OdnResult * result,
			       gsize * size)
{
  *size = 0;
  return NULL;
}

static void
odn_redirect_result_dispose(OdnResult * result)
{
  OdnRedirectResult * self = (OdnRedirectResult*)result;
  g_free(self->url);
}


static void
odn_view_result_prepare(OdnResult * result,
			   HttpResponse * response,
			   OdnApplication * app)
{
  OdnViewResult * self = (OdnViewResult*)result;
  self->mime_type = g_strdup("text/html");
  http_package_set_string(HTTP_PACKAGE(response),
			  "Content-Type",
			  self->mime_type,
			  g_utf8_strlen(self->mime_type,128));

  OdnViewRenderContext * context = odn_view_render_context_new(app,self->data);
  if(odn_application_render_view(app,self->name,context))
    {
      const gchar * layout_name =
	  odn_application_get_layout_for_view(app,self->name);
      self->content = odn_application_apply_layout(
	  app,
	  layout_name,
	  odn_view_render_context_get_content(context));
      self->content_size = g_utf8_strlen(self->content,G_MAXUINT32);
    }
  else
    {
      self->content = NULL;
      self->content_size = 0;
    }
  odn_view_render_context_free(context);
}

static gconstpointer
odn_view_result_get_content(OdnResult * result,
			       gsize * size)
{
  OdnViewResult * self = (OdnViewResult*)result;
  *size = self->content_size;
  return self->content;
}

static void
odn_view_result_dispose(OdnResult * result)
{
  OdnViewResult * self = (OdnViewResult*)result;
  g_free(self->content);
  g_free(self->mime_type);
}


static OdnResultClass odn_content_result_class = {
    odn_content_result_prepare,
    odn_content_result_get_content,
    odn_content_result_dispose
};

static OdnResultClass odn_json_result_class = {
    odn_json_result_prepare,
    odn_json_result_get_content,
    odn_json_result_dispose
};

static OdnResultClass odn_redirect_result_class = {
    odn_redirect_result_prepare,
    odn_redirect_result_get_content,
    odn_redirect_result_dispose
};

static OdnResultClass odn_view_result_class = {
    odn_view_result_prepare,
    odn_view_result_get_content,
    odn_view_result_dispose
};


/* constructors */

OdnResult *
odn_content_result_new(const gchar * content,
			gssize content_size,
			const gchar * mime_type)
{
  OdnContentResult * result = g_new(OdnContentResult,1);
  result->parent.klass = &odn_content_result_class;
  if(content_size < 0)
    content_size = g_utf8_strlen(content,G_MAXUINT32);

  result->content = g_strndup(content,content_size);
  result->content_size = content_size;
  result->mime_type = g_strdup(mime_type);
  return (OdnResult*)result;
}

OdnResult *
odn_redirect_result_new(const gchar * url)
{
  OdnRedirectResult * result = g_new(OdnRedirectResult,1);
  result->parent.klass = &odn_redirect_result_class;
  result->url = g_strdup(url);
  return (OdnResult*)result;
}

OdnResult *
odn_view_result_new(const gchar * view_name,
		    gpointer data)
{
  OdnViewResult * result = g_new(OdnViewResult,1);
  result->parent.klass = &odn_view_result_class;
  result->name = g_strdup(view_name);
  result->data = data;
  return (OdnResult*)result;
}

OdnResult *
odn_json_result_new(gpointer model,gboolean is_list)
{
  OdnJSONResult * result = g_new(OdnJSONResult,1);
  result->parent.klass = &odn_json_result_class;
  result->is_list = is_list;
  result->data = model;
  return (OdnResult*)result;
}
