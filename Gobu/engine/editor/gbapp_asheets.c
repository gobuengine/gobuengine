#include "gbapp_asheets.h"
#include "gapp_gobu_embed.h"

typedef enum ASheetsDirectionMoveFrame {
    ASHEETS_DIRECTION_MOVE_FRAME_LEFT = -1,
    ASHEETS_DIRECTION_MOVE_FRAME_RIGHT = 1
}ASheetsDirectionMoveFrame;

typedef struct asheets_resources {
    gchar* path;
}asheets_resources;

typedef struct asheets_animation_frame {
    gchar* resource;
    double duration;
    int width;
    int height;
    int x;
    int y;
}asheets_animation_frame;

typedef struct asheets_animations {
    char* name;
    int fps;
    bool loop;
    asheets_animation_frame frames[100];
    int frames_count;
}asheets_animations;

typedef struct asheets_file {
    GHashTable* garbage_collector;
    GHashTable* resources;      // asheets_resources
    GPtrArray* animations;     // asheets_animations
    gchar* default_animation;
}asheets_file;

struct _GbAppAsheets
{
    GtkWidget parent;
    gb_world_t* world;
    GtkStringList* list_anim;
    GtkStringList* list_frame;
    GtkWidget* list_view_frame;
    GtkWidget* list_view_anim;
    GtkWidget* frame_duration;
    GtkWidget* vbox_frames_anims;
    GtkWidget* vbox_frames_toolbar;
    GtkSingleSelection* selection_anim;
    GtkSingleSelection* selection_frame;
    //
    gchar* filename;
    asheets_file asheets;
    asheets_animation_frame frame_copy; // experimental
};

extern GAPP* EditorCore;

static void fn_widget_focus_focus(GtkWidget* widget);
static void fn_asheets_serialize_from_file(GbAppAsheets* self);
static void fn_clear_string_list(GtkStringList* list);
static const char* fn_resource_set(GbAppAsheets* self, const gchar* path);
static void fn_animation_load_list_animation(GbAppAsheets* self);
static void fn_animation_add_frame(GbAppAsheets* self, asheets_animations* anim, asheets_animation_frame* frame);
static void fn_animation_remove_frame(GbAppAsheets* self, asheets_animations* anim, guint post);
static gboolean fn_animation_new(GbAppAsheets* self, const gchar* name);
static gboolean fn_animation_remove(GbAppAsheets* self, guint post);
static asheets_animations* fn_animation_by_name(GbAppAsheets* self, const gchar* name);
static int fn_animation_index_by_name(GbAppAsheets* self, const gchar* name);
static const gchar* fn_animation_selected_get_name(GbAppAsheets* self);
static gchar* fn_animation_default(GbAppAsheets* self);
static void fn_animation_load_frames(GbAppAsheets* self, asheets_animations* anim);
static gboolean fn_animation_exists(GbAppAsheets* self, const gchar* name);

G_DEFINE_TYPE_WITH_PRIVATE(GbAppAsheets, gbapp_asheets, GTK_TYPE_BOX);

static void gbapp_asheets_class_init(GbAppAsheetsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gbapp_asheets_init(GbAppAsheets* self)
{
    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);
}

// --------------------

static gboolean fn_source_func_grab_focus(GtkWidget* widget)
{
    gtk_widget_grab_focus(widget);
    return FALSE;
}

static void fn_widget_focus_focus(GtkWidget* widget)
{
    g_timeout_add(10, (GSourceFunc)fn_source_func_grab_focus, widget);
}

static gboolean fn_source_func_selected_first_animation(GbAppAsheets* self)
{
    gtk_single_selection_set_selected(self->selection_anim, 0);
    return FALSE;
}

static void fn_animation_selected_play(GbAppAsheets* self)
{
    const char* key = gb_resource_set(self->world, self->filename);

    printf("key: %s\n", key);
    ecs_entity_t preview = ecs_lookup(self->world, "asheets");
    gb_ecs_entity_set(self->world, preview, gb_animate_sprite_t, { .resource = key });
    gb_ecs_entity_set(self->world, preview, gb_sprite_t, { 0 });
}

/**
 * Guarda los datos de asheets en un archivo.
 *
 * @param self El puntero al objeto GbAppAsheets.
 */
static gboolean fn_save_asheets_data_to_file(GbAppAsheets* self)
{
    bool r = FALSE;

    binn* asheets_s = binn_object();
    {
        binn_object_set_str(asheets_s, "default", self->asheets.default_animation);

        binn* resources = binn_object();
        {
            GList* keys = g_hash_table_get_keys(self->asheets.resources);
            for (int i = 0; i < g_list_length(keys); i++)
            {
                char* key = g_list_nth_data(keys, i);
                asheets_resources* resource = g_hash_table_lookup(self->asheets.resources, key);
                binn* resource_s = binn_object();
                {
                    binn_object_set_str(resource_s, "path", resource->path);
                }
                binn_object_set_object(resources, key, resource_s);
            }
        }
        binn_object_set_object(asheets_s, "resources", resources);

        binn* animations = binn_list();
        {
            for (int i = 0; i < self->asheets.animations->len; i++)
            {
                asheets_animations* anim = g_ptr_array_index(self->asheets.animations, i);

                binn* anim_s = binn_object();
                {
                    binn_object_set_int32(anim_s, "fps", anim->fps);
                    binn_object_set_bool(anim_s, "loop", anim->loop);
                    binn_object_set_str(anim_s, "name", anim->name);

                    binn* frames = binn_list();
                    for (int i = 0; i < anim->frames_count; i++)
                    {
                        binn* frame_s = binn_object();
                        {
                            binn_object_set_str(frame_s, "resource", anim->frames[i].resource);
                            binn_object_set_double(frame_s, "duration", anim->frames[i].duration);
                            binn_object_set_int32(frame_s, "x", anim->frames[i].x);
                            binn_object_set_int32(frame_s, "y", anim->frames[i].y);
                            binn_object_set_int32(frame_s, "width", anim->frames[i].width);
                            binn_object_set_int32(frame_s, "height", anim->frames[i].height);
                        }
                        binn_list_add_object(frames, frame_s);
                    }
                    binn_object_set_list(anim_s, "frames", frames);
                }
                binn_list_add_object(animations, anim_s);
            }
        }
        binn_object_set_object(asheets_s, "animations", animations);
        r = binn_deserialize_from_file(asheets_s, self->filename);
        binn_free(asheets_s);
    }
    return r;
}

