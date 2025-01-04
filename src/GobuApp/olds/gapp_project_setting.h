// gapp_project_setting.h
#ifndef GOBU_PROJECT_SETTING_H
#define GOBU_PROJECT_SETTING_H

#include "config.h"

G_BEGIN_DECLS

#define GOBU_TYPE_PROJECT_SETTING (gobu_project_setting_get_type())
G_DECLARE_FINAL_TYPE(GappProjectSetting, gobu_project_setting, GOBU, PROJECT_SETTING, GtkWindow)

GappProjectSetting *gobu_project_setting_new(void);

void gobu_project_setting_show(GappProjectSetting *self, GtkWidget *parent);

G_END_DECLS

#endif // GOBU_PROJECT_SETTING_H
