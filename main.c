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

gint
main(gint argc,gchar ** argv)
{
  OdnApplication * app = odn_application_new("org.gnome.service.ondina");

  odn_application_bind_inet_port(app,8080);

  odn_application_set_session_type(app,COMMON_TYPE_SESSION);

  odn_application_bind_content(app,"/","application/content");

  odn_application_bind_dynamic(app,"application/dynamic");

  odn_application_bind_controller(app,"/default",CONTROLLER_TYPE_DEFAULT);
  odn_application_bind_controller(app,"/client",CONTROLLER_TYPE_CLIENT);


  return g_application_run(G_APPLICATION(app),argc,argv);
}
