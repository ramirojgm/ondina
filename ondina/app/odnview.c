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

struct _OdnViewPrivate
{
  gchar * layout_name;
  OdnViewContainer * root;
};

G_DEFINE_TYPE_WITH_PRIVATE(OdnView,odn_view,G_TYPE_OBJECT)

static OdnModelClass OdnViewModelRootClass = {
  "ViewModelRoot",
  1,
  {
      {"root", ODN_MODEL_TYPE_LIST,NULL,offsetof(OdnViewModelRoot,root) }
  },
  sizeof(OdnViewModelRoot)
};

OdnModelClass *
odn_view_model_root_get_class()
{
  return &OdnViewModelRootClass;
}

static void odn_view_text_render(OdnViewItem * item,
				 OdnViewRenderContext * context);

static void odn_view_text_free(OdnViewItem * item);

static void odn_view_value_render(OdnViewItem * item,
				  OdnViewRenderContext * context);

static void odn_view_value_free(OdnViewItem * item);

static void odn_view_content_render(OdnViewItem * item,
				    OdnViewRenderContext * context);

static void odn_view_content_free(OdnViewItem * item);

static void odn_view_element_attribute_render(OdnViewItem * item,
					     OdnViewRenderContext * context);

static void odn_view_element_attribute_free(OdnViewItem * item);

static void odn_view_body_render(OdnViewItem * item,
				 OdnViewRenderContext * context);

static void odn_view_body_free(OdnViewItem * item);

static void odn_view_element_render(OdnViewItem * item,
				 OdnViewRenderContext * context);

static void odn_view_element_free(OdnViewItem * item);

static void odn_view_foreach_render(OdnViewItem * item,
				 OdnViewRenderContext * context);

static void odn_view_foreach_free(OdnViewItem * item);

static void odn_view_root_render(OdnViewItem * item,
				 OdnViewRenderContext * context);

static void odn_view_root_free(OdnViewItem * item);

static OdnViewBody * odn_view_body_new();

static OdnViewContent * odn_view_content_new(const gchar * text);

static OdnViewItemClass odn_view_text_class = {
    odn_view_text_render,
    odn_view_text_free
};

static OdnViewItemClass odn_view_value_class = {
    odn_view_value_render,
    odn_view_value_free
};

static OdnViewItemClass odn_view_content_class = {
    odn_view_content_render,
    odn_view_content_free
};

static OdnViewItemClass odn_view_element_attribute_class = {
    odn_view_element_attribute_render,
    odn_view_element_attribute_free
};

static OdnViewItemClass odn_view_body_class = {
    odn_view_body_render,
    odn_view_body_free
};

static OdnViewItemClass odn_view_element_class = {
    odn_view_element_render,
    odn_view_element_free
};

static OdnViewItemClass odn_view_foreach_class = {
    odn_view_foreach_render,
    odn_view_foreach_free
};

static OdnViewItemClass odn_view_root_class = {
    odn_view_root_render,
    odn_view_root_free
};

static void
odn_view_init(OdnView * self)
{
  self->priv = odn_view_get_instance_private(self);
  self->priv->root = g_new(OdnViewContainer,1);
  self->priv->root->parent.klass = &odn_view_root_class;
  self->priv->root->body = odn_view_body_new();
}

static void
odn_view_dispose(GObject * object)
{
  OdnView * self = ODN_VIEW(object);
  odn_view_item_free((OdnViewItem*)self->priv->root);
  G_OBJECT_CLASS(odn_view_parent_class)->dispose(object);
}

static void
odn_view_class_init(OdnViewClass * klass)
{
  G_OBJECT_CLASS(klass)->dispose = odn_view_dispose;
}

OdnView *
odn_view_new()
{
  return ODN_VIEW(g_object_new(ODN_TYPE_VIEW,NULL));
}

gboolean
odn_view_load_from_file(OdnView * view,
			const gchar * filename,
			GError ** error)
{
  gchar * content = NULL;
  if(g_file_get_contents(filename,&content,NULL,error))
    {
      gboolean done = odn_view_load_from_data(view,content,error);
      g_free(content);
      return done;
    }
  return FALSE;
}


const gchar *
odn_view_get_layout(OdnView * view)
{
  return view->priv->layout_name;
}

void
odn_view_render(OdnView * view,
		OdnViewRenderContext * context)
{
  odn_view_item_render((OdnViewItem*)view->priv->root,context);
}

/* OdnViewRenderContext */

