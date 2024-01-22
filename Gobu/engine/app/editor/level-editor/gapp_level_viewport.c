#include "gapp_level_viewport.h"

struct _GappLevelViewport
{
    GtkWidget parent;
    GappLevelEditor* editor;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelViewport, gapp_level_viewport, GTK_TYPE_RAY);

static void signal_viewport_start(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_drop(GappLevelViewport* viewport, GListStore* filess, double x, double y, gpointer data);

static void gapp_level_viewport_class_init(GappLevelViewportClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gapp_level_viewport_init(GappLevelViewport* self)
{
    g_signal_connect(self, "raystart", G_CALLBACK(signal_viewport_start), NULL);
    g_signal_connect(self, "raydrop", G_CALLBACK(signal_viewport_drop), NULL);
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
    int width = gtk_ray_get_width(viewport);
    int height = gtk_ray_get_height(viewport);

    g_signal_emit_by_name(viewport->editor, "raystart", width, height, 0);
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
        // GFileInfo* file_info = G_FILE_INFO(g_list_model_get_item(G_LIST_MODEL(filess), i));
        // GFile* file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

        // gchar* filename = g_file_get_path(file);
        // char* name = gb_str_remove_spaces(gb_fs_get_name(filename, true));
        // ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);

        // const char* key = gb_resource_set(world, filename);
        // gb_log_info(TF("Resource load: %s", key));

        // gb_camera_t* camera = ecs_get(world, ecs_lookup(world, "Engine"), gb_camera_t);
        // gb_vec2_t mouseWorld = screen_to_world(*camera, (gb_vec2_t) { x, y });

        // ecs_entity_t parent = ecs_lookup(world, GAME_ROOT_ENTITY);
        // ecs_entity_t e = gb_ecs_entity_new(world, parent, name, gb_ecs_transform(mouseWorld.x, mouseWorld.y));

        // if (gb_fs_is_extension(filename, ".png") || gb_fs_is_extension(filename, ".jpg") || gb_fs_is_extension(filename, ".sprite")) {
        //     ecs_set(world, e, gb_sprite_t, { .resource = key });
        // }

        // g_free(filename);
        // g_free(name);
    }
}

