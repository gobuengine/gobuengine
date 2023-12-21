#include "gbapp_asheets.h"
#include "gapp_gobu_embed.h"

#define gb_return_if_fail(expr) if(!(expr)) {gb_print_warning(TF("Assertion '%s' failed", #expr)); return;}
#define gb_return_val_if_fail(expr, val) if(!(expr)) {gb_print_warning(TF("Assertion '%s' failed", #expr)); return val;}

typedef enum ASheetsDirectionMoveFrame {
    ASHEETS_DIRECTION_MOVE_FRAME_LEFT = -1,
    ASHEETS_DIRECTION_MOVE_FRAME_RIGHT = 1
}ASheetsDirectionMoveFrame;

struct _GbAppAsheets
{
    GtkWidget parent;
    GtkStringList* list_anim;
    GtkStringList* list_frame;
    GtkWidget* list_view_frame;
    GtkWidget* list_view_anim;
    GtkWidget* frame_duration;
    GtkWidget* btn_paste_frame;
    GtkWidget* btn_copy_frame;
    GtkWidget* btn_move_left_frame;
    GtkWidget* btn_move_right_frame;
    GtkWidget* btn_remove_frame;
    GtkWidget* btn_aframesheet;
    GtkWidget* btn_playanim;
    GtkWidget* btn_anim_remove;
    GtkWidget* btn_anim_add;
    GtkWidget* label_frame_duration;
    GtkSingleSelection* selection_anim;
    GtkSingleSelection* selection_frame;
    int timeout_check_anim;
    gb_world_t* world;
    //
    gchar* filename;
    ecs_entity_t entity;
    gb_animate_sprite_t* animate_sprite;    // component base
    gb_animate_frame_t* frame_copy;         // copia del frame seleccionado
    GHashTable* garbage_collector;          // recoletor de recursos
};

extern GAPP* EditorCore;

static void fn_widget_focus_focus(GtkWidget* widget);
static void fn_clear_string_list(GtkStringList* list);
static void fn_animation_load_list_animation(GbAppAsheets* self);
static void fn_animation_add_frame(GbAppAsheets* self, gb_animate_animation_t* anim, gb_animate_frame_t* frame);
static void fn_animation_remove_frame(GbAppAsheets* self, gb_animate_animation_t* anim, guint post);
static gboolean fn_animation_new(GbAppAsheets* self, const gchar* name);
static gboolean fn_animation_remove(GbAppAsheets* self, guint post);
static gb_animate_animation_t* fn_animation_by_name(GbAppAsheets* self, const gchar* name);
static int fn_animation_index_by_name(GbAppAsheets* self, const gchar* name);
static const gchar* fn_animation_selected_get_name(GbAppAsheets* self);
static gchar* fn_animation_default(GbAppAsheets* self);
static void fn_animation_set_default(GbAppAsheets* self, const gchar* name);
static void fn_animation_load_frames(GbAppAsheets* self, gb_animate_animation_t* anim);
static gboolean fn_animation_exists(GbAppAsheets* self, const gchar* name);
static guint fn_frame_get_index_by_id(GbAppAsheets* self, gb_animate_animation_t* anim, guint position);

G_DEFINE_TYPE_WITH_PRIVATE(GbAppAsheets, gbapp_asheets, GTK_TYPE_BOX);


static void gbapp_asheets_finalize(GObject* object)
{
    GbAppAsheets* self = GBAPP_ASHEETS(object);

    gb_print_info(TF("Asheets Free [%s]\n", gb_fs_get_name(self->filename, false)));

    g_source_remove(self->timeout_check_anim);

    ecs_delete(self->world, self->entity);

    ecs_quit(self->world);
    g_free(self->filename);
}

