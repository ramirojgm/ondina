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

#ifndef APPLICATION_MODEL_H_
#define APPLICATION_MODEL_H_

#define CLIENT_MODEL (client_model_get_class())

typedef struct
{
  OdnModel parent;
  gint32 idclient;
  gchar * ident;
  gchar * name;
  gchar * address;
  gchar * phone;
  gchar * email;
  gchar * contact;
} ClientModel;

typedef struct
{
  OdnModel parent;
  gint32  iduser;
  gchar * name;
  gchar * password;
} UserModel;

OdnModelClass * client_model_get_class(void);

OdnModelClass * user_model_get_class(void);

#endif /* APPLICATION_MODEL_H_ */