OdnViewRenderContext *
odn_view_render_context_new(OdnApplication * app,
			   gpointer model)
{
  OdnViewRenderContext * context = g_new0(OdnViewRenderContext,1);
  context->app = ODN_APPLICATION(g_object_ref(app));
  context->buffer = g_string_new(NULL);
  context->data = g_queue_new();
  g_queue_push_tail(context->data,model);
  return context;
}

void
odn_view_render_context_append(OdnViewRenderContext* context,
			       const gchar * str)
{
  g_string_append(context->buffer,str);
}

void
odn_view_render_context_printf(OdnViewRenderContext * context,
			       const gchar * format,
			       ...)
{
  va_list vargs;
  va_start(vargs,format);
  gchar * new_value = g_strdup_vprintf(format,vargs);
  g_string_append(context->buffer,new_value);
  g_free(new_value);
  va_end(vargs);
}

OdnApplication*
odn_view_render_context_get_application(OdnViewRenderContext * context)
{
  return context->app;
}

const gchar *
odn_view_render_context_get_content(OdnViewRenderContext * context)
{
  return context->buffer->str;
}

void
odn_view_render_context_free(OdnViewRenderContext * context)
{
  g_object_unref(context->app);
  g_queue_free_full(context->data,odn_model_free);
  g_string_free(context->buffer,TRUE);
  g_free(context);
}

/* view item */

void
odn_view_item_render(OdnViewItem * item,
		     OdnViewRenderContext * context)
{
  if(item->klass->render)
    item->klass->render(item,context);
}

void
odn_view_item_free(OdnViewItem * item)
{
  item->klass->free(item);
}

/* view container */

void
odn_view_container_add(OdnViewContainer * container,
		       OdnViewItem * item)
{
  container->body->items = g_list_append(container->body->items,item);
}

/* GMarkupParser */

static const gchar *
odn_view_markup_attribute(const gchar * attribute_name,
			  const gchar **attribute_names,
			  const gchar **attribute_values)
{
  for(guint index = 0;attribute_names[index];index++)
    {
      if(g_strcmp0(attribute_names[index],attribute_name) == 0)
	return attribute_values[index];
    }
  return NULL;
}


static void
odn_view_markup_start_element  (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
				GError             **error)
{
  GQueue * queue = (GQueue*)user_data;
  OdnView * view = ODN_VIEW(g_queue_peek_head(queue));
  OdnViewContainer * current = (OdnViewContainer *)g_queue_peek_tail(queue);

  if(g_strcmp0(element_name,"view") == 0)
    {
      const gchar * layout_name = odn_view_markup_attribute("layout",
							    attribute_names,
							    attribute_values);
      if(layout_name)
	view->priv->layout_name = g_strdup(layout_name);
      else
	view->priv->layout_name = NULL;
    }
  else if(g_strcmp0(element_name,"foreach") == 0)
    {
      OdnViewForeach * foreach = g_new(OdnViewForeach,1);
      foreach->parent.parent.klass = &odn_view_foreach_class;
      foreach->parent.body = odn_view_body_new();
      const gchar * member = odn_view_markup_attribute("member",
						    attribute_names,
						    attribute_values);
      if(member)
	foreach->member = g_strdup(member);
      else
	foreach->member = g_strdup("root");
      odn_view_container_add(current,(OdnViewItem*)foreach);
      g_queue_push_tail(queue,foreach);
    }
  else
    {
      OdnViewElement * element = g_new(OdnViewElement,1);
      element->parent.parent.klass = &odn_view_element_class;
      element->parent.body = odn_view_body_new();
      element->name = g_strdup(element_name);
      element->attribute = NULL;
      for(guint index = 0;attribute_names[index];index++)
	{
	  OdnViewElementAttribute * attribute = g_new(OdnViewElementAttribute,1);
	  attribute->parent.klass = &odn_view_element_attribute_class;
	  attribute->name = g_strdup(attribute_names[index]);
	  if(g_utf8_strlen(attribute_values[index],100) > 0)
	      attribute->content = odn_view_content_new(attribute_values[index]);
	  else
	      attribute->content = NULL;
	  element->attribute = g_list_append(element->attribute,attribute);
	}
      odn_view_container_add(current,(OdnViewItem*)element);
      g_queue_push_tail(queue,element);
    }
}


static void
odn_view_markup_end_element(GMarkupParseContext *context,
			    const gchar         *element_name,
			    gpointer             user_data,
			    GError             **error)
{
  GQueue * queue = (GQueue*)user_data;
  g_queue_pop_tail(queue);
}