/**
 * @brief Elimina un elemento del recolector de basura de recursos.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param key Clave del elemento a eliminar.
 */
static void fn_resource_garbage_collector_remove(GbAppAsheets* self, const gchar* key)
{
    guint* result = g_hash_table_lookup(self->asheets.garbage_collector, key);
    if (result == NULL)
        return;

    guint n = GPOINTER_TO_UINT(result) - 1;
    n = n < 0 ? 0 : n;
    g_hash_table_insert(self->asheets.garbage_collector, gb_strdup(key), GUINT_TO_POINTER(n));

    if (n == 0)
    {
        asheets_resources* resource = g_hash_table_lookup(self->asheets.resources, key);
        g_return_if_fail(resource != NULL);
        g_hash_table_remove(self->asheets.resources, key);
    }
}

/**
 * @brief Agrega una clave al recolector de basura de recursos.
 *
 * Esta función agrega una clave al recolector de basura de recursos de GbAppAsheets.
 * El recolector de basura se encarga de liberar los recursos asociados a las claves
 * cuando ya no son necesarios.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param key Clave del recurso a agregar.
 */
static void fn_resource_garbage_collector_add(GbAppAsheets* self, const gchar* key)
{
    guint n = GPOINTER_TO_UINT(g_hash_table_lookup(self->asheets.garbage_collector, key));
    n += 1;
    g_hash_table_insert(self->asheets.garbage_collector, gb_strdup(key), GUINT_TO_POINTER(n));
}

/**
 * @brief Limpia una lista de cadenas.
 *
 * @param list La lista de cadenas a limpiar.
 */
static void fn_clear_string_list(GtkStringList* list)
{
    g_return_if_fail(list != NULL);
    guint n = g_list_model_get_n_items(G_LIST_MODEL(list));
    gtk_string_list_splice(list, 0, n, NULL);
}

/**
 * Establece un recurso en el objeto asheets.resources.
 *
 * @param self El objeto GbAppAsheets.
 * @param key La clave del recurso.
 * @param path La ruta del recurso.
 * @param x La posición X del recurso.
 * @param y La posición Y del recurso.
 * @param width El ancho del recurso.
 * @param height La altura del recurso.
 */
static const char* fn_resource_set(GbAppAsheets* self, const gchar* path)
{
    const char* path_relative = gb_path_relative_content(path);
    const char* kkey = gb_path_normalize(gb_strdups("resource://%s", gb_str_replace(path_relative, ".", "!")));
    g_hash_table_insert(self->asheets.resources, gb_strdup(kkey), gb_strdup(path_relative));
    return kkey;
}

/**
 * @brief Crea una nueva animación.
 *
 * Esta función crea una nueva animación en el objeto GbAppAsheets.
 *
 * @param self El puntero al objeto GbAppAsheets.
 * @param name El nombre de la animación.
 * @return TRUE si la animación se creó correctamente, FALSE en caso contrario.
 */
static gboolean fn_animation_new(GbAppAsheets* self, const gchar* name)
{
    asheets_animations* anim = g_new0(asheets_animations, 1);
    anim->fps = 12;
    anim->loop = false;
    anim->name = gb_strdup(name);

    g_ptr_array_add(self->asheets.animations, anim);
    gtk_string_list_append(self->list_anim, gb_strdup(name));

    return TRUE;
}

/**
 * @brief Remueve una animación de la hoja de animaciones.
 *
 * @param self El puntero a la estructura GbAppAsheets.
 * @param post El índice de la animación a remover.
 * @return gboolean Verdadero si la animación fue removida exitosamente, falso en caso contrario.
 */
static gboolean fn_animation_remove(GbAppAsheets* self, guint post)
{
    gchar* name = gtk_string_list_get_string(self->list_anim, post);
    g_return_if_fail(name != NULL);

    asheets_animations* anim = fn_animation_by_name(self, name);
    g_return_if_fail(anim != NULL);

    // Borramos de la lista
    g_ptr_array_remove_index(self->asheets.animations, fn_animation_index_by_name(self, anim->name));
    gtk_string_list_remove(self->list_anim, post);

    // borramos los frames de la animacion para que se puedan
    // borrar los recursos.
    for (int i = anim->frames_count - 1; i >= 0; i--)
        fn_animation_remove_frame(self, anim, i);

    if (g_strcmp0(self->asheets.default_animation, anim->name) == 0)
        self->asheets.default_animation = "\0";

    // deshabitamos el vbox de frames cuando no hay animaciones
    if (g_list_model_get_n_items(G_LIST_MODEL(self->list_anim)) == 0)
        gtk_widget_set_sensitive(self->vbox_frames_anims, FALSE);

    g_free(anim);

    return TRUE;
}

/**
 * @brief Carga la lista de animaciones.
 *
 * Esta función se encarga de cargar la lista de animaciones en la estructura GbAppAsheets.
 *
 * @param self Puntero a la estructura GbAppAsheets.
 */
static void fn_animation_load_list_animation(GbAppAsheets* self)
{
    fn_clear_string_list(self->list_anim);

    for (int i = 0; i < self->asheets.animations->len; i++)
    {
        asheets_animations* anim = g_ptr_array_index(self->asheets.animations, i);
        gtk_string_list_append(self->list_anim, gb_strdup(anim->name));
    }
}

/**
 * @brief Obtiene la animación seleccionada.
 *
 * Esta función devuelve un puntero a la estructura de datos que representa la animación seleccionada en la aplicación GbAppAsheets.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param post Puntero a un entero sin signo donde se almacenará el valor de retorno.
 * @return Puntero a la estructura de datos que representa la animación seleccionada.
 */
static asheets_animations* fn_animation_selected(GbAppAsheets* self, guint* post)
{
    *post = gtk_single_selection_get_selected(self->selection_frame);
    g_return_if_fail(post != -1);

    const gchar* anim_name = fn_animation_selected_get_name(self);
    g_return_if_fail(anim_name != NULL);

    return fn_animation_by_name(self, anim_name);
}

/**
 * Busca una animación por su nombre en la estructura asheets_animations.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param name Nombre de la animación a buscar.
 * @return Puntero a la animación encontrada o NULL si no se encuentra.
 */
