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

#ifndef ONDINA_CONTROLLER_H_
#define ONDINA_CONTROLLER_H_

#define ODN_TYPE_CONTROLLER	(odn_controller_get_type())
G_DECLARE_DERIVABLE_TYPE(OdnController,odn_controller,ODN,CONTROLLER,GObject)

typedef struct _OdnControllerPrivate OdnControllerPrivate;

typedef OdnResult * (*OdnControllerAction)(
		      OdnController * controller,
		      OdnContext * context,
		      GError ** error,
		      gconstpointer data);

struct _OdnControllerClass
{
  GObjectClass parent_class;
};

GType		odn_controller_get_type(void);

OdnController * odn_controller_new();

void		odn_controller_bind(OdnController * controller,
				    const gchar * name,
				    OdnControllerAction action,
				    gpointer data,
				    GDestroyNotify destroy_func);

gboolean	odn_controller_has(OdnController * controller,
				   const gchar * name);

OdnResult *	odn_controller_invoke(OdnController * controller,
				      const gchar * action_name,
				      OdnContext * context,
				      GError ** error);

#endif /* ONDINA_CONTROLLER_H_ */
