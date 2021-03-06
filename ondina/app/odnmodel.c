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
odn_model_class_find(OdnModelClass * klass,const gchar * name)
{
  for(guint index = 0;index < klass->attributes_count;index++)
    {
      if(g_strcmp0(klass->attributes[index].name,name) == 0)
	return &(klass->attributes[index]);
    }
  return NULL;
}

static gboolean
_odn_model_parse(OdnModelClass * klass,
		 gpointer model,
		 OdnParser * parser,
		 GError ** error)
{
  gboolean done = TRUE;
  if(odn_parser_next_word_check(parser,"{"))
    {
      do{
	  if(odn_parser_check_is_string(parser))
	    {
	      g_autofree gchar * member_name =  odn_parser_extract_string(odn_parser_next_word(parser,NULL,TRUE));
	      if(odn_parser_next_word_check(parser,":"))
		{
		  OdnModelAttribute * attr = odn_model_class_find(klass,member_name);
		  if(attr)
		    {
		      switch(attr->type)
		      {
			case ODN_MODEL_TYPE_BOOLEAN:
			  if(g_strcmp0("true",odn_parser_next_word(parser,NULL,TRUE)) == 0)
			    *((gboolean*)(model + attr->offset)) = TRUE;
			  else
			    *((gboolean*)(model + attr->offset)) = FALSE;
			  break;
			case ODN_MODEL_TYPE_CHAR:
			  if(odn_parser_next_word_check(parser,"null"))
			    *((gchar*)(model + attr->offset)) = 0;
			  else
			    *((gchar*)(model + attr->offset)) = *(odn_parser_next_word(parser,NULL,TRUE) + 1);
			  break;
			case ODN_MODEL_TYPE_STRING:
			  *((gchar**)(model + attr->offset)) = odn_parser_extract_string(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case ODN_MODEL_TYPE_FLOAT:
			  *((gfloat*)(model + attr->offset)) = atof(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case ODN_MODEL_TYPE_DOUBLE:
			  *((gdouble*)(model + attr->offset)) = atof(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case ODN_MODEL_TYPE_INT16:
			  *((gint16*)(model + attr->offset)) = atoi(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case ODN_MODEL_TYPE_INT32:
			  *((gint32*)(model + attr->offset)) = atoi(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case ODN_MODEL_TYPE_INT64:
			  *((gint64*)(model + attr->offset)) = atoi(odn_parser_next_word(parser,NULL,TRUE));
			  break;
			case ODN_MODEL_TYPE_LIST:
			  if(odn_parser_next_word_check(parser,"null"))
			    continue;
			  else if(odn_parser_next_word_check(parser,"["))
			    {
			      do
				{
				  if(odn_parser_next_word_check(parser,"null")||
				     odn_parser_next_word_check(parser,"undefined"))
				    {
				      odn_model_add_null(model,member_name);
				    }
				  else
				    {
				      gpointer child = odn_model_add(model,member_name);
				      if(!_odn_model_parse(((OdnModel*)child)->klass,child,parser,error))
					{
					  odn_model_remove(model,member_name,child);
					  odn_model_free(child);
					  done = FALSE;
					}
				    }
				}
			      while(odn_parser_next_word_check(parser,",") && !odn_parser_is_end(parser));
			      if(done)
				done = odn_parser_next_word_check(parser,"]");
			    }
			  else
			    {
			      done = FALSE;
			      break;
			    }
			  break;
			case ODN_MODEL_TYPE_INVALID:
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
	  else
	    {
	      done = FALSE;
	      break;
	    }

      } while (odn_parser_next_word_check(parser,",") && !odn_parser_is_end(parser));

      if(done)
      	done = odn_parser_next_word_check(parser,"}");
    }
  else
    {
      done = FALSE;
    }


  if(!done && error && !*error)
    {
      *error = g_error_new(
	  g_quark_from_static_string("odn-model-error"),
	  0,
	  "bad syntaxes exception");
    }

  return done;
}

gpointer
odn_model_from_string(OdnModelClass * klass,
		      const gchar * str,
		      gssize length,
		      GError ** error)
{
  OdnParser * p = odn_parser_new();
  odn_parser_parse(p,str,length);
  gpointer model = odn_model_new(klass);
  if(!_odn_model_parse(klass,model,p,error))
    {
      odn_model_free(model);
      model = NULL;
    }
  g_object_unref(p);
  return model;
}


gchar *
odn_model_to_string(gpointer model)
{
  GString * buffer = g_string_new("{");

  OdnModelClass * klass = ((OdnModel*)model)->klass;
  gboolean is_first = TRUE;

  for(guint index = 0;index < klass->attributes_count;index++)
    {
      OdnModelAttribute * attr = &(klass->attributes[index]);
      if(is_first)
	is_first = FALSE;
      else
	g_string_append(buffer,",");

      g_string_append_printf(buffer,"\"%s\":",attr->name);
      switch(attr->type)
      {
	case ODN_MODEL_TYPE_BOOLEAN:
	  g_string_append(buffer,*((gboolean*)(model + attr->offset)) ? "true": "false");
	  break;
	case ODN_MODEL_TYPE_CHAR:
	  g_string_append_printf(buffer,"\"%c\"",*((gchar*)(model + attr->offset)));
	  break;
	case ODN_MODEL_TYPE_STRING:
	  if(*((gchar**)(model + attr->offset)))
	    g_string_append_printf(buffer,"\"%s\"",*((gchar**)(model + attr->offset)));
	  else
	    g_string_append(buffer,"null");
	  break;
	case ODN_MODEL_TYPE_FLOAT:
	  g_string_append_printf(buffer,"%g",*((gfloat*)(model + attr->offset)));
	  break;
	case ODN_MODEL_TYPE_DOUBLE:
	  g_string_append_printf(buffer,"%g",*((gdouble*)(model + attr->offset)));
	  break;
	case ODN_MODEL_TYPE_INT16:
	  g_string_append_printf(buffer,"%d",*((gint16*)(model + attr->offset)));
	  break;
	case ODN_MODEL_TYPE_INT32:
	  g_string_append_printf(buffer,"%d",*((gint32*)(model + attr->offset)));
	  break;
	case ODN_MODEL_TYPE_INT64:
	  g_string_append_printf(buffer,"%ld",*((gint64*)(model + attr->offset)));
	  break;
	case ODN_MODEL_TYPE_LIST:
	  {
	    GList * list = g_list_first(*((GList**)(model + attr->offset)));
	    g_string_append(buffer,"[");
	    for(GList * iter = list;iter;iter = g_list_next(iter)){
		if(iter != list)
		  g_string_append(buffer,",");
		if(iter->data)
		  {
		    gchar * model_string = odn_model_to_string(iter->data);
		    g_string_append(buffer,model_string);
		    g_free(model_string);
		  }
		else
		  {
		    g_string_append(buffer,"null");
		  }
	    }
	    g_string_append(buffer,"]");
	  }
	  break;
	case ODN_MODEL_TYPE_INVALID:
	  g_string_append(buffer,"undefined");
	  break;
      }
    }
  g_string_append(buffer,"}");
  return g_string_free(buffer,FALSE);
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

void
odn_model_add_null(gpointer model,
		 const gchar * name)
{
  OdnModelAttribute * attr = odn_model_find(model,name);
  if(attr)
    {
      GList ** attr_ptr = ((GList **)(model + attr->offset));
      if(attr->klass)
	{
	  *attr_ptr = g_list_append(*attr_ptr,NULL);
	}
    }
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
