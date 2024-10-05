#ifndef GOBU_PROJECT_CONFIG_H
#define GOBU_PROJECT_CONFIG_H
#include <stdio.h>
#include <stdbool.h>
#include <glib.h>

typedef struct
{
    gchar *name;
    gchar *version;
    gchar *description;
    gchar *id;
    gchar *author;
    gchar *email;
    gchar *icon;
    gchar *first_scene;
    gint viewport_width;
    gint viewport_height;
    gint sampling;
    GdkRGBA background_color;
    gint window_width;
    gint window_height;
    gint window_mode;
    gboolean window_resizable;
    gboolean window_borderless;
    gboolean window_alway_on_top;
    gboolean window_transparent;
    gboolean window_no_focus;
    gboolean window_high_dpi;
    gboolean window_vsync;
    gboolean window_aspect_ratio;
    gint physics_gravity;
    gint physics_gravity_direction[2];
} gobuProjectConfig;

gboolean gobu_project_config_init(const gchar *project_path, gboolean load_from_file);

gboolean gobu_project_config_load_from_file(void);

void gobu_project_config_free(gboolean full_free);

gboolean gobu_project_config_save(void);

gobuProjectConfig *gobu_project_config_get(void);

gboolean gobu_project_config_set_default(const gchar *name);

#endif // GOBU_PROJECT_CONFIG_H