static asheets_animations* fn_animation_by_name(GbAppAsheets* self, const gchar* name)
{
    for (int i = 0; i < self->asheets.animations->len; i++)
    {
        asheets_animations* anim = g_ptr_array_index(self->asheets.animations, i);
        if (g_strcmp0(anim->name, name) == 0)
            return anim;
    }
    return NULL;
}

/**
 * @brief Busca el índice de una animación por su nombre.
 *
 * Esta función busca el índice de una animación en el objeto GbAppAsheets
 * basándose en su nombre.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param name Nombre de la animación a buscar.
 * @return El índice de la animación si se encuentra, -1 si no se encuentra.
 */
static int fn_animation_index_by_name(GbAppAsheets* self, const gchar* name)
{
    for (int i = 0; i < self->asheets.animations->len; i++)
    {
        asheets_animations* anim = g_ptr_array_index(self->asheets.animations, i);
        if (g_strcmp0(anim->name, name) == 0)
            return i;
    }
    return -1;
}

/**
 * @brief Verifica si existe una animación en el objeto asheets.animations.
 *
 * @param self El objeto GbAppAsheets.
 * @param name El nombre de la animación a verificar.
 * @return gboolean Devuelve TRUE si la animación existe, FALSE en caso contrario.
 */
static gboolean fn_animation_exists(GbAppAsheets* self, const gchar* name)
{
    return fn_animation_index_by_name(self, name) != -1 ? TRUE : FALSE;
}

/**
 * @brief Función para obtener el nombre de la animacion predeterminada.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @return Puntero a una cadena de caracteres que representa el nombre de archivo predeterminado de la animación.
 */
static gchar* fn_animation_default(GbAppAsheets* self)
{
    return self->asheets.default_animation;
}

/**
 * @brief Obtiene la posición seleccionada de la animación.
 *
 * Esta función devuelve la posición seleccionada de la animación en el objeto GbAppAsheets.
 *
 * @param self El puntero al objeto GbAppAsheets.
 * @return La posición seleccionada de la animación.
 */
static guint fn_animation_position_selected(GbAppAsheets* self)
{
    return gtk_single_selection_get_selected(self->selection_anim);
}

/**
 * @brief Obtiene el nombre de la animación seleccionada.
 *
 * Esta función devuelve el nombre de la animación actualmente seleccionada en la hoja de animación.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @return El nombre de la animación seleccionada.
 */
static const gchar* fn_animation_selected_get_name(GbAppAsheets* self)
{
    guint position = fn_animation_position_selected(self);
    g_return_if_fail(position != -1);
    return gtk_string_list_get_string(self->list_anim, position);
}

/**
 * @brief Agrega un fotograma a una animación.
 *
 * Esta función agrega un fotograma a una animación existente en la estructura GbAppAsheets.
 *
 * @param self Puntero a la estructura GbAppAsheets.
 * @param anim Puntero a la estructura asheets_animations que representa la animación.
 * @param id Identificador del fotograma a agregar.
 * @param duration Duración del fotograma en segundos.
 */
static void fn_animation_add_frame(GbAppAsheets* self, asheets_animations* anim, asheets_animation_frame* frame)
{
    anim->frames[anim->frames_count].resource = gb_strdup(frame->resource);
    anim->frames[anim->frames_count].x = frame->x;
    anim->frames[anim->frames_count].y = frame->y;
    anim->frames[anim->frames_count].width = frame->width;
    anim->frames[anim->frames_count].height = frame->height;

    gtk_string_list_append(self->list_frame, gb_strdups("%d", anim->frames_count));
    anim->frames_count++;

    gtk_widget_set_sensitive(self->vbox_frames_toolbar, TRUE);

    // garbage_collector
    fn_resource_garbage_collector_add(self, frame->resource);
}

/**
 * @brief Remueve un fotograma de una animación en la hoja de sprites.
 *
 * Esta función elimina un fotograma específico de una animación en la hoja de sprites.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param anim Puntero a la estructura de datos de la animación.
 * @param post Índice del fotograma a eliminar.
 */
static void fn_animation_remove_frame(GbAppAsheets* self, asheets_animations* anim, guint post)
{
    gchar* id = gb_strdup(anim->frames[post].resource);
    g_free(anim->frames[post].resource);
    anim->frames_count--;

    // movemos los frames a la izquierda
    for (int i = post; i < anim->frames_count; i++)
    {
        anim->frames[i].duration = anim->frames[i + 1].duration;
        anim->frames[i].resource = gb_strdup(anim->frames[i + 1].resource);
    }

    gtk_string_list_remove(self->list_frame, post);

    // deshabitamos el vbox de frames cuando no hay animaciones
    if (g_list_model_get_n_items(G_LIST_MODEL(self->list_frame)) == 0)
        gtk_widget_set_sensitive(self->vbox_frames_toolbar, FALSE);

    // garbage_collector
    fn_resource_garbage_collector_remove(self, id);

    g_free(id);
}

/**
 * @brief Carga los fotogramas de una animación.
 *
 * Esta función carga los fotogramas de una animación en la estructura de datos proporcionada.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param anim Puntero a la estructura de datos de animaciones.
 */
static void fn_animation_load_frames(GbAppAsheets* self, asheets_animations* anim)
{
    g_return_if_fail(anim != NULL);

    fn_clear_string_list(self->list_frame);

    for (int i = 0; i < anim->frames_count; i++)
    {
        gtk_string_list_append(self->list_frame, gb_strdup(anim->frames[i].resource));
        fn_resource_garbage_collector_add(self, anim->frames[i].resource);
    }

    // deshabitamos el vbox de frames cuando no hay animaciones
    guint n = g_list_model_get_n_items(G_LIST_MODEL(self->list_frame));
    gtk_widget_set_sensitive(self->vbox_frames_toolbar, (n > 0));
}

/**
 * @brief Mueve el frame seleccionado en la dirección especificada.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param direction Dirección en la que se moverá el marco seleccionado.
 */
static void fn_frame_selected_move_to_direction(GbAppAsheets* self, ASheetsDirectionMoveFrame direction)
{
    guint position = -1;
    asheets_animations* anim = fn_animation_selected(self, &position);
    g_return_if_fail(anim != NULL);

    int new_pos = position + direction;
    if (new_pos > (anim->frames_count - 1) || new_pos < 0)
        return;

    // movemos el frame a la nueva direccion
    asheets_animation_frame frame = anim->frames[position];
    anim->frames[position] = anim->frames[new_pos];
    anim->frames[new_pos] = frame;

    // Es mas facil recargar los datos y seleccionar el frame...
    fn_animation_load_frames(self, anim);
    gtk_single_selection_set_selected(self->selection_frame, new_pos);
}

