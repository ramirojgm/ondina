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

const gchar *
odn_context_get_params(OdnContext * context)
{
  return context->request_params;
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
