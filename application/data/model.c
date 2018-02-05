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

static OdnModelClass ClientModelClass = {
    "ClientModel",
    7,
    {
	{ "idclient" , ODN_MODEL_TYPE_INT32, NULL,offsetof(ClientModel,idclient) },
	{ "ident" , ODN_MODEL_TYPE_STRING, NULL,offsetof(ClientModel,ident) },
	{ "name" , ODN_MODEL_TYPE_STRING, NULL,offsetof(ClientModel,name) },
	{ "address" , ODN_MODEL_TYPE_STRING, NULL,offsetof(ClientModel,address) },
	{ "phone" , ODN_MODEL_TYPE_STRING, NULL,offsetof(ClientModel,phone) },
	{ "email" , ODN_MODEL_TYPE_STRING, NULL,offsetof(ClientModel,email) },
	{ "contact" , ODN_MODEL_TYPE_STRING, NULL,offsetof(ClientModel,contact) }
    },
    sizeof(ClientModel)
};

OdnModelClass *
client_model_get_class(void)
{
  return &ClientModelClass;
}
