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

static OdnModelClass UserModelClass = {
    "UserModel",
    3,
    {
	{ "iduser" , ODN_MODEL_TYPE_INT32, NULL,offsetof(UserModel,iduser) },
	{ "name" , ODN_MODEL_TYPE_STRING, NULL,offsetof(UserModel,name) },
	{ "password" ,ODN_MODEL_TYPE_STRING, NULL,offsetof(UserModel,password) },
    },
    sizeof(UserModel)
};



OdnModelClass *
user_model_get_class(void)
{
  return &UserModelClass;
}


gboolean
user_model_check(sqlite3 * db,
	       UserModel * model,
	       GError ** error)
{
  gboolean exists = FALSE;
  sqlite3_stmt * stmt = NULL;
  if(sqlite3_prepare(db,"SELECT iduser FROM user WHERE name = ? AND password = ?",-1,&stmt,NULL) == SQLITE_OK)
    {
      sqlite3_bind_text(stmt,1,model->name,-1,NULL);
      sqlite3_bind_text(stmt,2,model->password,-1,NULL);
      if(sqlite3_step(stmt) == SQLITE_ROW)
	{
	  model->iduser = sqlite3_column_int(stmt,0);
	  exists = TRUE;
	}
      sqlite3_finalize(stmt);
    }
  return exists;
}
