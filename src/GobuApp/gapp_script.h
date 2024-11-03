// gapp_script.h
#ifndef GAPP_SCRIPT_H
#define GAPP_SCRIPT_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_SCRIPT (gapp_script_get_type())
G_DECLARE_FINAL_TYPE(GappScript, gapp_script, GAPP, SCRIPT, GtkBox)

GappScript *gapp_script_new(const gchar *filename);

void gapp_script_load_file(GappScript *self, const gchar *filename);

G_END_DECLS

#endif // GAPP_SCRIPT_H