static void
odn_view_markup_text(GMarkupParseContext *context,
		    const gchar         *text,
		    gsize                text_len,
		    gpointer             user_data,
		    GError             **error)
{
  GQueue * queue = (GQueue*)user_data;
  OdnViewContainer * current = (OdnViewContainer *)g_queue_peek_tail(queue);
  odn_view_container_add(current,(OdnViewItem*)odn_view_content_new(text));
}

static GMarkupParser odn_view_markup_parser = {
    odn_view_markup_start_element,
    odn_view_markup_end_element,
    odn_view_markup_text,
    NULL,
    NULL
};

gboolean
odn_view_load_from_data(OdnView * view,
			const gchar * data,
			GError ** error)
{
  GQueue * parser_data = g_queue_new();
  GMarkupParseContext * context = g_markup_parse_context_new(
      &odn_view_markup_parser,
      G_MARKUP_TREAT_CDATA_AS_TEXT,
      parser_data,
      (GDestroyNotify)g_queue_free);
  g_queue_push_head(parser_data,view);
  g_queue_push_tail(parser_data,view->priv->root);
  gboolean done = g_markup_parse_context_parse(context,data,-1,error);
  g_markup_parse_context_free(context);
  return done;
}

static void
odn_view_text_render(OdnViewItem * item,
		     OdnViewRenderContext * context)
{
  odn_view_render_context_append(context,((OdnViewText*)item)->text);
}

static void
odn_view_text_free(OdnViewItem * item)
{

}

static void
odn_view_value_render(OdnViewItem * item,
		      OdnViewRenderContext * context)
{
  OdnViewValue * self = (OdnViewValue *)item;
  gpointer data = g_queue_peek_tail(context->data);

  if(odn_model_contains(data,self->name))
    {
      switch(self->type)
      {
	case ODN_VIEW_VALUE_INT:
	  odn_view_render_context_printf(context,
					 "%d",
					 odn_model_get_int(data,self->name));
	  break;
	case ODN_VIEW_VALUE_DOUBLE:
	  odn_view_render_context_printf(context,
					 "%g",
					 odn_model_get_double(data,self->name));
	  break;
	case ODN_VIEW_VALUE_STRING:
	  {
	    gchar * str = g_markup_escape_text(
		odn_model_get_string(data,self->name),
		-1);
	    odn_view_render_context_append(context,
					   str);
	    g_free(str);
	  }
	  break;
	case ODN_VIEW_VALUE_RAW:
	  odn_view_render_context_append(context,
					 odn_model_get_string(data,self->name));
	  break;
	case ODN_VIEW_VALUE_BASE64:
	  break;
      }
    }
}

static void
odn_view_value_free(OdnViewItem * item)
{

}

static void
odn_view_content_render(OdnViewItem * item,
			OdnViewRenderContext * context)
{
  OdnViewContent * self = (OdnViewContent*)(item);
  for(GList * iter = g_list_first(self->content); iter; iter = g_list_next(iter))
    {
      OdnViewItem * child = (OdnViewItem*)iter->data;
      odn_view_item_render(child,context);
    }
}

static void
odn_view_content_free(OdnViewItem * item)
{

}

static void
odn_view_element_attribute_render(OdnViewItem * item,
				 OdnViewRenderContext * context)
{
  OdnViewElementAttribute * self = (OdnViewElementAttribute*)(item);
  odn_view_render_context_printf(context,"%s=\"",self->name);
  odn_view_item_render((OdnViewItem*)self->content,context);
  odn_view_render_context_append(context,"\"");
}

static void
odn_view_element_attribute_free(OdnViewItem * item)
{

}

static void
odn_view_body_render(OdnViewItem * item,
		     OdnViewRenderContext * context)
{
  OdnViewBody * self = (OdnViewBody*)(item);
  for(GList * iter = g_list_first(self->items); iter; iter = g_list_next(iter))
    {
      OdnViewItem * child = (OdnViewItem*)iter->data;
      odn_view_item_render(child,context);
    }
}

static void
odn_view_body_free(OdnViewItem * item)
{

}

