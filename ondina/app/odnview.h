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

#ifndef ONDINA_VIEW_H_
#define ONDINA_VIEW_H_

#define ODN_TYPE_VIEW	(odn_view_get_type())

G_DECLARE_FINAL_TYPE(OdnView,odn_view,ODN,VIEW,GObject)

#define ODN_VIEW_MODEL_ROOT	(odn_view_model_root_get_class())

typedef struct _OdnViewPrivate OdnViewPrivate;
typedef struct _OdnViewItem OdnViewItem;
typedef struct _OdnViewItemClass OdnViewItemClass;

typedef enum
{
  ODN_VIEW_VALUE_INT,
  ODN_VIEW_VALUE_DOUBLE,
  ODN_VIEW_VALUE_STRING,
  ODN_VIEW_VALUE_RAW,
  ODN_VIEW_VALUE_BASE64,
} OdnViewValueType;

struct _OdnView
{
  GObject parent_instance;
  /* private */
  OdnViewPrivate * priv;
};

struct _OdnViewClass
{
  GObjectClass parent_class;
};

struct _OdnViewRenderContext
{
  GString * buffer;
  OdnApplication * app;
  GQueue * data;
};

struct _OdnViewItem
{
  OdnViewItemClass * klass;
};

struct _OdnViewItemClass
{
  void (*render)(OdnViewItem * item,OdnViewRenderContext * context);
  void (*free)(OdnViewItem * item);
};

typedef struct
{
  OdnViewItem parent;
  gchar * text;
} OdnViewText;

typedef struct
{
  OdnViewItem parent;
  gchar * name;
  OdnViewValueType type;
} OdnViewValue;

typedef struct
{
  OdnViewItem parent;
  GList * content;
} OdnViewContent;

typedef struct
{
  OdnViewItem parent;
  gchar * name;
  OdnViewContent * content;
} OdnViewElementAttribute;

typedef struct
{
  OdnViewItem parent;
  GList * items;
} OdnViewBody;

typedef struct
{
  OdnViewItem parent;
  OdnViewBody * body;
} OdnViewContainer;

typedef struct
{
  OdnViewContainer parent;
  gchar * name;
  GList * attribute;
} OdnViewElement;

typedef struct
{
  OdnViewContainer parent;
  gchar * member;
} OdnViewForeach;

typedef struct
{
  OdnModel model;
  GList * root;
} OdnViewModelRoot;


OdnModelClass * odn_view_model_root_get_class(void);


GType		odn_view_get_type(void);

OdnView *	odn_view_new();

gboolean	odn_view_load_from_file(OdnView * view,
					const gchar * filename,
					GError ** error);

gboolean	odn_view_load_from_data(OdnView * view,
					const gchar * data,
					GError ** error);

const gchar *	odn_view_get_layout(OdnView * view);

void		odn_view_render(OdnView * view,
				OdnViewRenderContext * context);

/* OdnViewRenderContext */

OdnViewRenderContext * odn_view_render_context_new(OdnApplication * app,
						   gpointer data);

void		odn_view_render_context_append(OdnViewRenderContext* context,
					       const gchar * str);

void		odn_view_render_context_printf(OdnViewRenderContext * context,
					       const gchar * format,
					       ...);

OdnApplication* odn_view_render_context_get_application(OdnViewRenderContext * context);

const gchar *	odn_view_render_context_get_content(OdnViewRenderContext * context);

void		odn_view_render_context_free(OdnViewRenderContext * context);

/* view item */

void		odn_view_item_render(OdnViewItem * item,
				     OdnViewRenderContext * context);

void		odn_view_item_free(OdnViewItem * item);

/* view container */

void		odn_view_container_add(OdnViewContainer * container,
				       OdnViewItem * item);

#endif /* ONDINA_VIEW_H_ */
