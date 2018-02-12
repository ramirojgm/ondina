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
#include "ctype.h"

typedef struct _OdnParserWord OdnParserWord;
typedef struct _OdnParserPrivate OdnParserPrivate;


struct _OdnParserPrivate
{
  GList
    * words,
    * cur_word;
};

struct _OdnParserWord
{
  gchar * content;
  gsize length;
};

G_DEFINE_TYPE_WITH_PRIVATE(OdnParser,odn_parser,G_TYPE_OBJECT)

static const gchar * odn_parser_spaces[] = {"\n","\r","\t"," ",NULL};
static const gchar * odn_parser_spliters[] = {",",";",":","[","]","(",")","{","}",NULL};

static void
odn_parser_word_free(OdnParserWord * word)
{
  g_free(word->content);
  g_free(word);
}

static void
odn_parser_finalize(GObject * instance)
{
  OdnParserPrivate * priv;
  priv = odn_parser_get_instance_private(ODN_PARSER(instance));
  g_list_free_full(priv->words,(GDestroyNotify)odn_parser_word_free);
}


static void
odn_parser_init(OdnParser * self)
{
  OdnParserPrivate * priv;
  priv = odn_parser_get_instance_private(self);
  priv->cur_word = NULL;
  priv->words = NULL;
}

static void
odn_parser_class_init(OdnParserClass * klass)
{
  G_OBJECT_CLASS(klass)->finalize = odn_parser_finalize;
}

OdnParser *
odn_parser_new()
{
  OdnParser * parser = ODN_PARSER(g_object_new(ODN_TYPE_PARSER,NULL));
  return parser;
}


gint
odn_parser_index_of(const gchar * str,const gchar ** word_set)
{
  gint word_index = 0;
  for(;*word_set;word_set++)
    {
      if(g_str_has_prefix(str,*word_set))
	{
	  return word_index;
	}
      word_index ++;
    }
  return -1;
}

static const gchar*
odn_parser_skip_space(const gchar * str,const gchar * end)
{
  const gchar * iter = str;
  for(;iter < end;iter++)
    {
      if(odn_parser_index_of(iter, odn_parser_spaces) == -1)
	  return iter;
    }
  if(str > end)
    return end;
  else
    return iter;
}

static gint
odn_parser_get_spliter(const gchar * str)
{
  return odn_parser_index_of(str,odn_parser_spliters);
}

static const gchar*
odn_parser_parse_next_word(const gchar * str,gsize * length,const gchar * end)
{
  gint spliter = odn_parser_get_spliter(str);
  gboolean in_escape;
  gchar literal;
  const gchar * start = str;
  if(spliter != -1)
    {
      *length = g_utf8_strlen(odn_parser_spliters[spliter],-1);
      return start;
    }
  else if(str >= end)
    {
      *length = 0;
      return end;
    }
  else if((*str == '\"')||(*str == '\''))
    {
      literal = *str;
      in_escape = FALSE;
      while( str < end)
	{
	  str++;
	  if (*str == literal)
	    {
	      if (!in_escape)
		break;
	      else
		in_escape = FALSE;
	    }
	  else if (*str == '\\')
	    in_escape = !in_escape;
	  else if (in_escape)
	    in_escape = FALSE;
	}
      str++;
      * length = str - start;
      return start;
    }
  else
    {
      while((str < end) && (odn_parser_index_of(str,odn_parser_spaces) == -1) && (odn_parser_index_of(str,odn_parser_spliters) == -1))
	str ++;
      if( start != str)
	{
	  if(length)
	    *length = str - start;
	  return start;
	}
      else
	{
	  *length = 0;
	  return end;
	}
    }
}



gboolean
odn_parser_parse(OdnParser * parser,const gchar * str,gssize length)
{
  OdnParserPrivate * priv;
  const gchar * cur,* end;
  OdnParserWord * word;
  gboolean done = TRUE;
  gsize word_length;

  priv = odn_parser_get_instance_private(parser);

  if(length < 0)
    length = g_utf8_strlen(str,-1);

  cur = str;
  end = (cur + length);

  while(cur != end)
    {
      cur = odn_parser_skip_space(cur,end);
      cur = odn_parser_parse_next_word(cur,&word_length,end);
      if((cur < end) && (length > 0))
	{
	  word = g_new(OdnParserWord,1);
	  word->content = g_strndup(cur,word_length);
	  //g_print("%s\n",word->content);
	  word->length = word_length;
	  priv->words = g_list_append(priv->words,word);
	  cur += word_length;
	}
      else
	break;
    }

  priv->cur_word = priv->words;
  return done;
}



gboolean
odn_parser_is_end(OdnParser * parser)
{
  OdnParserPrivate * priv;
  priv = odn_parser_get_instance_private(parser);
  return priv->cur_word == NULL;
}

