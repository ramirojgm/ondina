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

#ifndef ONDINA_RESULT_H_
#define ONDINA_RESULT_H_

#define ODN_TYPE_RESULT		(odn_result_get_type())

typedef struct _OdnResult OdnResult;
typedef struct _OdnResultClass OdnResultClass;

struct _OdnResultClass
{
  void (*prepare)(
      OdnResult * result,
      HttpResponse * response,
      OdnApplication * app);

  gconstpointer (*get_content)(
      OdnResult * result,
      gsize * size);

  void (*dispose)(
      OdnResult * result);

};

struct _OdnResult
{
  volatile guint refcount;
  OdnResultClass * klass;
};

typedef struct
{
  OdnResult parent;

  /* data */
  gchar * mime_type;
  gchar * content;
  gsize   content_size;
} OdnContentResult;

typedef struct
{
  OdnResult parent;

  /* data */
  gboolean is_list;
  gpointer data;
  gchar * result;
} OdnJSONResult;

typedef struct
{
  OdnResult parent;

  /* data */
  gchar * url;
} OdnRedirectResult;

typedef struct
{
  OdnResult parent;

  /* data */
  gchar * name;
  gboolean is_array;
  gpointer data;

  /* result */
  gchar * content;
  gchar * mime_type;
  gsize content_size;
} OdnViewResult;

GType		odn_result_get_type(void);

void		odn_result_prepare(OdnResult * result,
				   HttpResponse * response,
				   OdnApplication * app);

gconstpointer	odn_result_get_content(OdnResult * result,
				       gsize * size);

OdnResult *	odn_result_copy(OdnResult * result);

void 		odn_result_free(OdnResult * result);

/* constructors */

OdnResult * 	odn_content_result_new(const gchar * content,
				      gssize content_size,
				      const gchar * mime_type);

OdnResult * 	odn_redirect_result_new(const gchar * url);

OdnResult * 	odn_view_result_new(const gchar * view_name,gpointer data,gboolean is_list);

OdnResult * 	odn_json_result_new(gpointer data,gboolean is_list);

#endif /* ONDINA_RESULT_H_ */
