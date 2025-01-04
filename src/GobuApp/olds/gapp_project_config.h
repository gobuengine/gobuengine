// gapp_project_config.h
#ifndef GAPP_PROJECT_CONFIG_H
#define GAPP_PROJECT_CONFIG_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_PROJECT_CONFIG (gapp_project_config_get_type())
G_DECLARE_FINAL_TYPE(GappProjectConfig, gapp_project_config, GAPP, PROJECT_CONFIG, GObject)

GappProjectConfig *gapp_project_config_new(void);

gboolean gapp_project_config_load(GappProjectConfig *self, const gchar *filename);

gboolean gapp_project_config_save(GappProjectConfig *self);

gboolean gapp_project_config_create_file_default(GappProjectConfig *self, const gchar *filename, const gchar *name);

const gchar *gapp_project_setting_get_name(GappProjectConfig *self);

G_END_DECLS

#endif // GAPP_PROJECT_CONFIG_H
