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

#ifndef ONDINA_CONTEXT_H_
#define ONDINA_CONTEXT_H_

#define ODN_TYPE_CONTEXT		(odn_context_get_type())

typedef struct
{
  OdnApplication * app;
  HttpRequest * request;
  gchar * request_params;
  gchar * request_body;
  gsize request_body_size;
  gpointer session;
} OdnContext;

GType		odn_context_get_type(void);

OdnContext *	odn_context_new(OdnApplication * app,
				HttpRequest * request,
				const gchar * params,
				const gchar * body,
				gsize body_size,
				gpointer session);

gboolean	odn_context_get_params(OdnContext * context,...);

OdnApplication *odn_context_get_application(OdnContext * context);

HttpRequest * 	odn_context_get_request(OdnContext * context);


const gchar *	odn_context_get_body(OdnContext * context);

gsize		odn_context_get_body_size(OdnContext * context);

gpointer	odn_context_get_session(OdnContext * context);

OdnContext *	odn_context_copy(OdnContext * context);

void		odn_context_free(OdnContext * context);

#endif /* ONDINA_CONTEXT_H_ */
