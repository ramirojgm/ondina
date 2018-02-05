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

typedef struct
{
  gchar * name;
  OdnControllerAction action;
  gpointer data;
  GDestroyNotify destroy_func;
} OdnKeyAction;

struct _OdnControllerPrivate
{
  GList * action;
};

G_DEFINE_TYPE_WITH_PRIVATE(OdnController,odn_controller,G_TYPE_OBJECT)

static OdnKeyAction *
odn_key_action_new(const gchar * name,
		   OdnControllerAction action,
		   gpointer data,
		   GDestroyNotify destroy_func)
{
  OdnKeyAction * key_action = g_new(OdnKeyAction,1);
  key_action->name = g_strdup(name);
  key_action->action = action;
  key_action->data = data;
  key_action->destroy_func = destroy_func;
  return key_action;
}

static OdnKeyAction *
odn_key_action_find(GList * list,
		    const gchar * key)
{
  for(GList * iter = g_list_first(list);iter;iter = g_list_next(iter))
    {
      OdnKeyAction * key_action = (OdnKeyAction *)iter->data;
      if(g_strcmp0(key_action->name,key) == 0)
	return key_action;
    }
  return NULL;
}

static void
odn_key_action_free(OdnKeyAction * action)
{
    g_free(action->name);
    if(action->data && action->destroy_func)
      action->destroy_func(action->data);
    g_free(action);
}

static void
odn_controller_init(OdnController * self)
{

}

static void
odn_controller_dispose(GObject * object)
{
  OdnControllerPrivate * priv =
	odn_controller_get_instance_private(ODN_CONTROLLER(object));
  g_list_free_full(priv->action,(GDestroyNotify)odn_key_action_free);
  G_OBJECT_CLASS(odn_controller_parent_class)->dispose(object);
}

static void
odn_controller_class_init(OdnControllerClass * klass)
{
  G_OBJECT_CLASS(klass)->dispose = odn_controller_dispose;
}

void
odn_controller_bind(OdnController * controller,
		    const gchar * name,
		    OdnControllerAction action,
		    gpointer data,
		    GDestroyNotify destroy_func)
{
    OdnControllerPrivate * priv =
	odn_controller_get_instance_private(controller);

    priv->action = g_list_append(priv->action,
				 odn_key_action_new(
				     name,
				     action,data,
				     destroy_func));
}

gboolean
odn_controller_has(OdnController * controller,
		   const gchar * name)
{
  OdnControllerPrivate * priv =
	odn_controller_get_instance_private(controller);

  return odn_key_action_find(priv->action,name) != NULL;
}

OdnResult *
odn_controller_invoke(OdnController * controller,
		      const gchar * action_name,
		      OdnContext * context,
		      GError ** error)
{
  OdnControllerPrivate * priv =
  	odn_controller_get_instance_private(controller);

  OdnKeyAction * key_action = odn_key_action_find(priv->action,action_name);
  if(key_action && key_action->action)
    return key_action->action(controller,context,error,key_action->data);
  else
    return NULL;
}