/**
 * @brief Serializa los datos de GbAppAsheets desde un archivo.
 *
 * @param self Puntero al objeto GbAppAsheets.
 */
static void fn_asheets_serialize_from_file(GbAppAsheets* self)
{
    self->list_anim = gtk_string_list_new(NULL);
    self->list_frame = gtk_string_list_new(NULL);

    self->asheets.animations = g_ptr_array_new();
    self->asheets.resources = g_hash_table_new(g_str_hash, g_str_equal);
    self->asheets.garbage_collector = g_hash_table_new(g_str_hash, g_str_equal);

    binn* asheets_s = binn_serialize_from_file(self->filename);
    {
        self->asheets.default_animation = gb_strdup(binn_object_str(asheets_s, "default"));
        binn* resources = binn_object_object(asheets_s, "resources");
        binn* animations = binn_object_object(asheets_s, "animations");

        binn_iter iter;
        binn value;
        char key[256];

        // resources
        binn_object_foreach(resources, key, value)
        {
            char* path = binn_object_str(&value, "path");
            fn_resource_set(self, path);
        }

        // animations
        int anims_count = binn_count(animations);
        for (int a = 1; a <= anims_count; a++)
        {
            binn* animation = binn_list_object(animations, a);
            {
                asheets_animations* anim = g_new0(asheets_animations, 1);
                anim->fps = binn_object_int32(animation, "fps");
                anim->loop = binn_object_bool(animation, "loop");
                anim->name = gb_strdup(binn_object_str(animation, "name"));

                binn* frames = binn_object_list(animation, "frames");
                anim->frames_count = binn_count(frames);
                for (int i = 1; i <= anim->frames_count; i++)
                {
                    binn* frame_props = binn_list_object(frames, i);
                    {
                        anim->frames[i - 1].resource = gb_strdup(binn_object_str(frame_props, "resource"));
                        anim->frames[i - 1].duration = binn_object_double(frame_props, "duration");
                        anim->frames[i - 1].x = binn_object_int32(frame_props, "x");
                        anim->frames[i - 1].y = binn_object_int32(frame_props, "y");
                        anim->frames[i - 1].width = binn_object_int32(frame_props, "width");
                        anim->frames[i - 1].height = binn_object_int32(frame_props, "height");
                    }
                }
                g_ptr_array_add(self->asheets.animations, anim);
                gtk_string_list_append(self->list_anim, gb_strdup(anim->name));
            }
        }
    }
    binn_free(asheets_s);

    fn_animation_load_frames(self, fn_animation_by_name(self, fn_animation_default(self)));
}

/**
 * @brief Función que se ejecuta cuando se inicia la visualización del viewport.
 *
 * @param viewport El widget del viewport.
 * @param asheets La instancia de GbAppAsheets.
 */
static void signal_viewport_start(GtkWidget* viewport, GbAppAsheets* asheets)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);

    asheets->world = gb_app_init(&(gb_app_t) { .width = width, .height = height, .show_grid = true });

    gb_camera_t* camera = ecs_get(asheets->world, ecs_lookup(asheets->world, "Engine"), gb_camera_t);
    camera->mode = GB_CAMERA_EDITOR;

    ecs_entity_t preview = gb_ecs_entity_new(asheets->world, "asheets", gb_ecs_transform(0, 0));
}

/**
 * @brief Función que se encarga de renderizar el viewport de la aplicación.
 *
 * @param viewport El widget del viewport a renderizar.
 * @param asheets Puntero a la estructura GbAppAsheets.
 */
static void signal_viewport_render(GtkWidget* viewport, GbAppAsheets* asheets)
{
    gb_app_progress(asheets->world);
}

/**
 * @brief Función que se ejecuta cuando se hace clic en el botón de nueva animación en la barra de herramientas.
 *
 * @param widget El widget que emitió la señal.
 * @param self   El puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_new_animation(GtkWidget* widget, GbAppAsheets* self)
{
    gchar* name;
    for (int i = 1;; i++)
    {
        name = gb_strdups("new_animation_%d", i, NULL);
        if (fn_animation_exists(self, name) == FALSE)
            break;
        g_free(name);
    }

    if (fn_animation_new(self, name))
        gtk_single_selection_set_selected(self->selection_anim, g_list_model_get_n_items(G_LIST_MODEL(self->list_anim)) - 1);
}

/**
 * @brief Función que se ejecuta cuando se hace clic en el botón de eliminar animación en la barra de herramientas.
 *
 * @param widget El widget del botón que se ha hecho clic.
 * @param self   Puntero a la estructura GbAppAsheets que contiene los datos de la aplicación.
 */
static void signal_toolbar_btn_remove_animation(GtkWidget* widget, GbAppAsheets* self)
{
    guint post = gtk_single_selection_get_selected(self->selection_anim);
    g_return_if_fail(post != -1);
    fn_animation_remove(self, post);
}

/**
 * @brief Función de señal para el botón de animación predeterminada en la barra de herramientas.
 *
 * Esta función se llama cuando se hace clic en el botón de animación predeterminada en la barra de herramientas.
 * Recibe un puntero al widget del botón y un puntero a la estructura GbAppAsheets.
 *
 * @param widget El widget del botón que emitió la señal.
 * @param self   Un puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_default_animation(GtkWidget* widget, GbAppAsheets* self)
{
    const gchar* name = fn_animation_selected_get_name(self);
    self->asheets.default_animation = gb_strdup(name);

    int n = gtk_single_selection_get_selected(self->selection_anim);

    fn_animation_load_list_animation(self);
    gtk_single_selection_set_selected(self->selection_anim, n);
}

/**
 * @brief Función que se ejecuta cuando se selecciona una animación.
 *
 * @param selection El GtkSingleSelection que emitió la señal.
 * @param pspec El GParamSpec asociado a la señal.
 * @param self El objeto GbAppAsheets que contiene la función.
 */
