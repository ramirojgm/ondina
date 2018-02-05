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

#ifndef ONDINA_APPLICATION_H_
#define ONDINA_APPLICATION_H_

#define ODN_TYPE_APPLICATION	(odn_application_get_type())
G_DECLARE_FINAL_TYPE(OdnApplication,odn_application,ODN,APPLICATION,GApplication)

typedef struct _OdnApplicationPrivate OdnApplicationPrivate;

struct _OdnApplication
{
  GApplication parent_instance;

  /* private */
  OdnApplicationPrivate * priv;
};

GType	odn_application_get_type(void);

OdnApplication * odn_application_new(const gchar * application_id);

GType	odn_application_get_session_type(OdnApplication * app);

void	odn_application_set_session_type(OdnApplication * app,
					 GType session_type);

void	odn_application_get_model_class(OdnApplication * app,
					const gchar * name);

void	odn_application_bind_inet_port(OdnApplication * app,
				       guint16 inet_port);

void	odn_application_bind_dynamic(OdnApplication * app,
				      const gchar * path);

void	odn_application_bind_content(OdnApplication * app,
				    const gchar * vpath,
				    const gchar * path);

gchar * odn_application_apply_layout(OdnApplication * app,
				     const gchar * layout_name,
				     const gchar * content);

gboolean odn_application_render_view(OdnApplication * app,
				     const gchar * view_name,
				     OdnViewRenderContext * context);

const gchar * odn_application_get_layout_for_view(OdnApplication * app,
						  const gchar * view_name);

void	odn_application_bind_controller(OdnApplication * app,
					const gchar * vpath,
					GType type);

#endif /* ONDINA_APPLICATION_H_ */
