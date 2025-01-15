// gapp_widget_hierarchy.h
#ifndef GAPP_WIDGET_HIERARCHY_H
#define GAPP_WIDGET_HIERARCHY_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_WIDGET_TYPE_HIERARCHY (gapp_widget_hierarchy_get_type())
G_DECLARE_FINAL_TYPE(GappWidgetHierarchy, gapp_widget_hierarchy, GAPP_WIDGET, HIERARCHY, GtkBox)

GappWidgetHierarchy *gapp_widget_hierarchy_new(void);

G_END_DECLS

#endif // GAPP_WIDGET_HIERARCHY_H

