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

#include <application.h>

struct _ControllerClientPrivate
{
  gpointer padding[12];
};

G_DEFINE_TYPE_WITH_PRIVATE(ControllerClient,controller_client,ODN_TYPE_CONTROLLER)

/* client controller */

static OdnResult *
controller_client_index(ControllerClient * self,
			OdnContext * context,
			GError ** error)
{
  OdnViewModelRoot * model = (OdnViewModelRoot *)odn_model_new(ODN_VIEW_MODEL_ROOT);
  for(guint index = 0;index < 100; index++)
    {
      ClientModel * row = odn_model_new(CLIENT_MODEL);
      row->idclient = index + 1;
      row->name = g_strdup("Ninguna");
      row->ident = g_strdup("Ninguna");
      row->phone = g_strdup("Ninguna");
      row->address = g_strdup("Ninguna");
      row->email = g_strdup("Ninguna");
      row->contact = g_strdup("Ninguna");
      model->root = g_list_append(model->root,row);
    }

  return odn_view_result_new(
      "client.view",
       model);
}


/* client type implementation */

static void
controller_client_init(ControllerClient * self)
{
  self->priv = controller_client_get_instance_private(self);

  /* controller action */

 odn_controller_bind(ODN_CONTROLLER(self),
		     "/index",
		     (OdnControllerAction)controller_client_index,
		     NULL,
		     NULL);

}

static void
controller_client_class_init(ControllerClientClass * klass)
{

}
