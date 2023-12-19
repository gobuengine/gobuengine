#include "gapp_level_viewport.h"
#include "gapp_main.h"

struct _GappLevelViewport
{
    GtkWidget parent;
    GappLevelEditor* editor;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelViewport, gapp_level_viewport, GAPP_TYPE_GOBU_EMBED);

extern gb_engine_t engine;

static void signal_viewport_start(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_render(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_drop(GappLevelViewport* viewport, GListStore* filess, double x, double y, gpointer data);

static void gapp_level_viewport_class_init(GappLevelViewportClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gapp_level_viewport_init(GappLevelViewport* self)
{
    g_signal_connect(self, "gobu-embed-start", G_CALLBACK(signal_viewport_start), NULL);
    g_signal_connect(self, "gobu-embed-render", G_CALLBACK(signal_viewport_render), NULL);
    g_signal_connect(self, "gobu-embed-drop", G_CALLBACK(signal_viewport_drop), NULL);
}

/**
 * @brief Crea una nueva instancia de GappLevelViewport.
 *
 * @param editor El editor de nivel asociado al viewport.
 * @return Un puntero a la nueva instancia de GappLevelViewport.
 */
GappLevelViewport* gapp_level_viewport_new(GappLevelEditor* editor)
{
    GappLevelViewport* self = g_object_new(GAPP_LEVEL_TYPE_VIEWPORT, NULL);
    {
        self->editor = editor;
    }
    return self;
}

/**
 * @brief Función que se ejecuta cuando se inicia la vista del viewport.
 *
 * @param viewport El objeto GappLevelViewport.
 * @param data Datos adicionales pasados como argumento.
 */
static void signal_viewport_start(GappLevelViewport* viewport, gpointer data)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);

    g_signal_emit_by_name(viewport->editor, "level-viewport-init", width, height, 0);
}

/**
 * @brief Renderiza el viewport de nivel.
 *
 * Esta función se encarga de renderizar el viewport de nivel.
 *
 * @param viewport El viewport de nivel a renderizar.
 * @param data Datos adicionales pasados a la función de señal.
 */
static void signal_viewport_render(GappLevelViewport* viewport, gpointer data)
{
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);
    gb_app_progress(world);
}

/**
 * @brief Maneja la señal de soltar en el viewport del nivel.
 *
 * Esta función se llama cuando se suelta un archivo en el viewport del nivel.
 *
 * @param viewport El viewport del nivel.
 * @param file El archivo soltado.
 * @param x La posición x donde se soltó el archivo.
 * @param y La posición y donde se soltó el archivo.
 * @param data Datos adicionales pasados al manejador de señales.
 */
static void signal_viewport_drop(GappLevelViewport* viewport, GListStore* filess, double x, double y, gpointer data)
{
    guint items_n = g_list_model_get_n_items(G_LIST_MODEL(filess));
    for (int i = 0; i < items_n; i++)
    {
        GFileInfo* file_info = G_FILE_INFO(g_list_model_get_item(G_LIST_MODEL(filess), i));
        GFile* file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

        gchar* filename = g_file_get_path(file);
        char* name = gb_str_remove_spaces(gb_fs_get_name(filename, true));
        gb_world_t* world = gapp_level_editor_get_world(viewport->editor);

        const char *key = gb_resource_set(world, filename);
        gb_print_info(gb_strdups("Resource load: %s", key));

        gb_camera_t* camera = ecs_get(world, ecs_lookup(world, "Engine"), gb_camera_t);
        gb_vec2_t mouseWorld = engine.screen_to_world(*camera, (gb_vec2_t) { x, y });

        ecs_entity_t e = gb_ecs_entity_new(world, ecs_lookup(world, "World"), name, gb_ecs_transform(mouseWorld.x, mouseWorld.y));

        if (gb_fs_is_extension(filename, ".png") || gb_fs_is_extension(filename, ".jpg")) {
            gb_ecs_entity_set(world, e, gb_sprite_t, { .resource = key });
        }
    }
}

