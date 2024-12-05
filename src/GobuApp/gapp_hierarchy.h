// gapp_hierarchy.h
#ifndef GAPP_HIERARCHY_H
#define GAPP_HIERARCHY_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_HIERARCHY (gapp_hierarchy_get_type())
G_DECLARE_FINAL_TYPE(GappHierarchy, gapp_hierarchy, GAPP, HIERARCHY, GtkBox)

GappHierarchy *gapp_hierarchy_new(void);

G_END_DECLS

#endif // GAPP_HIERARCHY_H

