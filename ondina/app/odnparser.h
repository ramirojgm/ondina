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

#ifndef ONDINA_PARSER_H_
#define ONDINA_PARSER_H_

#define ODN_TYPE_PARSER (odn_parser_get_type())
G_DECLARE_DERIVABLE_TYPE(OdnParser,odn_parser,ODN,PARSER,GObject)

struct _OdnParserClass
{
  GObjectClass parent_class;
};


GType		odn_parser_get_type(void);

OdnParser * 	odn_parser_new();

gboolean	odn_parser_parse(OdnParser * parser,const gchar * str,gssize length);

gboolean	odn_parser_skip(OdnParser * parser);

gboolean	odn_parser_is_end(OdnParser * parser);

gboolean	odn_parser_is_next_word(OdnParser * parser,const gchar * word);

const gchar *	odn_parser_next_word(OdnParser * parser,gsize * length,gboolean move);

gboolean	odn_parser_next_word_check(OdnParser * parser,const gchar * str);

gboolean	odn_parser_check_is_number(OdnParser * parser);

gboolean	odn_parser_check_is_const(OdnParser * parser);

gboolean	odn_parser_check_is_named(OdnParser * parser);

gboolean	odn_parser_check_is_string(OdnParser * parser);

#endif /* GOLEMPARSER_H_ */
