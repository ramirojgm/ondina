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

#ifndef CONTROLLER_DEFAULT_H_
#define CONTROLLER_DEFAULT_H_

#define CONTROLLER_TYPE_DEFAULT	(controller_default_get_type())
G_DECLARE_FINAL_TYPE(ControllerDefault,controller_default,CONTROLLER,DEFAULT,OdnController)

typedef struct _ControllerDefaultPrivate ControllerDefaultPrivate;

struct _ControllerDefault
{
  OdnController parent_instance;

  /* private */
  ControllerDefaultPrivate * priv;
};

struct _ControllerDefaultClass
{
  OdnControllerClass parent_class;

};

GType	controller_default_get_type(void);

#endif /* CONTROLLER_DEFAULT_H_ */