static void signal_selected_animation(GtkSingleSelection* selection, GParamSpec* pspec, GbAppAsheets* self)
{
    const gchar* name = fn_animation_selected_get_name(self);
    g_return_if_fail(name != NULL);

    asheets_animations* anim = fn_animation_by_name(self, name);
    g_return_if_fail(anim != NULL);

    fn_animation_load_frames(self, anim);

    gtk_widget_set_sensitive(self->vbox_frames_anims, TRUE);
}

/**
 * @brief Función que se ejecuta cuando se cambia el valor del GtkSpinButton de la frecuencia de animación.
 *
 * @param spin_button El GtkSpinButton que ha cambiado su valor.
 * @param self El puntero a la estructura GbAppAsheets.
 */
static void signal_value_changed_input_fps_animation(GtkSpinButton* spin_button, GbAppAsheets* self)
{
    asheets_animations* anim = (asheets_animations*)g_object_get_data(G_OBJECT(spin_button), "asheets_animations");
    g_return_if_fail(anim != NULL);

    anim->fps = (float)gtk_spin_button_get_value(spin_button);
}

/**
 * @brief Función que se ejecuta cuando se cambia el estado del botón de loop de una animación.
 *
 * @param button El botón de verificación que ha cambiado de estado.
 * @param self   El objeto GbAppAsheets al que pertenece el botón.
 */
static void signal_changed_checked_loop_animation(GtkCheckButton* button, GbAppAsheets* self)
{
    asheets_animations* anim = (asheets_animations*)g_object_get_data(G_OBJECT(button), "asheets_animations");
    g_return_if_fail(anim != NULL);

    anim->loop = gtk_check_button_get_active(button);
}

/**
 * @brief Función que maneja la señal de doble clic en una etiqueta para cambiar el nombre de entrada.
 *
 * Esta función es llamada cuando se produce un doble clic en una etiqueta y se encarga de cambiar el nombre de entrada correspondiente.
 *
 * @param gesture El gesto de clic que se activó.
 * @param n_press El número de veces que se ha presionado el botón del ratón.
 * @param x La coordenada X del punto de clic.
 * @param y La coordenada Y del punto de clic.
 * @param self El puntero a la estructura GbAppAsheets que contiene los datos de la aplicación.
 */
static void signal_double_click_label_to_change_input_name(GtkGestureClick* gesture, gint n_press, gdouble x, gdouble y, GbAppAsheets* self)
{
    GtkWidget* stack = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    if (n_press == 2)
    {
        const gchar* name = fn_animation_selected_get_name(self);

        gtk_stack_set_visible_child_name(GTK_STACK(stack), "input");
        {
            GtkEntry* entry = GTK_ENTRY(gtk_stack_get_child_by_name(GTK_STACK(stack), "input"));
            gapp_widget_entry_set_text(GTK_ENTRY(entry), name);

            fn_widget_focus_focus(entry);
        }
    }
}

/**
 * @brief Función que se ejecuta cuando se activa el cambio de nombre de la animación.
 *
 * @param entry El GtkEntry que activó el cambio de nombre.
 * @param self  El puntero a la estructura GbAppAsheets.
 */
static void signal_change_animation_name_on_enter_key_press(GtkEntry* entry, GbAppAsheets* self)
{
    const gchar* name_new_animation = gb_str_tolower(gb_str_remove_spaces(gapp_widget_entry_get_text(entry)));

    GtkStack* stack = GTK_STACK(gtk_widget_get_parent(entry));
    asheets_animations* anim = (asheets_animations*)g_object_get_data(G_OBJECT(stack), "asheets_animations");

    if (g_strcmp0(anim->name, name_new_animation) == 0)
        goto end;

    if (fn_animation_exists(self, name_new_animation) == TRUE)
        return;

    g_free(anim->name);
    anim->name = gb_strdup(name_new_animation);

    fn_animation_load_list_animation(self);
    gtk_single_selection_set_selected(self->selection_anim, fn_animation_index_by_name(self, anim->name));

end:
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "name");
}

/**
 * @brief Función que se ejecuta cuando se pierde el foco en el GtkEntry al cambiar el nombre de una animacion.
 *
 * @param event El controlador de eventos GtkEventControllerFocus.
 * @param self El objeto GbAppAsheets.
 */
static void signal_input_lost_focus(GtkEventControllerFocus* event, GbAppAsheets* self)
{
    GtkWidget* entry = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(event));
    GtkStack* stack = GTK_STACK(gtk_widget_get_parent(entry));
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "name");
}

/**
 * @brief Configura la fábrica de elementos de lista para la animación.
 *
 * Esta función configura la fábrica de elementos de lista para la animación.
 *
 * @param factory La fábrica de elementos de lista a configurar.
 * @param item El elemento de lista a configurar.
 * @param self El objeto GbAppAsheets actual.
 */
static void signal_list_item_factory_setup_animation(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkWidget* box, * icon, * label, * entry, * box_label, * box_tool, * fps, * loop;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_list_item_set_child(item, box);
    {
        icon = gtk_image_new_from_icon_name("input-dialpad-symbolic");
        gtk_box_append(box, icon);

        box_label = gtk_stack_new();
        gtk_box_append(box, box_label);
        gtk_widget_set_hexpand(box_label, TRUE);

        GtkGesture* active = gtk_gesture_click_new();
        gtk_widget_add_controller(box_label, GTK_EVENT_CONTROLLER(active));
        g_signal_connect(active, "pressed", G_CALLBACK(signal_double_click_label_to_change_input_name), self);
        {
            label = gtk_label_new(NULL);
            gtk_stack_add_named(GTK_STACK(box_label), label, "name");

            entry = gtk_entry_new();
            gtk_widget_set_margin_top(entry, 8);
            gtk_widget_set_margin_bottom(entry, 8);
            gtk_stack_add_named(GTK_STACK(box_label), entry, "input");
            g_signal_connect(entry, "activate", G_CALLBACK(signal_change_animation_name_on_enter_key_press), self);

            GtkEventController* evt_focus = gtk_event_controller_focus_new();
            gtk_widget_add_controller(entry, GTK_EVENT_CONTROLLER(evt_focus));
            g_signal_connect(evt_focus, "leave", G_CALLBACK(signal_input_lost_focus), self);
        }

        gtk_box_append(box, gapp_widget_separator_h());

        box_tool = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_widget_set_halign(box_tool, GTK_ALIGN_END);
        gapp_widget_set_margin(box_tool, 5);
        gtk_box_append(box, box_tool);
        {
            fps = gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(fps), 12.0);
            gtk_widget_set_tooltip_text(fps, "Frames per second");
            gtk_widget_set_margin_top(fps, 3);
            gtk_widget_set_margin_bottom(fps, 3);
            gtk_box_append(box_tool, fps);
            g_signal_connect(fps, "value-changed", G_CALLBACK(signal_value_changed_input_fps_animation), self);

            loop = gtk_check_button_new_with_label("Loop");
            gtk_widget_set_tooltip_text(loop, "Loop animation");
            gtk_box_append(box_tool, loop);
            g_signal_connect(loop, "toggled", G_CALLBACK(signal_changed_checked_loop_animation), self);
        }
    }
}

