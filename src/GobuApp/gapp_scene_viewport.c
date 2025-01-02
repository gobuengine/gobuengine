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
    gfx_backend_t *context = gapp_widget_viewport_get_context_render(viewport);
    gfxb_viewport_color(context, 18, 18, 18);

    // ecs_entity_t paint = gobu_ecs_entity_new(GWORLD, gobu_scene_get_open(GWORLD), "ShapeRect");
    // ecs_set(GWORLD, paint, gb_shape_rec_t, {.width = 100, .height = 100, .color = YELLOW, .lineColor = RED, .lineWidth = 2});
    // gb_transform_t *transform = ecs_get(GWORLD, paint, gb_transform_t);
    // transform->position = (gb_vec2_t){100, 100};
}

static void gapp_scene_viewport_render(GtkWidget *viewport, int width, int height)
{
    // gobu_draw_grid(width, height, 32, GRIDMODEDARK, 0);
    // gobu_scene_process(GWORLD, gobu_scene_get_open(GWORLD), 0.0f);
    gobu_ecs_process(GWORLD, 0.0f);
}

GappSceneViewport *gapp_scene_viewport_new(void)
{
    return g_object_new(GAPP_TYPE_SCENE_VIEWPORT, NULL);
}

// MARK: PUBLIC
