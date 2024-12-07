// gapp_component.h
#ifndef GAPP_COMPONENTS_H
#define GAPP_COMPONENTS_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_COMPONENT (gapp_component_get_type())
G_DECLARE_FINAL_TYPE(GappComponent, gapp_component, GAPP, COMPONENT, GtkBox)

GappComponent *gapp_component_new(const gchar *filename);

G_END_DECLS

#endif // GAPP_COMPONENTS_H

