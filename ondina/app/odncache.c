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


#include <ondina.h>

struct _OdnCachePrivate
{
  gchar * mime_type;

  gpointer content;
  gpointer content_gzip;
  gpointer content_deflate;

  gsize content_size;
  gsize content_gzip_size;
  gsize content_deflate_size;
};

G_DEFINE_TYPE_WITH_PRIVATE(OdnCache,odn_cache,G_TYPE_OBJECT)

static void
odn_cache_init(OdnCache * self)
{
  self->priv = odn_cache_get_instance_private(self);
}

static void
odn_cache_finalize(GObject * object)
{
  OdnCache * self = ODN_CACHE(object);
  g_free(self->priv->content);
  g_free(self->priv->content_gzip);
  g_free(self->priv->content_deflate);
  G_OBJECT_CLASS(odn_cache_parent_class)->finalize(object);
}

static void
odn_cache_class_init(OdnCacheClass * klass)
{
  klass->parent_class.finalize = odn_cache_finalize;
}

OdnCache *
odn_cache_new_from_file(const gchar * filename,
			GError ** error)
{
  OdnCache * self = NULL;
  gchar * content;
  gsize content_size;

  gsize bytes_read;
  gsize bytes_written;

  if(g_file_get_contents(filename,&content,&content_size,error))
    {
      self = ODN_CACHE(g_object_new(ODN_TYPE_CACHE,NULL));

      self->priv->content = content;
      self->priv->content_size = content_size;

      /* MIME-Type guess */
      self->priv->mime_type = g_content_type_guess(
	  filename,
	  (guchar*)content,
	  content_size,
	  NULL);

      /* gzip compression */
      GZlibCompressor *
      compressor = g_zlib_compressor_new(G_ZLIB_COMPRESSOR_FORMAT_GZIP,9);
      gpointer outbuff = g_malloc(content_size * 2);
      g_converter_convert(G_CONVERTER(compressor),
			 content,
			 content_size,
			 outbuff,
			 content_size * 2,
			 G_CONVERTER_INPUT_AT_END||G_CONVERTER_FLUSH,
			 &bytes_read,
			 &bytes_written,
			 error);

      self->priv->content_gzip = g_realloc(outbuff,bytes_written);
      self->priv->content_gzip_size = bytes_written;
      g_object_unref(compressor);

      /* deflate compression */
      compressor = g_zlib_compressor_new(G_ZLIB_COMPRESSOR_FORMAT_RAW,9);
      outbuff = g_malloc(content_size * 2);
      g_converter_convert(G_CONVERTER(compressor),
		   content,
		   content_size,
		   outbuff,
		   content_size * 2,
		   G_CONVERTER_INPUT_AT_END||G_CONVERTER_FLUSH,
		   &bytes_read,
		   &bytes_written,
		   error);

      self->priv->content_deflate = g_realloc(outbuff,bytes_written);
      self->priv->content_deflate_size = bytes_written;
      g_object_unref(compressor);
    }

  return self;
}

const gchar *
odn_cache_get_mime_type(OdnCache * cache)
{
  return cache->priv->mime_type;
}

gconstpointer
odn_cache_get_content(OdnCache * cache,
		      OdnCompressionType compression_type,
		      gsize * size)
{
  switch(compression_type)
  {
    case ODN_COMPRESSION_DEFLATE:
      *size = cache->priv->content_deflate_size;
      return cache->priv->content_deflate;
    case ODN_COMPRESSION_GZIP:
      *size = cache->priv->content_gzip_size;
      return cache->priv->content_gzip;
    default:
      *size = cache->priv->content_size;
      return cache->priv->content;
  }
}
