// gapp_project_setting.h
#ifndef GAPP_PROJECT_SETTING_H
#define GAPP_PROJECT_SETTING_H

#include "config.h"
#include "gobu/gobu.h"

G_BEGIN_DECLS

#define GAPP_PROJECT_TYPE_SETTING (gapp_project_setting_get_type())
G_DECLARE_FINAL_TYPE(GappProjectSetting, gapp_project_setting, GAPP_PROJECT, SETTING, GtkWindow)

GappProjectSetting *gapp_project_setting_new(void);
void gapp_project_setting_show(GappProjectSetting *self);
void gapp_project_settings_set_name(ecs_world_t *world, const char *name);
const char *gapp_project_settings_name(ecs_world_t *world);

G_END_DECLS

#endif // GAPP_PROJECT_SETTING_H
