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

#ifndef ONDINA_CACHE_H_
#define ONDINA_CACHE_H_

#define ODN_TYPE_CACHE	(odn_cache_get_type())
G_DECLARE_FINAL_TYPE(OdnCache,odn_cache,ODN,CACHE,GObject)

typedef struct _OdnCachePrivate OdnCachePrivate;

struct _OdnCache
{
  GObject parent_instance;

  /* private */
  OdnCachePrivate * priv;
};



GType		odn_cache_get_type(void);

OdnCache *	odn_cache_new_from_file(const gchar * filename,
					GError ** error);

const gchar *	odn_cache_get_mime_type(OdnCache * cache);

gconstpointer	odn_cache_get_content(OdnCache * cache,
				      OdnCompressionType compression_type,
				      gsize * size);

#endif /* ONDINA_CACHE_H_ */
