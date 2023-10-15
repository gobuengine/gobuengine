#include "gobu_editor_world_viewport.h"

struct _GobuEditorWorldViewport
{
    GtkWidget parent_instance;
    GtkWidget *embed;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuEditorWorldViewport, gobu_editor_world_viewport, GOBU_TYPE_GOBU_EMBED)

Texture2D texture_logo;

ECS_COMPONENT_DECLARE(cmp_test);

static void gobu_editor_world_viewport_class_init(GobuEditorWorldViewportClass *klass)
{
    // GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

/**
 * Maneja la señal de renderizado en el viewport del mundo en el editor de Gobu.
 */
static void gobu_editor_world_viewport_signal_render(GtkWidget *embed, GobuEditorWorldViewport *data)
{
    GobuEditorWorldViewportPrivate *private = gobu_editor_world_viewport_get_instance_private(data);
    bugo_ecs_progress(0.0f);
}

/**
 * Maneja la señal de soltar un archivo en el viewport del mundo en el editor de Gobu.
 *
 * @param widget  El widget en el que se suelta el archivo.
 * @param file    El archivo que se suelta.
 * @param x       La posición X del punto de suelta.
 * @param y       La posición Y del punto de suelta.
 */
static void gobu_editor_world_viewport_signal_drop_drop(GtkWidget *widget, GFile *file, gdouble x, gdouble y)
{
}

/**
 * Maneja la señal de entrada de arrastre en el viewport del mundo en el editor de Gobu.
 *
 * @param self       El objetivo de arrastre que recibe la señal.
 * @param x          La posición X del punto de entrada.
 * @param y          La posición Y del punto de entrada.
 * @param user_data  Datos de usuario opcionales relacionados con la entrada de arrastre.
 *
 * @return Las acciones de arrastre admitidas en respuesta a la señal.
 */
static GdkDragAction *gobu_editor_world_viewport_signal_drop_enter(GtkDropTarget *self, gdouble x, gdouble y, gpointer user_data)
{
    const GValue *value = gtk_drop_target_get_value(self);

    if (!G_VALUE_HOLDS(value, G_TYPE_FILE))
        return;

    // GFile *file = G_FILE(g_value_get_object(value));
    // paint[paint_n] = LoadTexture(g_file_get_path(file));

    return TRUE;
}

/**
 * Maneja la señal de salida de arrastre del viewport del mundo en el editor de Gobu.
 *
 * @param selft      El objetivo de arrastre que recibe la señal de salida.
 * @param user_data  Datos de usuario opcionales relacionados con la salida de arrastre.
 */
static void gobu_editor_world_viewport_signal_drop_leave(GtkDropTarget *self, gpointer user_data)
{
}

/**
 * Maneja la señal de movimiento de arrastre en el viewport del mundo en el editor de Gobu.
 *
 * @param selft      El objetivo de arrastre que recibe la señal de movimiento.
 * @param x          La posición X del punto de movimiento.
 * @param y          La posición Y del punto de movimiento.
 * @param data       Datos relacionados con el movimiento de arrastre.
 *
 * @return Las acciones de arrastre admitidas en respuesta a la señal de movimiento.
 */
static GdkDragAction *gobu_editor_world_viewport_signal_drop_motion(GtkDropTarget *self, gdouble x, gdouble y, gpointer data)
{
    return TRUE;
}

/**
 * Inicializa el viewport del mundo en el editor de Gobu.
 *
 * @param self  El viewport del mundo que se inicializa.
 */
void spawn_actor(GtkGestureClick *gesture, gint n_press, double x, double y, GobuEditorWorldViewport *viewport)
{
    for (int i = 0; i < 1; i++)
    {
        ecs_entity_t e = bugo_ecs_entity_new((Vector2){x, y}, (Vector2){0.2f, 0.2f}, 0.0f, (Vector2){0.5f, 0.5f});
        bugo_ecs_set_sprite(e, &(ComponentSprite){.texture = texture_logo, .tint = WHITE});
        // bugo_ecs_set_shape_rect(e, &(ComponentShapeRectangle){.width = 50, .height = 50, .color = WHITE});
    }
}

static void gobu_editor_world_viewport_init(GobuEditorWorldViewport *self)
{
    GobuEditorWorldViewportPrivate *private = gobu_editor_world_viewport_get_instance_private(self);

    private->camera.rotation = 0.0f;
    private->camera.zoom = 1.0f;
    private->camera.target = bugo_math_vector2_zero();
    private->camera.offset = bugo_math_vector2_zero();
    private->bGridShow = TRUE;

    texture_logo = bugo_load_texture("D:/549GS/Projects/GobuProject/imagenes/logo_tmp.png");

    // self->embed = gobu_gobu_embed_new();
    g_signal_connect(self, "gobu-embed-render", G_CALLBACK(gobu_editor_world_viewport_signal_render), self);

    GtkDropTarget *target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    gtk_drop_target_set_gtypes(target, (GType[2]){G_TYPE_FILE, G_TYPE_FILE_INFO}, 2);
    gtk_drop_target_set_preload(target, TRUE);
    g_signal_connect(target, "enter", G_CALLBACK(gobu_editor_world_viewport_signal_drop_enter), self);
    g_signal_connect(target, "leave", G_CALLBACK(gobu_editor_world_viewport_signal_drop_leave), self);
    g_signal_connect(target, "motion", G_CALLBACK(gobu_editor_world_viewport_signal_drop_motion), self);
    g_signal_connect(target, "drop", G_CALLBACK(gobu_editor_world_viewport_signal_drop_drop), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    GtkGestureClick *clic = gtk_gesture_click_new();
    g_signal_connect(clic, "pressed", G_CALLBACK(spawn_actor), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(clic));
}

/**
 * Crea un nuevo viewport de mundo en el editor de Gobu.
 *
 * @return Un nuevo widget que representa el viewport de mundo.
 */
GobuEditorWorldViewport *gobu_editor_world_viewport_new(void)
{
    return g_object_new(GOBU_EDITOR_TYPE_WORLD_VIEWPORT, NULL);
}
