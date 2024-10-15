// gapp_inspector_widgets.h
#ifndef GAPP_INSPECTOR_WIDGETS_H
#define GAPP_INSPECTOR_WIDGETS_H

#include "config.h"
#include "pixio/pixio.h"


GtkWidget *gapp_inspector_widgets_input_string(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_bool(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_u32(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_f64(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_f32(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_color(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_vector2(ecs_meta_cursor_t cursor);

// enums
GtkWidget *gapp_inspector_widgets_input_enum(ecs_meta_cursor_t cursor);

// resources
GtkWidget *gapp_inspector_widgets_input_texture(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_font(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_sound(ecs_meta_cursor_t cursor);
GtkWidget *gapp_inspector_widgets_input_scene(ecs_meta_cursor_t cursor);

#endif // GAPP_INSPECTOR_WIDGETS_H

