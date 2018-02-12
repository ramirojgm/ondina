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
#include <stdlib.h>

G_DEFINE_BOXED_TYPE(OdnContext,odn_context,odn_context_copy,odn_context_free);

OdnContext *
odn_context_new(OdnApplication * app,
		HttpRequest * request,
		const gchar * params,
		const gchar * body,
		gsize body_size,
		gpointer session)
{
  OdnContext * new_context = g_new(OdnContext,1);
  new_context->app = (OdnApplication*)g_object_ref(app);
  new_context->request = (HttpRequest*)g_object_ref(request);
  new_context->request_body = g_memdup(body,body_size);
  new_context->request_body_size = body_size;
  new_context->request_params = g_strdup(params);
  new_context->session = g_object_ref(session);
  return new_context;
}

OdnApplication *
odn_context_get_application(OdnContext * context)
{
  return context->app;
}

HttpRequest *
odn_context_get_request(OdnContext * context)
{
  return context->request;
}

gboolean
odn_context_get_params(OdnContext * context,...)
{
  struct  {
    const gchar * name;
    GType 	type;
    gpointer 	address;
  } param [32];

  guint param_count = 0;

  gboolean done = TRUE;
  OdnParser * parser = odn_parser_new();
  va_list va;
  if(odn_parser_parse(parser,context->request_body,context->request_body_size))
    {
      va_start(va,context);
      while((param [param_count].name = va_arg(va,gchar*)) != NULL)
	{
	  param [param_count].type = va_arg(va,GType);
	  param [param_count].address = va_arg(va,gpointer);
	  param_count ++;
	}
      va_end(va);

      if(odn_parser_next_word_check(parser,"{"))
	{
	  do
	    {
	      g_autofree gchar * member_name =  odn_parser_extract_string(odn_parser_next_word(parser,NULL,TRUE));
	      if(odn_parser_next_word_check(parser,":"))
		{
		  guint param_index = -1;
		  for(guint index = 0;index < param_count; index++)
		    {
		      g_print("%s == %s\n",param[index].name,member_name);
		      if(g_strcmp0(param[index].name,member_name) == 0)
			{
			  param_index = index;
			  break;
			}
		    }

		  if(param_index != -1)
		    {
		      switch(param[param_index].type)
		      {
			case G_TYPE_INT:
			  *(gint*)(param[param_index].address) = atoi(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case G_TYPE_DOUBLE:
			  *(gdouble*)(param[param_index].address) = atof(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case G_TYPE_STRING:
			  *(gchar**)(param[param_index].address) = odn_parser_extract_string(odn_parser_next_word(parser,NULL,TRUE));
			  break;
		      }
		    }
		}
	      else
		{
		  done = FALSE;
		  break;
		}
	    }
	  while(odn_parser_next_word_check(parser,",") && !odn_parser_is_end(parser));
	  if(done)
	    done = odn_parser_next_word_check(parser,"}");
	}
    }
  g_object_unref(parser);
  return done;
}

const gchar *
odn_context_get_body(OdnContext * context)
{
  return context->request_body;
}

gsize
odn_context_get_body_size(OdnContext * context)
{
  return context->request_body_size;
}

gpointer
odn_context_parse_object(OdnContext * context,GType object_type)
{
  return NULL;
}

gpointer
odn_context_get_session(OdnContext * context)
{
  return context->session;
}

OdnContext *
odn_context_copy(OdnContext * context)
{
  return odn_context_new(context->app,
			 context->request,
			 context->request_params,
			 context->request_body,
			 context->request_body_size,
			 context->session);
}

void
odn_context_free(OdnContext * context)
{
  g_object_unref(context->app);
  g_object_unref(context->request);
  g_object_unref(context->session);
  g_free(context->request_params);
  g_free(context->request_body);
  g_free(context);
}