static void gbapp_asheets_class_init(GbAppAsheetsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = gbapp_asheets_finalize;
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

static gboolean fn_source_func_play_animation_check(GbAppAsheets* self)
{
    if (self->animate_sprite != NULL)
    {
        bool is_playing = gb_animate_sprite_is_playing(self->animate_sprite);

        gtk_button_set_icon_name(GTK_BUTTON(self->btn_playanim), is_playing ? "media-playback-stop-symbolic" : "media-playback-start-symbolic");
    }

    return TRUE;
}

static gboolean fn_source_func_selected_first_animation(GbAppAsheets* self)
{
    gtk_single_selection_set_selected(self->selection_anim, 0);
    return FALSE;
}

static void fn_widget_focus_focus(GtkWidget* widget)
{
    g_timeout_add(10, (GSourceFunc)fn_source_func_grab_focus, widget);
}

/**
 * Guarda los datos de asheets en un archivo.
 *
 * @param self El puntero al objeto GbAppAsheets.
 */
static gboolean fn_save_asheets_data_to_file(GbAppAsheets* self)
{
    char* json = ecs_entity_to_json(self->world, self->entity, &(ecs_iter_to_json_desc_t){.serialize_entities = true, .serialize_values = true});
    gb_fs_write_file(self->filename, json);
    ecs_os_free(json);

    return true;
}

/**
 * @brief Elimina un elemento del recolector de basura de recursos.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param key Clave del elemento a eliminar.
 */
static void fn_resource_garbage_collector_remove(GbAppAsheets* self, const gchar* key)
{
    guint* result = g_hash_table_lookup(self->garbage_collector, key);
    gb_return_if_fail(result != NULL);

    guint n = GPOINTER_TO_UINT(result) - 1;
    n = n < 0 ? 0 : n;
    g_hash_table_insert(self->garbage_collector, gb_strdup(key), GUINT_TO_POINTER(n));

    if (n == 0)
    {
        gb_resource_remove(self->world, key);
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
    guint n = GPOINTER_TO_UINT(g_hash_table_lookup(self->garbage_collector, key));
    n += 1;
    g_hash_table_insert(self->garbage_collector, gb_strdup(key), GUINT_TO_POINTER(n));
}

/**
 * @brief Limpia una lista de cadenas.
 *
 * @param list La lista de cadenas a limpiar.
 */
static void fn_clear_string_list(GtkStringList* list)
{
    gb_return_if_fail(list != NULL);
    guint n = g_list_model_get_n_items(G_LIST_MODEL(list));
    gtk_string_list_splice(list, 0, n, NULL);
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
    // 
    gb_animate_animation_t* ptr = ecs_vec_append_t(NULL, &self->animate_sprite->animations, gb_animate_animation_t);
    ptr->name = gb_strdup(name);
    ptr->fps = 12;
    ptr->loop = false;

    // una animacion tiene un vector de frames con una capacidad inicial de 100
    ecs_vec_init(NULL, &ptr->frames, ecs_id(gb_animate_frame_t), 100);

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
    gb_return_if_fail(name != NULL);

    gb_animate_animation_t* anim = fn_animation_by_name(self, name);
    gb_return_if_fail(anim != NULL);

    for (int i = ecs_vec_count(&anim->frames) - 1; i >= 0; i--)
        fn_animation_remove_frame(self, anim, i);

    if (g_strcmp0(fn_animation_default(self), anim->name) == 0)
        fn_animation_set_default(self, "\0");

    // Borramos de la lista
    int32_t index = fn_animation_index_by_name(self, name);
    gb_ecs_vec_remove_t(&self->animate_sprite->animations, gb_animate_animation_t, index);
    gtk_string_list_remove(self->list_anim, post);

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

    for (int i = 0; i < ecs_vec_count(&self->animate_sprite->animations); i++)
    {
        gb_animate_animation_t* anim = ecs_vec_get_t(&self->animate_sprite->animations, gb_animate_animation_t, i);
        gtk_string_list_append(self->list_anim, gb_strdup(anim->name));
    }
}

/**
 * @brief Obtiene el índice del marco seleccionado.
 *
 * Esta función devuelve el índice del marco seleccionado en la hoja de animación.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @return El índice del marco seleccionado.
 */
static guint fn_frame_get_index_by_id(GbAppAsheets* self, gb_animate_animation_t* anim, guint position)
{
    gb_return_if_fail(position != -1);

    for (int i = 0; i < ecs_vec_count(&anim->frames); i++)
    {
        gb_animate_frame_t* frame = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, i);
        if (frame->index_ == position)
            return i;
    }

    return -1;
}

/**
 * Busca una animación por su nombre en la estructura gb_animate_animation_t.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param name Nombre de la animación a buscar.
 * @return Puntero a la animación encontrada o NULL si no se encuentra.
 */
static gb_animate_animation_t* fn_animation_by_name(GbAppAsheets* self, const gchar* name)
{
    for (int i = 0; i < ecs_vec_count(&self->animate_sprite->animations); i++)
    {
        gb_animate_animation_t* anim = ecs_vec_get_t(&self->animate_sprite->animations, gb_animate_animation_t, i);
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
    for (int i = 0; i < ecs_vec_count(&self->animate_sprite->animations); i++)
    {
        gb_animate_animation_t* anim = ecs_vec_get_t(&self->animate_sprite->animations, gb_animate_animation_t, i);
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
    return gb_strdup(self->animate_sprite->animation);
}

/**
 * @brief Establece el nombre de la animación por defecto.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param name Nombre de la animación por defecto.
 */
static void fn_animation_set_default(GbAppAsheets* self, const gchar* name)
{
    self->animate_sprite->animation = gb_strdup(name);
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
    guint position = gtk_single_selection_get_selected(self->selection_anim);
    gb_return_val_if_fail(position != -1, NULL);
    return gtk_string_list_get_string(self->list_anim, position);
}

/**
 * @brief Agrega un fotograma a una animación.
 *
 * Esta función agrega un fotograma a una animación existente en la estructura GbAppAsheets.
 *
 * @param self Puntero a la estructura GbAppAsheets.
 * @param anim Puntero a la estructura gb_animate_animation_t que representa la animación.
 * @param id Identificador del fotograma a agregar.
 * @param duration Duración del fotograma en segundos.
 */
static void fn_animation_add_frame(GbAppAsheets* self, gb_animate_animation_t* anim, gb_animate_frame_t* frame)
{
    gb_return_if_fail(anim != NULL);

    gb_animate_frame_t* f = ecs_vec_append_t(NULL, &anim->frames, gb_animate_frame_t);
    f->duration = frame->duration;
    f->sprite = frame->sprite;
    f->sprite.texture = gb_resource(self->world, frame->sprite.resource)->texture;

    int32_t index = ecs_vec_count(&anim->frames) - 1;
    f->index_ = index;

    gtk_string_list_append(self->list_frame, gb_strdups("%d", index));
    // gtk_widget_set_sensitive(self->vbox_frames_toolbar, TRUE);

    // garbage_collector
    fn_resource_garbage_collector_add(self, frame->sprite.resource);
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
static void fn_animation_remove_frame(GbAppAsheets* self, gb_animate_animation_t* anim, guint post)
{
    gb_return_if_fail(anim != NULL);

    gb_animate_sprite_stop(self->animate_sprite);

    int index = fn_frame_get_index_by_id(self, anim, post);

    gb_animate_frame_t* frame = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, index);
    // garbage_collector
    fn_resource_garbage_collector_remove(self, frame->sprite.resource);
    // eliminamos el frame
    gb_ecs_vec_remove_t(&anim->frames, gb_animate_frame_t, index);
    // recargamos la lista de frames
    fn_animation_load_frames(self, anim);
    // mantenemos la seleccion en el mismo frame
    int32_t count = ecs_vec_count(&anim->frames);
    guint n = post >= count ? count - 1 : post;
    gtk_single_selection_set_selected(self->selection_frame, n);

    gb_animate_sprite_play(self->animate_sprite);
}

/**
 * @brief Carga los fotogramas de una animación.
 *
 * Esta función carga los fotogramas de una animación en la estructura de datos proporcionada.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param anim Puntero a la estructura de datos de animaciones.
 */
static void fn_animation_load_frames(GbAppAsheets* self, gb_animate_animation_t* anim)
{
    gb_return_if_fail(anim != NULL);

    fn_clear_string_list(self->list_frame);

    for (int i = 0; i < ecs_vec_count(&anim->frames); i++)
    {
        gb_animate_frame_t* frame = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, i);
        frame->index_ = i;

        gtk_string_list_append(self->list_frame, gb_strdups("%d", i));
        fn_resource_garbage_collector_add(self, frame->sprite.resource);
    }
}

/**
 * @brief Mueve el frame seleccionado en la dirección especificada.
 *
 * @param self Puntero al objeto GbAppAsheets.
 * @param direction Dirección en la que se moverá el marco seleccionado.
 */

static void fn_frame_selected_move_to_direction(GbAppAsheets* self, ASheetsDirectionMoveFrame direction)
{
    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    gb_return_if_fail(anim != NULL);

    guint position = gtk_single_selection_get_selected(self->selection_frame);
    int new_pos = position + direction;
    if (new_pos > (ecs_vec_count(&anim->frames) - 1) || new_pos < 0)
        return;

    // intercambiamos frames
    gb_ecs_vec_swap_t(&anim->frames, gb_animate_frame_t, position, new_pos);

    // Es mas facil recargar los datos y seleccionar el frame...
    fn_animation_load_frames(self, anim);
    gtk_single_selection_set_selected(self->selection_frame, new_pos);
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

    asheets->world = gapp_gobu_embed_get_world(viewport);

    gb_camera_t* camera = ecs_get(asheets->world, ecs_lookup(asheets->world, "Engine"), gb_camera_t);
    camera->mode = GB_CAMERA_EDITOR;
    camera->target = (gb_vec2_t){ -width / 2, -height / 2 };

    const char* key = gb_resource_set(asheets->world, asheets->filename);

    asheets->entity = gb_ecs_entity_new(asheets->world, 0, "AnimationSprite", gb_ecs_transform(0, 0));
    gb_ecs_entity_set(asheets->world, asheets->entity, gb_animate_sprite_t, { .resource = key });
    gb_ecs_entity_set(asheets->world, asheets->entity, gb_sprite_t, { 0 });
    ecs_remove(asheets->world, asheets->entity, gb_gizmos_t);

    asheets->animate_sprite = ecs_get(asheets->world, asheets->entity, gb_animate_sprite_t);
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
    gb_return_if_fail(post != -1);

    fn_animation_remove(self, post);

    if (g_list_model_get_n_items(G_LIST_MODEL(self->list_anim)) == 0)
    {
        gtk_widget_set_sensitive(self->btn_playanim, FALSE);
        gtk_widget_set_sensitive(self->btn_anim_remove, FALSE);
        gtk_widget_set_sensitive(self->btn_anim_add, FALSE);
        return;
    }

    gtk_single_selection_set_selected(self->selection_anim, -1);
    g_timeout_add(5, (GSourceFunc)fn_source_func_selected_first_animation, self);
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
    fn_animation_set_default(self, name);

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

    guint count = g_list_model_get_n_items(G_LIST_MODEL(self->list_anim));
    if (count == 0) {
        gtk_widget_set_sensitive(self->btn_aframesheet, FALSE);
        // gtk_widget_set_sensitive(self->btn_playanim, FALSE);
    }

    gb_return_if_fail(name != NULL);


    gb_animate_animation_t* anim = fn_animation_by_name(self, name);
    gb_return_if_fail(anim != NULL);

    fn_animation_load_frames(self, anim);

    gb_animate_sprite_set(self->animate_sprite, name);

    gtk_widget_set_sensitive(self->btn_playanim, ecs_vec_count(&anim->frames) > 0);

    gtk_widget_set_sensitive(self->btn_anim_remove, TRUE);
    gtk_widget_set_sensitive(self->btn_anim_add, TRUE);
    gtk_widget_set_sensitive(self->btn_paste_frame, self->frame_copy != NULL);
    gtk_widget_set_sensitive(self->btn_aframesheet, TRUE);
    gtk_widget_set_sensitive(self->frame_duration, FALSE);
    gtk_widget_set_sensitive(self->label_frame_duration, FALSE);
    gtk_widget_set_sensitive(self->btn_move_left_frame, FALSE);
    gtk_widget_set_sensitive(self->btn_move_right_frame, FALSE);
    gtk_widget_set_sensitive(self->btn_copy_frame, FALSE);
    gtk_widget_set_sensitive(self->btn_remove_frame, FALSE);
}

/**
 * @brief Función que se ejecuta cuando se cambia el valor del GtkSpinButton de la frecuencia de animación.
 *
 * @param spin_button El GtkSpinButton que ha cambiado su valor.
 * @param self El puntero a la estructura GbAppAsheets.
 */
static void signal_value_changed_input_fps_animation(GtkSpinButton* spin_button, GbAppAsheets* self)
{
    gb_animate_animation_t* anim = g_object_get_data(G_OBJECT(spin_button), "gb_animate_animation_t");
    gb_return_if_fail(anim != NULL);

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
    gb_animate_animation_t* anim = g_object_get_data(G_OBJECT(button), "gb_animate_animation_t");
    gb_return_if_fail(anim != NULL);

    anim->loop = gtk_check_button_get_active(button);

    if (anim->loop == TRUE)
        gb_animate_sprite_play(self->animate_sprite);
    else gb_animate_sprite_stop(self->animate_sprite);
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
    gb_animate_animation_t* anim = g_object_get_data(G_OBJECT(stack), "gb_animate_animation_t");

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
            fps = gtk_spin_button_new_with_range(1.0, 120.0, 1.0);
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
    gb_animate_animation_t* anim = fn_animation_by_name(self, anim_name);

    g_object_set_data(G_OBJECT(fps), "gb_animate_animation_t", anim);
    g_object_set_data(G_OBJECT(loop), "gb_animate_animation_t", anim);
    g_object_set_data(G_OBJECT(stack), "gb_animate_animation_t", anim);

    bool is_default = g_strcmp0(fn_animation_default(self), anim_name) == 0;
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

    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    gb_return_if_fail(anim != NULL);

    gchar* id_r = gtk_string_object_get_string(string_object);

    int index = fn_frame_get_index_by_id(self, anim, atoi(id_r));

    gb_animate_frame_t* frame = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, index);

    // obtenemos el path relativo del recurso...
    ecs_entity_t resource = ecs_lookup(self->world, frame->sprite.resource);
    char* relative = ecs_get(self->world, resource, gb_resource_t)->path;

    char* path = gb_path_join(gb_project_get_path(), FOLDER_CONTENT_PROJECT, relative, NULL);
    path = gb_path_normalize(path);

    gtk_image_set_from_file(GTK_IMAGE(icon), path);
    gtk_label_set_text(label, gb_strdups("%d", index));
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
        gb_return_if_fail(animation_name != NULL);

        gb_animate_animation_t* anim = fn_animation_by_name(self, animation_name);
        gb_return_if_fail(anim != NULL);

        guint items_n = g_list_model_get_n_items(G_LIST_MODEL(filess));
        for (int i = 0; i < items_n; i++)
        {
            GFileInfo* file_info = G_FILE_INFO(g_list_model_get_item(G_LIST_MODEL(filess), i));
            GFile* file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

            gchar* filename = g_file_get_path(file);
            if (gb_fs_is_extension(filename, ".png") || gb_fs_is_extension(filename, ".jpg")) {
                gchar* name = gb_str_remove_spaces(gb_fs_get_name(filename, true));

                // crea el resource si no existe
                const char* key = gb_resource_set(self->world, filename);

                fn_animation_add_frame(self, anim, &(gb_animate_frame_t) {
                    .sprite.resource = key, .duration = 1.0
                });
            }
        }

        int n = gtk_single_selection_get_selected(self->selection_anim);
        fn_animation_load_list_animation(self);
        gtk_single_selection_set_selected(self->selection_anim, n);

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
guint pposition = 0;
static void signal_selected_frame(GtkSingleSelection* selection, GParamSpec* pspec, GbAppAsheets* self)
{
    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    gb_return_if_fail(anim != NULL);

    gtk_widget_set_sensitive(self->btn_paste_frame, self->frame_copy != NULL);
    int32_t count = ecs_vec_count(&anim->frames);

    if (count == 0)
    {
        gtk_widget_set_sensitive(self->frame_duration, FALSE);
        gtk_widget_set_sensitive(self->label_frame_duration, FALSE);
        gtk_widget_set_sensitive(self->btn_move_left_frame, FALSE);
        gtk_widget_set_sensitive(self->btn_move_right_frame, FALSE);
        gtk_widget_set_sensitive(self->btn_copy_frame, FALSE);
        gtk_widget_set_sensitive(self->btn_remove_frame, FALSE);
        return;
    }

    gtk_widget_set_sensitive(self->btn_copy_frame, TRUE);
    gtk_widget_set_sensitive(self->btn_remove_frame, TRUE);
    gtk_widget_set_sensitive(self->frame_duration, TRUE);
    gtk_widget_set_sensitive(self->label_frame_duration, TRUE);

    guint position = gtk_single_selection_get_selected(selection);
    gb_return_if_fail(position != -1);

    int32_t index = fn_frame_get_index_by_id(self, anim, position);
    gb_animate_frame_t* frame = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, index);

    gtk_widget_set_sensitive(self->btn_move_left_frame, index);
    gtk_widget_set_sensitive(self->btn_move_right_frame, index < (count - 1));

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->frame_duration), frame->duration);
}

