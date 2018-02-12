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
  CommonSession * session = odn_context_get_session(context);
  if(!common_session_get_authenticated(session))
    return odn_redirect_result_new("/");

  OdnViewModelRoot * model = (OdnViewModelRoot *)odn_model_new(ODN_VIEW_MODEL_ROOT);
  model->root = client_model_get_all(common_session_get_database(session),error);

  return odn_view_result_new(
      "client.view",
       model);
}

static OdnResult *
controller_client_get(ControllerClient * self,
			OdnContext * context,
			GError ** error)
{
  CommonSession * session = odn_context_get_session(context);
  if(common_session_get_authenticated(session))
    {
      sqlite3 * db = common_session_get_database(session);
      gint idclient = 0;

      if(odn_context_get_params(context,"idclient",G_TYPE_INT,&idclient,NULL))
	{
	  ClientModel * response = client_model_get(db,idclient,error);
	  return odn_json_result_new(response,FALSE);
	}
      return NULL;
    }
  else
    {
      return NULL;
    }
}

static OdnResult *
controller_client_insert(ControllerClient * self,
			OdnContext * context,
			GError ** error)
{
  CommonSession * session = odn_context_get_session(context);
  if(common_session_get_authenticated(session))
    {
      sqlite3 * db = common_session_get_database(session);

      ClientModel * client = odn_model_from_string(
	  CLIENT_MODEL,
	  odn_context_get_body(context),
	  odn_context_get_body_size(context)
	  ,error);

      if(client)
	{
	  client_model_insert(db,client,error);
	  return odn_json_result_new(client,FALSE);
	}

      return NULL;
    }
  else
    {
      return NULL;
    }
}

static OdnResult *
controller_client_update(ControllerClient * self,
			OdnContext * context,
			GError ** error)
{
  CommonSession * session = odn_context_get_session(context);
  if(common_session_get_authenticated(session))
    {
      sqlite3 * db = common_session_get_database(session);

      ClientModel * client = odn_model_from_string(
	  CLIENT_MODEL,
	  odn_context_get_body(context),
	  odn_context_get_body_size(context)
	  ,error);

      if(client)
	{
	  client_model_update(db,client,error);
	  return odn_json_result_new(client,FALSE);
	}

      return NULL;
    }
  else
    {
      return NULL;
    }
}

static OdnResult *
controller_client_delete(ControllerClient * self,
			OdnContext * context,
			GError ** error)
{
  CommonSession * session = odn_context_get_session(context);
  if(common_session_get_authenticated(session))
    {
      return NULL;
    }
  else
    {
      return NULL;
    }
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

 odn_controller_bind(ODN_CONTROLLER(self),
		     "/get",
		     (OdnControllerAction)controller_client_get,
		     NULL,
		     NULL);

 odn_controller_bind(ODN_CONTROLLER(self),
		     "/insert",
		     (OdnControllerAction)controller_client_insert,
		     NULL,
		     NULL);

 odn_controller_bind(ODN_CONTROLLER(self),
		     "/update",
		     (OdnControllerAction)controller_client_update,
		     NULL,
		     NULL);

 odn_controller_bind(ODN_CONTROLLER(self),
		     "/delete",
		     (OdnControllerAction)controller_client_delete,
		     NULL,
		     NULL);


}

static void
controller_client_class_init(ControllerClientClass * klass)
{

}