gchar *
odn_parser_extract_string(const gchar * str)
{
  if(!str) return NULL;

  if((g_str_has_prefix(str,"'") && g_str_has_suffix(str,"'"))||
      (g_str_has_prefix(str,"\"") && g_str_has_suffix(str,"\"")))
    {
      gsize str_length = g_utf8_strlen(str,G_MAXINT32);
      gchar * plain = g_strndup(str + 1,str_length - 2);
      gchar * utf8_encoded = g_utf8_make_valid(plain,-1);
      gchar * result = g_strcompress(utf8_encoded);
      g_free(plain);
      g_free(utf8_encoded);
      return result;
    }
  else
    {
      if(g_strcmp0(str,"null") == 0)
	return NULL;

      return g_strdup(str);
    }
}

gboolean
odn_parser_is_next_word(OdnParser * parser,const gchar * str)
{
  OdnParserPrivate * priv;
  OdnParserWord * word;

  priv = odn_parser_get_instance_private(parser);

  if(priv->cur_word)
    {
      word = (OdnParserWord *)(priv->cur_word->data);
      return g_strcmp0(word->content,str) == 0;
    }
  else
    {
      return FALSE;
    }
}

gboolean
odn_parser_next_word_has_prefix(OdnParser * parser,const gchar * prefix)
{
  OdnParserPrivate * priv;
  OdnParserWord * word;

  priv = odn_parser_get_instance_private(parser);

  if(priv->cur_word)
   {
     word = (OdnParserWord *)(priv->cur_word->data);
     return g_str_has_prefix(word->content,prefix);
   }
  else
   {
     return FALSE;
   }
}

gboolean
odn_parser_next_word_has_suffix(OdnParser * parser,const gchar * suffix)
{
  OdnParserPrivate * priv;
  OdnParserWord * word;

  priv = odn_parser_get_instance_private(parser);

  if(priv->cur_word)
   {
     word = (OdnParserWord *)(priv->cur_word->data);
     return g_str_has_suffix(word->content,suffix);
   }
  else
   {
     return FALSE;
   }
}

gboolean
odn_parser_skip(OdnParser * parser)
{
  OdnParserPrivate * priv;
  priv = odn_parser_get_instance_private(parser);

  if(priv->cur_word)
    {
      priv->cur_word = g_list_next(priv->cur_word);
      return FALSE;
    }
  else
    {
      return TRUE;
    }
}

const gchar *
odn_parser_next_word(OdnParser * parser,gsize * length,gboolean move)
{
  OdnParserPrivate * priv;
  OdnParserWord * word;

  priv = odn_parser_get_instance_private(parser);

  if(priv->cur_word)
    {
      word = (OdnParserWord *)(priv->cur_word->data);
      if(move)
	priv->cur_word = g_list_next(priv->cur_word);
      if(length)
	*length = word->length;
      return word->content;
    }
  else
    {
      if(length)
	*length = 0;
      return NULL;
    }
}

gboolean
odn_parser_next_word_check(OdnParser * parser,const gchar * str)
{
  OdnParserPrivate * priv;
  OdnParserWord * word;

  priv = odn_parser_get_instance_private(parser);

  if(priv->cur_word)
    {
      word = (OdnParserWord *)(priv->cur_word->data);
      if(g_strcmp0(word->content,str) == 0)
	{
	  priv->cur_word = g_list_next(priv->cur_word);
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}
    }
  else
    {
      return FALSE;
    }
}

gboolean
odn_parser_check_is_number(OdnParser * parser)
{
  gboolean done = FALSE;
  const gchar * word = odn_parser_next_word(parser,NULL,FALSE);
  if(word)
    {
      if((*word == 'l') || (*word == 'f'))
	{
	  done = FALSE;
	}
      else
	{
	  for(const gchar * c = word;*c != 0;c++)
	    {
	      if(!(isdigit(*c) || (*c == 'l') || (*c == 'f')))
		{
		  done = FALSE;
		  break;
		}
	      else
		{
		  done = TRUE;
		}
	    }
	}
    }
  return done;
}


gboolean
odn_parser_check_is_const(OdnParser * parser)
{
  return odn_parser_check_is_number(parser)||
	 odn_parser_check_is_string(parser);
}

gboolean
odn_parser_check_is_string(OdnParser * parser)
{
  return (odn_parser_next_word_has_prefix(parser,"\"")
	  && odn_parser_next_word_has_suffix(parser,"\""))
	  ||(odn_parser_next_word_has_prefix(parser,"\'")
	  && odn_parser_next_word_has_suffix(parser,"\'"));
}

gboolean
odn_parser_check_is_named(OdnParser * parser)
{
  const gchar * word = odn_parser_next_word(parser,NULL,FALSE);
    if(word)
      {
	if(!isalpha(*word))
	  return FALSE;
        for(const gchar * c = word;*c != 0;c++)
  	{
  	  if(!isalnum(*c) && *c != '_')
  	    return FALSE;
  	}
        return odn_parser_index_of(word,odn_parser_spliters) == -1;
      }
    return FALSE;
}
