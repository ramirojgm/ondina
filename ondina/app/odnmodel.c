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

/* model class */


const gchar *
odn_model_class_get_name(OdnModelClass * klass)
{
  return klass->name;
}


/* model */

gpointer
odn_model_new(OdnModelClass * klass)
{
  OdnModel * model = (OdnModel*) g_malloc0(klass->class_size);
  model->klass = klass;
  return model;
}

static OdnModelAttribute *
odn_model_find(gpointer model,const gchar * name)
{
  OdnModelClass * klass = ((OdnModel*)model)->klass;
  for(guint index = 0;index < klass->attributes_count;index++)
    {
      if(g_strcmp0(klass->attributes[index].name,name) == 0)
	return &(klass->attributes[index]);
    }
  return NULL;
}

gboolean
odn_model_contains(gpointer model,
		   const gchar * name)
{
  return odn_model_find(model,name) != NULL;
}

void
odn_model_set_boolean(gpointer model,
		      const gchar * name,
		      gboolean value)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    *((gboolean*)(model + attr->offset)) = value;
}

gboolean
odn_model_get_boolean(gpointer model,
		      const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    return *((gboolean*)(model + attr->offset));
  else
    return FALSE;
}

void
odn_model_set_double(gpointer model,
		     const gchar * name,
		     gdouble value)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    *((gdouble*)(model + attr->offset)) = value;
}

gdouble
odn_model_get_double(gpointer model,
		     const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    return *((gdouble*)(model + attr->offset));
  else
    return 0.0;
}

void
odn_model_set_int(gpointer model,
		  const gchar * name,
		  gint value)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    *((gint*)(model + attr->offset)) = value;
}

gint
odn_model_get_int(gpointer model,
		  const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    return *((gint*)(model + attr->offset));
  else
    return 0;
}

const gchar *
odn_model_get_string(gpointer model,
		     const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    return *((gchar**)(model + attr->offset));
  else
    return NULL;
}

void
odn_model_set_string(gpointer model,
		     const gchar * name,
		     const gchar * value)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    {
      gchar ** attr_ptr = ((gchar **)(model + attr->offset));
      if(*attr_ptr)
	g_free(*attr_ptr);
      *attr_ptr = g_strdup(value);
    }
}


gpointer
odn_model_add(gpointer model,
		 const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    {
      GList ** attr_ptr = ((GList **)(model + attr->offset));
      if(attr->klass)
	{
	  gpointer instance = odn_model_new(attr->klass);
	  *attr_ptr = g_list_append(*attr_ptr,instance);
	  return instance;
	}
    }
  return NULL;
}

GList *
odn_model_get_list(gpointer model,
		   const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
   return *((GList**)(model + attr->offset));
  else
   return NULL;
}

void
odn_model_remove(gpointer model,
		 const gchar * name,
		 gpointer data)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    {
      GList ** attr_ptr = ((GList **)(model + attr->offset));
      if(attr->klass)
	*attr_ptr = g_list_remove(*attr_ptr,data);
    }
}

void
odn_model_free(gpointer model)
{
  OdnModelClass * klass = ((OdnModel*)model)->klass;
  for(guint index = 0;index < klass->attributes_count;index++)
    {
      OdnModelAttribute * attr = &(klass->attributes[index]);
      gpointer ptr = *((gpointer*)(model + attr->offset));
      if(attr->type == ODN_MODEL_TYPE_STRING)
      	g_free(ptr);
      else if(attr->type == ODN_MODEL_TYPE_LIST)
      	g_list_free_full(((GList*)ptr),odn_model_free);
    }
  g_free(model);
}