/**
 * @brief Asocia una animación a un elemento de la lista en una fábrica de elementos de lista.
 *
 * Esta función se utiliza para vincular una animación a un elemento de la lista en una fábrica de elementos de lista.
 *
 * @param factory La fábrica de elementos de lista.
 * @param item El elemento de lista al que se va a vincular la animación.
 * @param self El objeto GbAppAsheets actual.
 */
static void signal_list_item_factory_bind_animation(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkStringObject* string_object = gtk_list_item_get_item(item);

    GtkBox* box = GTK_BOX(gtk_list_item_get_child(item));
    GtkImage* icon = gtk_widget_get_first_child(GTK_WIDGET(box));
    GtkWidget* stack = gtk_widget_get_next_sibling(icon);
    GtkWidget* separator = gtk_widget_get_next_sibling(stack);
    GtkBox* box_tool = gtk_widget_get_next_sibling(separator);
    GtkSpinButton* fps = GTK_SPIN_BUTTON(gtk_widget_get_first_child(box_tool));
    GtkCheckButton* loop = GTK_CHECK_BUTTON(gtk_widget_get_next_sibling(fps));

    gchar* anim_name = gtk_string_object_get_string(string_object);
    asheets_animations* anim = fn_animation_by_name(self, anim_name);

    g_object_set_data(G_OBJECT(fps), "asheets_animations", anim);
    g_object_set_data(G_OBJECT(loop), "asheets_animations", anim);
    g_object_set_data(G_OBJECT(stack), "asheets_animations", anim);

    bool is_default = g_strcmp0(self->asheets.default_animation, anim_name) == 0;
    gtk_image_set_from_icon_name(icon, (is_default ? "user-bookmarks-symbolic" : "input-dialpad-symbolic"));

    GtkWidget* label = gtk_stack_get_child_by_name(GTK_STACK(stack), "name");
    gtk_label_set_xalign(label, 0.0);
    gtk_label_set_text(label, anim->name);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(fps), anim->fps);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(loop), anim->loop);
}

/**
 * @brief Configura el frame de la fábrica de elementos de lista de señales.
 *
 * Esta función configura el marco de la fábrica de elementos de lista de señales
 * con los parámetros especificados.
 *
 * @param factory La fábrica de elementos de lista de señales.
 * @param item El elemento de lista de señales.
 * @param self El objeto GbAppAsheets.
 */
static void signal_list_item_factory_setup_frame(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkWidget* box, * icon, * label;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_list_item_set_child(item, box);
    {
        icon = gtk_image_new_from_file(NULL);
        gtk_widget_set_size_request(icon, 64, 64);
        gtk_box_append(box, icon);

        label = gtk_label_new(NULL);
        gtk_box_append(box, label);
    }
}

/**
 * @brief Asocia un frame a un elemento de la lista en una fábrica de elementos de lista.
 *
 * Esta función se utiliza para vincular un frame a un elemento de la lista en una fábrica de elementos de lista.
 *
 * @param factory La fábrica de elementos de lista.
 * @param item El elemento de la lista al que se va a vincular el marco.
 * @param self El objeto GbAppAsheets actual.
 */
static void signal_list_item_factory_bind_frame(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkStringObject* string_object = gtk_list_item_get_item(item);

    GtkLabel* box = GTK_BOX(gtk_list_item_get_child(item));
    GtkWidget* icon = gtk_widget_get_first_child(box);
    GtkWidget* label = gtk_widget_get_last_child(box);

    gchar* id_r = gtk_string_object_get_string(string_object);

    // resource 
    asheets_resources* resource = g_hash_table_lookup(self->asheets.resources, id_r);
    // asheets_animations * anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    g_return_if_fail(resource != NULL);

    char* path = gb_path_join(gb_project_get_path(), FOLDER_CONTENT_PROJECT, resource->path, NULL);
    path = gb_path_normalize(path);

    gtk_image_set_from_file(GTK_IMAGE(icon), path);
    gtk_label_set_text(label, id_r);
}

/**
 * @brief Función que maneja el evento de soltar archivos en la lista de frames.
 *
 * @param target El objetivo de arrastre.
 * @param value El valor del elemento arrastrado.
 * @param x La posición horizontal del puntero en el momento de soltar el elemento.
 * @param y La posición vertical del puntero en el momento de soltar el elemento.
 * @param self El puntero a la estructura GbAppAsheets.
 * @return gboolean Devuelve TRUE si el evento se maneja correctamente, FALSE en caso contrario.
 */
static gboolean signal_drop_assets_frames(GtkDropTarget* target, const GValue* value, double x, double y, GbAppAsheets* self)
{
    if (G_VALUE_HOLDS(value, G_TYPE_LIST_STORE))
    {
        GListStore* filess = G_LIST_STORE(g_value_get_object(value));

        const gchar* animation_name = fn_animation_selected_get_name(self);
        g_return_if_fail(animation_name != NULL);

        asheets_animations* anim = fn_animation_by_name(self, animation_name);
        g_return_if_fail(anim != NULL);

        guint items_n = g_list_model_get_n_items(G_LIST_MODEL(filess));
        for (int i = 0; i < items_n; i++)
        {
            GFileInfo* file_info = G_FILE_INFO(g_list_model_get_item(G_LIST_MODEL(filess), i));
            GFile* file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

            gchar* filename = g_file_get_path(file);
            if (gb_fs_is_extension(filename, ".png") || gb_fs_is_extension(filename, ".jpg")) {
                gchar* name = gb_str_remove_spaces(gb_fs_get_name(filename, true));

                // crea el resource si no existe
                const char* key = fn_resource_set(self, filename);

                // // agrega el frame a la animacion
                // GdkTexture* texture = gdk_texture_new_from_filename(filename, NULL);
                // int w = gdk_texture_get_width(texture);
                // int h = gdk_texture_get_height(texture);
                // g_object_unref(texture);

                fn_animation_add_frame(self, anim, &(asheets_animation_frame) {
                    .resource = key,
                        .duration = 1.0
                });
            }
        }

        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Función que se ejecuta cuando se selecciona un frame.
 *
 * @param selection La selección de frames.
 * @param pspec La especificación del parámetro.
 * @param self El objeto GbAppAsheets.
 */
static void signal_selected_frame(GtkSingleSelection* selection, GParamSpec* pspec, GbAppAsheets* self)
{
    asheets_animations* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    g_return_if_fail(anim != NULL);
    if (anim->frames_count == 0)return;

    guint position = gtk_single_selection_get_selected(selection);
    if (position == -1)return;

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->frame_duration), anim->frames[position].duration);
}

