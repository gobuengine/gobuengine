// gapp_inspector_widgets.h
#ifndef GAPP_INSPECTOR_WIDGETS_H
#define GAPP_INSPECTOR_WIDGETS_H

#include "config.h"
#include "gobu/gobu.h"

// callback esta funcion se llama cuando se crea un input_field nuevo.
typedef void (*GappPropsReadyCallback)(GtkWidget *parent, GtkWidget *input, const char *field_name, gpointer data);

void gapp_inspector_create_component_fields(void *ptr, ecs_entity_t component, GtkWidget *parent, GappPropsReadyCallback fieldCallback, gpointer data);

GtkWidget *gapp_inspector_create_component_group(GtkWidget *list, bool buttonRemove, const gchar *title_str, ecs_entity_t entity, ecs_entity_t component);

GtkWidget *gapp_inspector_create_field_row(GtkWidget *size_group, const char *label_str, GtkWidget *input, GtkOrientation orientation);

#endif // GAPP_INSPECTOR_WIDGETS_H

