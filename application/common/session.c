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
};

G_DEFINE_TYPE_WITH_PRIVATE(CommonSession,common_session,G_TYPE_OBJECT)

static void
common_session_init(CommonSession * self)
{
  self->priv = common_session_get_instance_private(self);
  self->priv->authenticated = FALSE;
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

