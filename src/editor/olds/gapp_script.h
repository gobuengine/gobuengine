// gapp_script.h
#ifndef GAPP_SCRIPT_H
#define GAPP_SCRIPT_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_SCRIPT (gapp_script_get_type())
G_DECLARE_FINAL_TYPE(GappScript, gapp_script, GAPP, SCRIPT, GtkBox)

GappScript *gapp_script_new(void);

void gapp_script_save(GappScript *self);

void gapp_script_load_file(GappScript *self, const gchar *filename);

const gchar *gapp_script_get_filename(GappScript *self);

gchar *gapp_script_get_text(GappScript *self);

void gapp_script_set_text(GappScript *self, const gchar *text);

GtkWidget *gapp_script_get_buffer(GappScript *self);

G_END_DECLS

#endif // GAPP_SCRIPT_H

