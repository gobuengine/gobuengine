#include "gapp_scene_viewport.h"
#include "gapp_widget.h"
#include "gapp_wviewport.h"
#include "gapp.h"
#include "gobu/gobu.h"

struct _GappSceneViewport
{
    GtkBox parent_instance;
};

static void gapp_scene_viewport_ready(GtkWidget *viewport);
static void gapp_scene_viewport_render(GtkWidget *viewport, int width, int height);

// MARK: CLASS
G_DEFINE_TYPE(GappSceneViewport, gapp_scene_viewport, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappSceneViewport *self = GAPP_SCENE_VIEWPORT(object);
    G_OBJECT_CLASS(gapp_scene_viewport_parent_class)->dispose(object);
}

static void gapp_scene_viewport_class_init(GappSceneViewportClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_scene_viewport_init(GappSceneViewport *self)
{
    GtkWidget *viewport = gapp_widget_viewport_new();
    g_signal_connect(viewport, "viewport-ready", G_CALLBACK(gapp_scene_viewport_ready), self);
    g_signal_connect(viewport, "viewport-render", G_CALLBACK(gapp_scene_viewport_render), self);
    gtk_box_append(GTK_BOX(self), viewport);
}

static void gapp_scene_viewport_ready(GtkWidget *viewport)
{
    // ecs_entity_t paint = gobu_ecs_entity_new(GWORLD, gobu_ecs_scene_get_open(GWORLD), "ShapeRect");
    // ecs_set(GWORLD, paint, gb_comp_rectangle_t, {.width = 100, .height = 100, .color = YELLOW, .lineColor = RED, .lineWidth = 2});
    // gb_transform_t *transform = ecs_get(GWORLD, paint, gb_transform_t);
    // transform->position = (gb_vec2_t){100, 100};
}

static void gapp_scene_viewport_render(GtkWidget *viewport, int width, int height)
{
    ecs_entity_t scene_id = gobu_ecs_scene_get_open(GWORLD);
    if (!scene_id)
        return;

    ecs_entity_t projectSettings = gobu_ecs_project_settings();
    if (!projectSettings)
        return;

    gb_core_scene_t *scene = ecs_get_mut(GWORLD, scene_id, gb_core_scene_t);
    gb_core_scene_grid_t *grid = ecs_get_mut(GWORLD, scene_id, gb_core_scene_grid_t);
    gb_core_project_settings3_t *rendering = ecs_get_mut(GWORLD, projectSettings, gb_core_project_settings3_t);

    gfxb_viewport_color(gapp_widget_viewport_context(viewport), scene->color.r, scene->color.g, scene->color.b);

    gb_color_t gridColor = gobu_color_adjust_contrast(scene->color, 0.1f);
    if (grid && grid->enabled)
        gobu_draw_grid(width, height, grid->size, gridColor, 0);

    gobu_draw_rect(0, 0, rendering->resolution.width, rendering->resolution.height, BLANK, gobu_color_adjust_contrast(gridColor, 0.2f), 2.0f, 0);

    gobu_ecs_process(GWORLD, 0.0f);
}

GappSceneViewport *gapp_scene_viewport_new(void)
{
    return g_object_new(GAPP_TYPE_SCENE_VIEWPORT, NULL);
}

// MARK: PUBLIC