static void
odn_view_element_render(OdnViewItem * item,
		       OdnViewRenderContext * context)
{
  OdnViewElement * self = (OdnViewElement*)(item);
  odn_view_render_context_printf(context,"<%s",self->name);
  for(GList * iter = g_list_first(self->attribute); iter; iter = g_list_next(iter))
    {
      OdnViewItem * attribute = (OdnViewItem*)iter->data;
      odn_view_render_context_printf(context," ",self->name);
      odn_view_item_render(attribute,context);
    }

  if(self->parent.body->items)
    {
      odn_view_render_context_append(context,">");
      odn_view_item_render((OdnViewItem*)self->parent.body,context);
      odn_view_render_context_printf(context,"</%s>",self->name);
    }
  else
    {
      odn_view_render_context_append(context," />");
    }
}

static void
odn_view_element_free(OdnViewItem * item)
{

}

static void
odn_view_foreach_render(OdnViewItem * item,
			 OdnViewRenderContext * context)
{
  OdnViewForeach * self = (OdnViewForeach*)(item);
  gpointer data = g_queue_peek_tail(context->data);

  if(odn_model_contains(data,self->member))
    {
      GList * rows = g_list_first(odn_model_get_list(data,self->member));
      for(GList * row = rows;row;row = g_list_next(row))
	{
	  g_queue_push_tail(context->data,row->data);
	  odn_view_item_render((OdnViewItem*)self->parent.body,context);
	  g_queue_pop_tail(context->data);
	}
    }
}

static void
odn_view_foreach_free(OdnViewItem * item)
{

}


static void
odn_view_root_render(OdnViewItem * item,
		     OdnViewRenderContext * context)
{
  OdnViewContainer * self = (OdnViewContainer*)(item);
  odn_view_item_render((OdnViewItem*)self->body,context);
}

static void
odn_view_root_free(OdnViewItem * item)
{

}

static OdnViewBody *
odn_view_body_new()
{
  OdnViewBody * body = g_new(OdnViewBody,1);
  body->parent.klass = &odn_view_body_class;
  body->items = NULL;
  return body;
}

static OdnViewContent *
odn_view_content_new(const gchar * text)
{
  OdnViewContent * content = g_new(OdnViewContent,1);
  content->parent.klass = &odn_view_content_class;
  content->content = NULL;
  const gchar * static_offset = text;
  for(const gchar * iter = text;*iter;iter++)
    {
      if(*iter == '@')
	{
	  if(static_offset != iter)
	    {
	      OdnViewText * text = g_new(OdnViewText,1);
	      text->parent.klass = &odn_view_text_class;
	      text->text = g_strndup(static_offset,iter-static_offset);
	      content->content = g_list_append(content->content,text);
	    }

	  const gchar * data_start = strstr(iter,"(");
	  if(!data_start)
	    data_start="";
	  const gchar * data_end = strstr(data_start,")");

	  if(data_start && data_end)
	    {
	      OdnViewValue * value = g_new(OdnViewValue,1);
	      value->parent.klass = &odn_view_value_class;
	      if(g_str_has_prefix(iter,"@int("))
		{
		  value->type = ODN_VIEW_VALUE_INT;
		  value->name = g_strndup(iter + 5,data_end - (iter + 5));
		}
	      else if(g_str_has_prefix(iter,"@double("))
		{
		  value->type = ODN_VIEW_VALUE_DOUBLE;
		  value->name = g_strndup(iter + 8,data_end - (iter + 8));
		}
	      else if(g_str_has_prefix(iter,"@string("))
		{
		  value->type = ODN_VIEW_VALUE_STRING;
		  value->name = g_strndup(iter + 8,data_end - (iter + 8));
		}
	      else if(g_str_has_prefix(iter,"@base64("))
		{
		  value->type = ODN_VIEW_VALUE_BASE64;
		  value->name = g_strndup(iter + 8,data_end - (iter + 8));
		}
	      else if(g_str_has_prefix(iter,"@raw("))
		{
		  value->type = ODN_VIEW_VALUE_RAW;
		  value->name = g_strndup(data_start + 1, data_end - (data_start + 1));
		}
	      else
		{
		  g_free(value);
		  value = NULL;
		}
	      static_offset = data_end + 1;
	      if(value)
		content->content = g_list_append(content->content,value);
	    }
	  else
	    {
	      static_offset = iter;
	    }
	}
    }

  if(static_offset != (text + g_utf8_strlen(text,G_MAXUINT32)))
    {
      OdnViewText * text = g_new(OdnViewText,1);
      text->parent.klass = &odn_view_text_class;
      text->text = g_strdup(static_offset);
      content->content = g_list_append(content->content,text);
    }
  return content;
}
