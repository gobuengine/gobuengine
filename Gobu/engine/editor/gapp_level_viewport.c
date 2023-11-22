#include "gapp_level_viewport.h"
#include "gapp_main.h"

struct _GappLevelViewport
{
    GtkWidget parent;
    GappLevelEditor* editor;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelViewport, gapp_level_viewport, GAPP_TYPE_GOBU_EMBED);

static void signal_viewport_start(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_render(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_resize(GappLevelViewport* viewpor, int width, int height, gpointer data);
static void signal_viewport_drop(GappLevelViewport* viewpor, GFile* file, double x, double y, gpointer data);

static void gapp_level_viewport_class_init(GappLevelViewportClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gapp_level_viewport_init(GappLevelViewport* self)
{
    g_signal_connect(self, "gobu-embed-start", G_CALLBACK(signal_viewport_start), NULL);
    g_signal_connect(self, "gobu-embed-render", G_CALLBACK(signal_viewport_render), NULL);
    g_signal_connect(self, "gobu-embed-resize", G_CALLBACK(signal_viewport_resize), NULL);
    g_signal_connect(self, "gobu-embed-drop", G_CALLBACK(signal_viewport_drop), NULL);
}

GappLevelViewport* gapp_level_viewport_new(GappLevelEditor* editor)
{
    GappLevelViewport* self = g_object_new(GAPP_LEVEL_TYPE_VIEWPORT, NULL);
    {
        self->editor = editor;
    }
    return self;
}

static void signal_viewport_start(GappLevelViewport* viewport, gpointer data)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);

    gobu_rendering_init(world, &(GWindow){.title = "LevelEditor", .width = width, .height = height, .viewport = true});
}

static void signal_viewport_render(GappLevelViewport* viewport, gpointer data)
{
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);
    gobu_rendering_progress(world);
}

static void signal_viewport_resize(GappLevelViewport* viewport, int width, int height, gpointer data)
{
    ViewportSizeCallback(width, height);
}

static void signal_viewport_drop(GappLevelViewport* viewport, GFile* file, double x, double y, gpointer data)
{
    gchar* filename = g_file_get_path(file);
    gchar* name = gb_fs_get_name(filename, true);
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);

    if (gobu_resource_set(world, name, filename))
        debug_print(CONSOLE_INFO, gb_strdups("Resource load: %s", name));

    ecs_entity_t entity = ecs_new_id(world);
    ecs_set_name(world, entity, gb_strdups("%s%d", name, entity));
    ecs_add_pair(world, entity, EcsChildOf, ecs_lookup(world, "World"));
    ecs_set(world, entity, GPosition, { x, y });

    if (gb_fs_is_extension(filename, ".png")) {
        ecs_set(world, entity, GSprite, { .resource = name });
        debug_print(CONSOLE_INFO, gb_strdups("Spawn Entity [%lld], Component Sprite [%s]", entity, name));
    }
}
