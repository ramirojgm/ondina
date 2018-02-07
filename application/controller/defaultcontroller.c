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

struct _ControllerDefaultPrivate
{
  gpointer padding[12];
};

G_DEFINE_TYPE_WITH_PRIVATE(ControllerDefault,controller_default,ODN_TYPE_CONTROLLER)

/* default controller */

static OdnResult *
controller_default_index(ControllerDefault * self,
			OdnContext * context,
			GError ** error)
{
  return NULL;
}


static OdnResult *
controller_default_login(ControllerDefault * self,
			OdnContext * context,
			GError ** error)
{
  OdnApplication * app = odn_context_get_application(context);
  g_autofree gchar * content = odn_application_apply_layout(app,"login.layout","");
  return odn_content_result_new(content,-1,"text/html");
}

static OdnResult *
controller_default_do_login(ControllerDefault * self,
			OdnContext * context,
			GError ** error)
{
  CommonSession * session = odn_context_get_session(context);
  common_session_set_authenticated(session,TRUE);

  return odn_redirect_result_new("/client");
}

static OdnResult *
controller_default_logout(ControllerDefault * self,
			OdnContext * context,
			GError ** error)
{
  CommonSession * session = odn_context_get_session(context);
  common_session_set_authenticated(session,FALSE);

  return odn_redirect_result_new("/default/login");
}



/* default type implementation */

static void
controller_default_init(ControllerDefault * self)
{
  self->priv = controller_default_get_instance_private(self);

  /* controller action */

 odn_controller_bind(ODN_CONTROLLER(self),
		     "/index",
		     (OdnControllerAction)controller_default_index,
		     NULL,
		     NULL);

 odn_controller_bind(ODN_CONTROLLER(self),
 		     "/do_login",
 		     (OdnControllerAction)controller_default_do_login,
 		     NULL,
		     NULL);

 odn_controller_bind(ODN_CONTROLLER(self),
 		     "/login",
 		     (OdnControllerAction)controller_default_login,
 		     NULL,
		     NULL);

 odn_controller_bind(ODN_CONTROLLER(self),
  		     "/logout",
  		     (OdnControllerAction)controller_default_logout,
  		     NULL,
  		     NULL);
}

static void
controller_default_class_init(ControllerDefaultClass * klass)
{

}