/**
 * @brief Función que se ejecuta cuando se cambia la duración de un frame seleccionado.
 *
 * Esta función es llamada cuando se cambia el valor del GtkSpinButton asociado a la duración de un frame seleccionado.
 * Recibe como parámetros el GtkSpinButton y un puntero a la estructura GbAppAsheets.
 */
static void signal_value_changed_duraction_frame(GtkSpinButton* spin_button, GbAppAsheets* self)
{
    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    gb_return_if_fail(anim != NULL);

    guint position = gtk_single_selection_get_selected(self->selection_frame);
    int index = fn_frame_get_index_by_id(self, anim, position);

    gb_animate_frame_t* frame = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, index);
    frame->duration = gtk_spin_button_get_value(spin_button);
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
    guint position = gtk_single_selection_get_selected(self->selection_frame);
    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
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
    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    gb_return_if_fail(anim != NULL);

    guint position = gtk_single_selection_get_selected(self->selection_frame);
    int index = fn_frame_get_index_by_id(self, anim, position);

    self->frame_copy = ecs_vec_get_t(&anim->frames, gb_animate_frame_t, index);

    gtk_widget_set_sensitive(self->btn_paste_frame, self->frame_copy != NULL);
}

/**
 * @brief Función que se ejecuta cuando se presiona el botón de pegar en la barra de herramientas.
 *
 * @param widget El widget que emitió la señal.
 * @param self   El puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_paste_frame(GtkWidget* widget, GbAppAsheets* self)
{
    gb_animate_animation_t* anim = fn_animation_by_name(self, fn_animation_selected_get_name(self));
    gb_return_if_fail(anim != NULL);

    fn_animation_add_frame(self, anim, self->frame_copy);
}

/**
 * @brief Función que se ejecuta cuando se presiona el botón de reproducción de un fotograma de animación en la barra de herramientas.
 *
 * @param widget El widget del botón que se presionó.
 * @param self El puntero a la estructura GbAppAsheets.
 */