/**
 * @brief Función que se ejecuta cuando se cambia la duración de un frame seleccionado.
 *
 * Esta función es llamada cuando se cambia el valor del GtkSpinButton asociado a la duración de un frame seleccionado.
 * Recibe como parámetros el GtkSpinButton y un puntero a la estructura GbAppAsheets.
 */
static void signal_value_changed_duraction_frame(GtkSpinButton* spin_button, GbAppAsheets* self)
{
    guint position = -1;
    asheets_animations* anim = fn_animation_selected(self, &position);
    g_return_if_fail(anim != NULL);

    anim->frames[position].duration = (float)gtk_spin_button_get_value(spin_button);
}

/**
 * @brief Función de señal para el botón de eliminación de marco en la barra de herramientas.
 *
 * Esta función se ejecuta cuando se hace clic en el botón de eliminación de marco en la barra de herramientas.
 * Recibe un puntero al widget que emitió la señal y un puntero a la estructura GbAppAsheets.
 *
 * @param widget El widget que emitió la señal.
 * @param self   Puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_remove_frame(GtkWidget* widget, GbAppAsheets* self)
{
    guint position = -1;
    asheets_animations* anim = fn_animation_selected(self, &position);
    g_return_if_fail(anim != NULL);

    fn_animation_remove_frame(self, anim, position);
}

/**
 * @brief Función de señal para el botón de mover a la izquierda del marco de la barra de herramientas.
 *
 * Esta función se llama cuando se hace clic en el botón de mover a la izquierda del frame en la barra de herramientas.
 * Toma un puntero al widget del botón y un puntero a la estructura GbAppAsheets.
 *
 * @param widget El widget del botón que emitió la señal.
 * @param self Puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_move_left_frame(GtkWidget* widget, GbAppAsheets* self)
{
    fn_frame_selected_move_to_direction(self, ASHEETS_DIRECTION_MOVE_FRAME_LEFT);
}

/**
 * @brief Función de señal para el botón de mover a la derecha del frame seleccionado.
 *
 * Esta función se llama cuando se hace clic en el botón de mover a la derecha en la barra de herramientas.
 *
 * @param widget El widget del botón que activó la señal.
 * @param self   El puntero a la estructura GbAppAsheets que contiene los datos de la aplicación.
 */
static void signal_toolbar_btn_move_right_frame(GtkWidget* widget, GbAppAsheets* self)
{
    fn_frame_selected_move_to_direction(self, ASHEETS_DIRECTION_MOVE_FRAME_RIGHT);
}

