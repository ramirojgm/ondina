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

#ifndef COMMON_SESSION_H_
#define COMMON_SESSION_H_

#include <sqlite3.h>

#define COMMON_TYPE_SESSION	(common_session_get_type())
G_DECLARE_FINAL_TYPE(CommonSession,common_session,COMMON,SESSION,GObject)

typedef struct _CommonSessionPrivate CommonSessionPrivate;

struct _CommonSessionClass
{
  GObjectClass parent_class;
};

struct _CommonSession
{
  GObject parent_instance;

  CommonSessionPrivate * priv;
};


GType	common_session_get_type(void);

void	 common_session_set_authenticated(CommonSession * session,gboolean setting);

gboolean common_session_get_authenticated(CommonSession * session);

sqlite3 * common_session_get_database(CommonSession * session);

#endif /* COMMON_SESSION_H_ */