static void signal_toolbar_btn_play_animation_frame(GtkWidget* widget, GbAppAsheets* self)
{
    if (gb_animate_sprite_is_playing(self->animate_sprite) == TRUE)
    {
        gb_animate_sprite_stop(self->animate_sprite);
    }
    else
    {
        gb_animate_sprite_play(self->animate_sprite);
    }
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

}

static GbAppAsheets* gbapp_asheets_template(GbAppAsheets* self)
{
    GtkWidget* scroll, * viewport, * paned_main, * paned2;

    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);

    GtkWidget* toolbar_main = gapp_widget_toolbar_new();
    gtk_box_append(self, toolbar_main);
    {
        GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
        g_signal_connect(btn_s, "clicked", G_CALLBACK(signal_toolbar_btn_save_asheets_deserialize_to_file), self);
        gtk_box_append(toolbar_main, btn_s);
    }

    paned_main = gapp_widget_paned_new(GTK_ORIENTATION_VERTICAL, FALSE);
    gtk_widget_set_size_request(paned_main, -1, 250);
    gtk_box_append(self, paned_main);
    {
        viewport = gapp_gobu_embed_new();
        gapp_gobu_embed_set_grid(viewport, TRUE);
        g_signal_connect(viewport, "gobu-embed-start", G_CALLBACK(signal_viewport_start), self);
        gtk_paned_set_start_child(GTK_PANED(paned_main), viewport);
        // --------------------
        // ?List Animations
        // --------------------
        GtkWidget* vbox_anims = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_size_request(vbox_anims, 325, -1);
        {
            GtkWidget* toolbar = gapp_widget_toolbar_new();
            gtk_box_append(vbox_anims, toolbar);
            {
                GtkWidget* btn_n = gapp_widget_button_new_icon_with_label("document-new-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_n, "New animation");
                gtk_box_append(toolbar, btn_n);
                g_signal_connect(btn_n, "clicked", G_CALLBACK(signal_toolbar_btn_new_animation), self);

                self->btn_anim_remove = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_anim_remove, "Remove animation");
                gtk_box_append(toolbar, self->btn_anim_remove);
                gtk_widget_set_sensitive(self->btn_anim_remove, FALSE);
                g_signal_connect(self->btn_anim_remove, "clicked", G_CALLBACK(signal_toolbar_btn_remove_animation), self);

                self->btn_anim_add = gapp_widget_button_new_icon_with_label("bookmark-new-symbolic", "Default");
                gtk_widget_set_tooltip_text(self->btn_anim_add, "Set as default animation");
                gtk_box_append(toolbar, self->btn_anim_add);
                gtk_widget_set_sensitive(self->btn_anim_add, FALSE);
                g_signal_connect(self->btn_anim_add, "clicked", G_CALLBACK(signal_toolbar_btn_default_animation), self);
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
        GtkWidget* box_frames = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        {
            GtkWidget* toolbar_frames = gapp_widget_toolbar_new();
            gtk_box_append(box_frames, toolbar_frames);
            {
                self->btn_playanim = gapp_widget_button_new_icon_with_label("media-playback-start-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_playanim, "Play animation");
                gtk_box_append(toolbar_frames, self->btn_playanim);
                gtk_widget_set_sensitive(self->btn_playanim, FALSE);
                g_signal_connect(self->btn_playanim, "clicked", G_CALLBACK(signal_toolbar_btn_play_animation_frame), self);

                // gtk_box_append(toolbar_frames, gapp_widget_separator_h());

                // self->btn_aframesheet = gapp_widget_button_new_icon_with_label("view-grid-symbolic", NULL);
                // gtk_widget_set_tooltip_text(self->btn_aframesheet, "Add frames from sprite sheet");
                // gtk_box_append(toolbar_frames, self->btn_aframesheet);
                // gtk_widget_set_sensitive(self->btn_aframesheet, FALSE);

                gtk_box_append(toolbar_frames, gapp_widget_separator_h());

                self->btn_copy_frame = gapp_widget_button_new_icon_with_label("edit-copy-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_copy_frame, "Copy Frame");
                gtk_box_append(toolbar_frames, self->btn_copy_frame);
                gtk_widget_set_sensitive(self->btn_copy_frame, FALSE);
                g_signal_connect(self->btn_copy_frame, "clicked", G_CALLBACK(signal_toolbar_btn_copy_frame), self);

                self->btn_paste_frame = gapp_widget_button_new_icon_with_label("edit-paste-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_paste_frame, "Paste Frame");
                gtk_box_append(toolbar_frames, self->btn_paste_frame);
                gtk_widget_set_sensitive(self->btn_paste_frame, FALSE);
                g_signal_connect(self->btn_paste_frame, "clicked", G_CALLBACK(signal_toolbar_btn_paste_frame), self);

                gtk_box_append(toolbar_frames, gapp_widget_separator_h());

                self->btn_move_left_frame = gapp_widget_button_new_icon_with_label("go-previous-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_move_left_frame, "Move frame left");
                gtk_box_append(toolbar_frames, self->btn_move_left_frame);
                gtk_widget_set_sensitive(self->btn_move_left_frame, FALSE);
                g_signal_connect(self->btn_move_left_frame, "clicked", G_CALLBACK(signal_toolbar_btn_move_left_frame), self);

                self->btn_move_right_frame = gapp_widget_button_new_icon_with_label("go-next-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_move_right_frame, "Move frame right");
                gtk_box_append(toolbar_frames, self->btn_move_right_frame);
                gtk_widget_set_sensitive(self->btn_move_right_frame, FALSE);
                g_signal_connect(self->btn_move_right_frame, "clicked", G_CALLBACK(signal_toolbar_btn_move_right_frame), self);

                self->btn_remove_frame = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
                gtk_widget_set_tooltip_text(self->btn_remove_frame, "Remove frame");
                gtk_box_append(toolbar_frames, self->btn_remove_frame);
                gtk_widget_set_sensitive(self->btn_remove_frame, FALSE);
                g_signal_connect(self->btn_remove_frame, "clicked", G_CALLBACK(signal_toolbar_btn_remove_frame), self);

                gtk_box_append(toolbar_frames, gapp_widget_separator_h());

                self->label_frame_duration = gtk_label_new("Frame Duration:");
                gtk_box_append(toolbar_frames, self->label_frame_duration);
                gtk_widget_set_sensitive(self->label_frame_duration, FALSE);

                self->frame_duration = gtk_spin_button_new_with_range(1.0, 120.0, 1.0);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->frame_duration), 1.0);
                gtk_widget_set_tooltip_text(self->frame_duration, "Frame Duration");
                gtk_box_append(toolbar_frames, self->frame_duration);
                gtk_widget_set_sensitive(self->frame_duration, FALSE);
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
            gtk_single_selection_set_can_unselect(self->selection_frame, TRUE);
            gtk_single_selection_set_autoselect(self->selection_frame, FALSE);
            g_signal_connect(self->selection_frame, "notify::selected", G_CALLBACK(signal_selected_frame), self);

            scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->list_view_frame);
            gtk_widget_set_vexpand(scroll, TRUE);
            gtk_box_append(box_frames, scroll);

            GtkDropTarget* target = gtk_drop_target_new(G_TYPE_LIST_STORE, GDK_ACTION_COPY);
            g_signal_connect(target, "drop", G_CALLBACK(signal_drop_assets_frames), self);
            gtk_widget_add_controller(self->list_view_frame, GTK_EVENT_CONTROLLER(target));
        }

        paned2 = gapp_widget_paned_new_with_notebook(GTK_ORIENTATION_HORIZONTAL, TRUE,
            gtk_label_new("Animation"), vbox_anims,
            gtk_label_new("Frames"), box_frames);
        gtk_paned_set_end_child(GTK_PANED(paned_main), paned2);
    }

    self->timeout_check_anim = g_timeout_add(100, (GSourceFunc)fn_source_func_play_animation_check, self);

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

    self->list_anim = gtk_string_list_new(NULL);
    self->list_frame = gtk_string_list_new(NULL);

    self->garbage_collector = g_hash_table_new(g_str_hash, g_str_equal);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 0, name, gbapp_asheets_template(self));

    return self;
}

