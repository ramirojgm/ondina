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

GList *
client_model_get_all(sqlite3 * db,
		     GError ** error)
{
  sqlite3_stmt * stmt = NULL;
  GList * result = NULL;
  if(sqlite3_prepare(db,"SELECT idclient,ident,name,address,phone,email,contact FROM client",-1,&stmt,NULL) == SQLITE_OK)
    {
      while(sqlite3_step(stmt) == SQLITE_ROW)
	{
	  ClientModel * model = odn_model_new(CLIENT_MODEL);
	  model->idclient = sqlite3_column_int(stmt,0);
	  model->ident = g_strdup((gchar*)sqlite3_column_text(stmt,1));
	  model->name = g_strdup((gchar*)sqlite3_column_text(stmt,2));
	  model->address = g_strdup((gchar*)sqlite3_column_text(stmt,3));
	  model->phone = g_strdup((gchar*)sqlite3_column_text(stmt,4));
	  model->email = g_strdup((gchar*)sqlite3_column_text(stmt,5));
	  model->contact = g_strdup((gchar*)sqlite3_column_text(stmt,6));
	  result = g_list_append(result,model);
	}
      sqlite3_finalize(stmt);
    }
  return result;
}

ClientModel *
client_model_get(sqlite3 * db,
		 gint idclient,
		 GError ** error)
{
  sqlite3_stmt * stmt = NULL;
  ClientModel * model = NULL;
  if(sqlite3_prepare(db,"SELECT idclient,ident,name,address,phone,email,contact FROM client WHERE idclient = ?",-1,&stmt,NULL) == SQLITE_OK)
    {
      sqlite3_bind_int(stmt,1,idclient);
      if(sqlite3_step(stmt) == SQLITE_ROW)
      {
	model = odn_model_new(CLIENT_MODEL);
	model->idclient = sqlite3_column_int(stmt,0);
	model->ident = g_strdup((gchar*)sqlite3_column_text(stmt,1));
	model->name = g_strdup((gchar*)sqlite3_column_text(stmt,2));
	model->address = g_strdup((gchar*)sqlite3_column_text(stmt,3));
	model->phone = g_strdup((gchar*)sqlite3_column_text(stmt,4));
	model->email = g_strdup((gchar*)sqlite3_column_text(stmt,5));
	model->contact = g_strdup((gchar*)sqlite3_column_text(stmt,6));
      }
      sqlite3_finalize(stmt);
    }
  return model;
}

gboolean
client_model_insert(sqlite3 * db,
		   ClientModel * model,
		   GError ** error)
{
  g_print("%s",model->name);

  return TRUE;
}

gboolean
client_model_update(sqlite3 * db,
		   ClientModel * model,
		   GError ** error)
{
  g_print("%s",model->name);
  return TRUE;
}

gboolean
client_model_delete(sqlite3 * db,
		   ClientModel * model,
		   GError ** error)
{

}
