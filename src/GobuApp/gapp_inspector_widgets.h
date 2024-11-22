// gapp_inspector_widgets.h
#ifndef GAPP_INSPECTOR_WIDGETS_H
#define GAPP_INSPECTOR_WIDGETS_H

#include "config.h"
#include "pixio/pixio.h"

void inspectorWidgetAddComponentToInspector(GtkWidget *content, ecs_world_t *world, void *ptr, ecs_entity_t component);

GtkWidget *inspectorWidgetCreateGroup(GtkWidget *list, const gchar *title_str, bool buttonRemove);

GtkWidget *inspectorWidgetCreateGroupChild(GtkWidget *size_group, const char *label_str, GtkWidget *input, GtkOrientation orientation);

#endif // GAPP_INSPECTOR_WIDGETS_H

