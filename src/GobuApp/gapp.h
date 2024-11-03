#ifndef GAPP_H
#define GAPP_H
#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_MAIN (gapp_main_get_type())
G_DECLARE_FINAL_TYPE(GappMain, gapp_main, GAPP, MAIN, GtkApplication)

GObject *gapp_get_editor_instance(void);

GObject *gapp_get_config_instance(void);

void gapp_open_project(GappMain *self, const gchar *path);

void gapp_append_right_panel(GappMain *self, const gchar *title, GtkWidget *module, gboolean is_button_close);

void gapp_append_left_panel(GappMain *self, const gchar *icon, GtkWidget *module);

G_END_DECLS

#endif // GAPP_H
