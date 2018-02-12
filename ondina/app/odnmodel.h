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

#ifndef ONDINA_MODEL_H_
#define ONDINA_MODEL_H_

#include <stddef.h>

typedef struct _OdnModelAttribute OdnModelAttribute;
typedef struct _OdnModelClass OdnModelClass;
typedef enum _OdnModelType OdnModelType;
typedef struct _OdnModel OdnModel;

enum _OdnModelType
{
  ODN_MODEL_TYPE_BOOLEAN,
  ODN_MODEL_TYPE_CHAR,
  ODN_MODEL_TYPE_INT16,
  ODN_MODEL_TYPE_INT32,
  ODN_MODEL_TYPE_INT64,
  ODN_MODEL_TYPE_FLOAT,
  ODN_MODEL_TYPE_DOUBLE,
  ODN_MODEL_TYPE_STRING,
  ODN_MODEL_TYPE_LIST,
  ODN_MODEL_TYPE_INVALID
};

struct _OdnModelAttribute
{
  const gchar * name;
  OdnModelType	type;
  OdnModelClass * klass;
  goffset offset;
};

struct _OdnModelClass
{
  const gchar * name;
  guint attributes_count;
  OdnModelAttribute attributes[16];
  gsize class_size;
};

struct _OdnModel
{
  OdnModelClass * klass;
};


const gchar *	odn_model_class_get_name(OdnModelClass * klass);


/* model */

gpointer 	odn_model_new(OdnModelClass * klass);

gpointer	odn_model_from_string(OdnModelClass * klass,
				      const gchar * str,
				      gssize length,
				      GError ** error);

gchar *		odn_model_to_string(gpointer model);


gboolean	odn_model_contains(gpointer model,
				   const gchar * name);

void		odn_model_set_boolean(gpointer model,
				      const gchar * name,
				      gboolean value);

gboolean 	odn_model_get_boolean(gpointer model,
				      const gchar * name);

void		odn_model_set_double(gpointer model,
				     const gchar * name,
				     gdouble value);

gdouble 	odn_model_get_double(gpointer model,
				     const gchar * name);

void		odn_model_set_int(gpointer model,
				  const gchar * name,
				  gint value);

gint		odn_model_get_int(gpointer model,
				  const gchar * name);

const gchar * 	odn_model_get_string(gpointer model,
				     const gchar * name);

void		odn_model_set_string(gpointer model,
				     const gchar * name,
				     const gchar * value);

GList *	 	odn_model_get_list(gpointer model,
				   const gchar * name);

gpointer 	odn_model_add(gpointer model,
				 const gchar * name);

void	 	odn_model_add_null(gpointer model,
				   const gchar * name);


void	 	odn_model_remove(gpointer model,
				 const gchar * name,
				 gpointer data);

void		odn_model_free(gpointer model);



#endif /* ONDINA_MODEL_H_ */
