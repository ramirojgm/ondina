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

#ifndef ONDINA_H_
#define ONDINA_H_

#include <glib.h>
#include <gio/gio.h>

typedef struct _OdnViewRenderContext OdnViewRenderContext;

typedef enum
{
  ODN_COMPRESSION_NONE,
  ODN_COMPRESSION_GZIP,
  ODN_COMPRESSION_DEFLATE
}OdnCompressionType;

#include "http/httppackage.h"
#include "http/httprequest.h"
#include "http/httpresponse.h"

#include "app/odnmodel.h"
#include "app/odncache.h"
#include "app/odnapplication.h"
#include "app/odnview.h"
#include "app/odncontext.h"
#include "app/odnresult.h"
#include "app/odncontroller.h"


#endif /* ONDINA_H_ */
