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

struct _CommonSessionPrivate
{
  gboolean authenticated;
  sqlite3 * db;
};

G_DEFINE_TYPE_WITH_PRIVATE(CommonSession,common_session,G_TYPE_OBJECT)

static void
common_session_init(CommonSession * self)
{
  gboolean exists = g_file_test("data.db",G_FILE_TEST_EXISTS);

  self->priv = common_session_get_instance_private(self);
  self->priv->authenticated = FALSE;

  sqlite3_open("data.db",&(self->priv->db));
  if(!exists)
    {
      sqlite3_exec(self->priv->db,"CREATE TABLE user (iduser INTEGER PRIMARY KEY,name TEXT,password TEXT);",NULL,NULL,NULL);
      sqlite3_exec(self->priv->db,"INSERT INTO user (name,password) VALUES ('admin','');",NULL,NULL,NULL);
      sqlite3_exec(self->priv->db,"CREATE TABLE client (idclient INTEGER PRIMARY KEY,ident TEXT,name TEXT,address TEXT,phone TEXT,email TEXT,contact TEXT);",NULL,NULL,NULL);
    }
}

static void
common_session_class_init(CommonSessionClass * klass)
{

}

void
common_session_set_authenticated(CommonSession * session,gboolean setting)
{
  session->priv->authenticated = setting;
}

gboolean
common_session_get_authenticated(CommonSession * session)
{
  return session->priv->authenticated;
}

sqlite3 *
common_session_get_database(CommonSession * session)
{
  return session->priv->db;
}