/**
 * @brief Función que se ejecuta cuando se hace clic en el botón de copiar marco en la barra de herramientas.
 *
 * @param widget El widget que emitió la señal.
 * @param self   El puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_copy_frame(GtkWidget* widget, GbAppAsheets* self)
{
    guint position;
    asheets_animations* anim = fn_animation_selected(self, &position);
    g_return_if_fail(anim != NULL);

    self->frame_copy = anim->frames[position];
}

/**
 * @brief Función que se ejecuta cuando se presiona el botón de pegar en la barra de herramientas.
 *
 * @param widget El widget que emitió la señal.
 * @param self   El puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_paste_frame(GtkWidget* widget, GbAppAsheets* self)
{
    guint position;
    asheets_animations* anim = fn_animation_selected(self, &position);
    g_return_if_fail(anim != NULL);

    fn_animation_add_frame(self, anim, &self->frame_copy);
}

/**
 * @brief Función que se ejecuta cuando se hace clic en el botón de guardar en la barra de herramientas.
 *        Deserializa los datos de GbAppAsheets y los guarda en un archivo.
 *
 * @param widget El widget que generó la señal.
 * @param self   El puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_save_asheets_deserialize_to_file(GtkWidget* widget, GbAppAsheets* self)
{
    fn_save_asheets_data_to_file(self);
    // fn_animation_selected_play(self);
}

static GbAppAsheets* gbapp_asheets_template(GbAppAsheets* self)
{
    GtkWidget* scroll, * vbox_anim, * viewport, * toolbar, * paned_main, * paned2, * vbox_anims;

    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);

    toolbar = gapp_widget_toolbar_new();
    gtk_box_append(self, toolbar);
    {
        GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
        g_signal_connect(btn_s, "clicked", G_CALLBACK(signal_toolbar_btn_save_asheets_deserialize_to_file), self);
        gtk_box_append(toolbar, btn_s);
    }

    paned_main = gapp_widget_paned_new(GTK_ORIENTATION_VERTICAL, FALSE);
    gtk_widget_set_size_request(paned_main, -1, 250);
    gtk_box_append(self, paned_main);
    {
        viewport = gapp_gobu_embed_new();
        g_signal_connect(viewport, "gobu-embed-start", G_CALLBACK(signal_viewport_start), self);
        g_signal_connect(viewport, "gobu-embed-render", G_CALLBACK(signal_viewport_render), self);
        gtk_paned_set_start_child(GTK_PANED(paned_main), viewport);

        // --------------------
        // ?List Animations
        // --------------------
        vbox_anims = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_size_request(vbox_anims, 325, -1);
        {
            toolbar = gapp_widget_toolbar_new();
            gtk_box_append(vbox_anims, toolbar);
            {
                GtkWidget* btn_n = gapp_widget_button_new_icon_with_label("document-new-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_n, "New animation");
                gtk_box_append(toolbar, btn_n);
                g_signal_connect(btn_n, "clicked", G_CALLBACK(signal_toolbar_btn_new_animation), self);

                GtkWidget* btn_r = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_r, "Remove animation");
                gtk_box_append(toolbar, btn_r);
                g_signal_connect(btn_r, "clicked", G_CALLBACK(signal_toolbar_btn_remove_animation), self);

                GtkWidget* btn_d = gapp_widget_button_new_icon_with_label("bookmark-new-symbolic", "Default");
                gtk_widget_set_tooltip_text(btn_d, "Set as default animation");
                gtk_box_append(toolbar, btn_d);
                g_signal_connect(btn_d, "clicked", G_CALLBACK(signal_toolbar_btn_default_animation), self);
            }

            GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_list_item_factory_setup_animation), self);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_list_item_factory_bind_animation), self);

            self->selection_anim = gtk_single_selection_new(self->list_anim);
            self->list_view_anim = gtk_list_view_new(self->selection_anim, factory);
            gtk_widget_add_css_class(self->list_view_anim, "navigation-sidebar");
            g_signal_connect(self->selection_anim, "notify::selected", G_CALLBACK(signal_selected_animation), self);
            gtk_single_selection_set_can_unselect(self->selection_anim, TRUE);
            gtk_single_selection_set_selected(self->selection_anim, -1);
            g_timeout_add(10, (GSourceFunc)fn_source_func_selected_first_animation, self);

            scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->list_view_anim);
            gtk_widget_set_vexpand(scroll, TRUE);
            gtk_box_append(vbox_anims, scroll);
        }

        // --------------------
        // ?List frames
        // --------------------
        self->vbox_frames_anims = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_sensitive(self->vbox_frames_anims, FALSE);
        {
            self->vbox_frames_toolbar = gapp_widget_toolbar_new();
            gtk_box_append(self->vbox_frames_anims, self->vbox_frames_toolbar);
            {
                GtkWidget* btn_aframesheet = gapp_widget_button_new_icon_with_label("view-grid-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_aframesheet, "Add frames from sprite sheet");
                gtk_box_append(self->vbox_frames_toolbar, btn_aframesheet);

                gtk_box_append(self->vbox_frames_toolbar, gapp_widget_separator_h());

                GtkWidget* btn_cframe = gapp_widget_button_new_icon_with_label("edit-copy-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_cframe, "Copy Frame");
                gtk_box_append(self->vbox_frames_toolbar, btn_cframe);
                g_signal_connect(btn_cframe, "clicked", G_CALLBACK(signal_toolbar_btn_copy_frame), self);

                GtkWidget* btn_pframe = gapp_widget_button_new_icon_with_label("edit-paste-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_pframe, "Paste Frame");
                gtk_box_append(self->vbox_frames_toolbar, btn_pframe);
                g_signal_connect(btn_pframe, "clicked", G_CALLBACK(signal_toolbar_btn_paste_frame), self);

                gtk_box_append(self->vbox_frames_toolbar, gapp_widget_separator_h());

                GtkWidget* btn_ml = gapp_widget_button_new_icon_with_label("go-previous-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_ml, "Move frame left");
                gtk_box_append(self->vbox_frames_toolbar, btn_ml);
                g_signal_connect(btn_ml, "clicked", G_CALLBACK(signal_toolbar_btn_move_left_frame), self);

                GtkWidget* btn_mr = gapp_widget_button_new_icon_with_label("go-next-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_mr, "Move frame right");
                gtk_box_append(self->vbox_frames_toolbar, btn_mr);
                g_signal_connect(btn_mr, "clicked", G_CALLBACK(signal_toolbar_btn_move_right_frame), self);

                GtkWidget* btn_r = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_r, "Remove frame");
                gtk_box_append(self->vbox_frames_toolbar, btn_r);
                g_signal_connect(btn_r, "clicked", G_CALLBACK(signal_toolbar_btn_remove_frame), self);

                gtk_box_append(self->vbox_frames_toolbar, gapp_widget_separator_h());

                gtk_box_append(self->vbox_frames_toolbar, gtk_label_new("Frame Duration:"));

                self->frame_duration = gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->frame_duration), 1.0);
                gtk_widget_set_tooltip_text(self->frame_duration, "Frame Duration");
                gtk_box_append(self->vbox_frames_toolbar, self->frame_duration);
                g_signal_connect(self->frame_duration, "value-changed", G_CALLBACK(signal_value_changed_duraction_frame), self);
            }

            GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_list_item_factory_setup_frame), self);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_list_item_factory_bind_frame), self);

            self->selection_frame = gtk_single_selection_new(self->list_frame);
            self->list_view_frame = gtk_grid_view_new(self->selection_frame, factory);
            gtk_widget_add_css_class(self->list_view_frame, "navigation-sidebar");
            gtk_grid_view_set_max_columns(self->list_view_frame, 15);
            gtk_grid_view_set_min_columns(self->list_view_frame, 2);
            g_signal_connect(self->selection_frame, "notify::selected", G_CALLBACK(signal_selected_frame), self);

            scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->list_view_frame);
            gtk_widget_set_vexpand(scroll, TRUE);
            gtk_box_append(self->vbox_frames_anims, scroll);

            GtkDropTarget* target = gtk_drop_target_new(G_TYPE_LIST_STORE, GDK_ACTION_COPY);
            g_signal_connect(target, "drop", G_CALLBACK(signal_drop_assets_frames), self);
            gtk_widget_add_controller(self->list_view_frame, GTK_EVENT_CONTROLLER(target));
        }

        paned2 = gapp_widget_paned_new_with_notebook(GTK_ORIENTATION_HORIZONTAL, TRUE,
            gtk_label_new("Animation"), vbox_anims,
            gtk_label_new("Frames"), self->vbox_frames_anims);
        gtk_paned_set_end_child(GTK_PANED(paned_main), paned2);
    }

    return self;
}


/**
 * Crea un nuevo Animation Sprite Sheets.
 *
 * @return Un nuevo widget que Animation Sprite Sheets.
 */
GtkWidget* gbapp_asheets_new(const gchar* filename)
{
    GbAppAsheets* self = g_object_new(GBAPP_TYPE_ASHEETS, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    self->filename = gb_strdup(filename);
    char* name = gb_fs_get_name(filename, false);

    fn_asheets_serialize_from_file(self);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 0, name, gbapp_asheets_template(self));

    return self;
}

