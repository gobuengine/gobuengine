#include "gobu_editor_world_viewport.h"

struct _GobuEditorWorldViewport
{
    GtkWidget parent_instance;
    GtkWidget* embed;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuEditorWorldViewport, gobu_editor_world_viewport, GOBU_TYPE_GOBU_EMBED)

Texture2D paint_texture;
ecs_entity_t entity_paint;
Vector2 mouse_drop;
gboolean mouse_down;

ECS_COMPONENT_DECLARE(cmp_test);

static void gobu_editor_world_viewport_class_init(GobuEditorWorldViewportClass* klass)
{
    // GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

/**
 * Maneja la señal de renderizado en el viewport del mundo en el editor de Gobu.
 */
static void gobu_editor_world_viewport_signal_render(GtkWidget* embed, GobuEditorWorldViewport* data)
{
    GobuEditorWorldViewportPrivate* private = gobu_editor_world_viewport_get_instance_private(data);
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
static void gobu_editor_world_viewport_signal_drop_drop(GtkDropTarget* self, const GValue* value, gdouble x, gdouble y, gpointer user_data)
{
    GFile* file = g_value_get_object(value);
    Texture2D tmp = LoadTexture(g_file_get_path(file));
    ecs_entity_t e = bugo_ecs_entity_new(mouse_drop, (Vector2) { 1.0f, 1.0f }, 0.0f, (Vector2) { 0.5f, 0.5f });
    bugo_ecs_set_sprite(e, &(ComponentSprite){.texture = tmp, .tint = WHITE});
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
static GdkDragAction* gobu_editor_world_viewport_signal_drop_enter(GtkDropTarget* self, gdouble x, gdouble y, gpointer user_data)
{
    const GValue* value = gtk_drop_target_get_value(self);

    if (!G_VALUE_HOLDS(value, G_TYPE_FILE))
        return;

    GFile* file = G_FILE(g_value_get_object(value));

    // UnloadTexture(paint_texture);
    paint_texture = LoadTexture(g_file_get_path(file));
    entity_paint = bugo_ecs_entity_new((Vector2) { x, y }, (Vector2) { 1.0f, 1.0f }, 0.0f, (Vector2) { 0.5f, 0.5f });
    bugo_ecs_set_sprite(entity_paint, &(ComponentSprite){.texture = paint_texture, .tint = WHITE});

    return TRUE;
}

/**
 * Maneja la señal de salida de arrastre del viewport del mundo en el editor de Gobu.
 *
 * @param selft      El objetivo de arrastre que recibe la señal de salida.
 * @param user_data  Datos de usuario opcionales relacionados con la salida de arrastre.
 */
static void gobu_editor_world_viewport_signal_drop_leave(GtkDropTarget* self, gpointer user_data)
{
    UnloadTexture(paint_texture);
    bugo_ecs_entity_delete(entity_paint);
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
static GdkDragAction* gobu_editor_world_viewport_signal_drop_motion(GtkDropTarget* self, gdouble x, gdouble y, gpointer data)
{
    GobuEditorWorldViewportPrivate* private = gobu_editor_world_viewport_get_instance_private(data);

    ecs_entity_t camera_t = bugo_ecs_get_camera2d_id();
    ComponentCamera* camera = (ECS_CAST(const ComponentCamera*, ecs_get_id(bugo_ecs_world(), camera_t, camera_t)));

    ComponentRenderer* render = bugo_ecs_get(entity_paint, bugo_ecs_get_renderer_id());
    render->position = bugo_world_to_screen_2d((Vector2) { (float)x, (float)y }, *camera);
    mouse_drop = render->position;

    return TRUE;
}

/**
 * Inicializa el viewport del mundo en el editor de Gobu.
 *
 * @param self  El viewport del mundo que se inicializa.
 */
void gobu_editor_world_viewport_signal_pressed(GtkGestureClick* gesture, gint n_press, double x, double y, GobuEditorWorldViewport* viewport)
{
    mouse_down = true;
}

/**
 * Inicializa el viewport del mundo en el editor de Gobu.
 *
 * @param self  El viewport del mundo que se inicializa.
 */
void gobu_editor_world_viewport_signal_released(GtkGestureClick* gesture, gint n_press, double x, double y, GobuEditorWorldViewport* viewport)
{
    mouse_down = false;
}

/**
 * Inicializa el viewport del mundo en el editor de Gobu.
 *
 * @param self  El viewport del mundo que se inicializa.
 */
void gobu_editor_world_viewport_signal_zoom(GtkEventControllerScroll* controller, gdouble dx, gdouble dy, gpointer widget)
{
    GobuEditorWorldViewportPrivate* private = gobu_editor_world_viewport_get_instance_private(widget);
    Vector2 mouse = private->mouse;

    ecs_entity_t camera_t = bugo_ecs_get_camera2d_id();
    ComponentCamera* camera = (ECS_CAST(const ComponentCamera*, ecs_get_id(bugo_ecs_world(), camera_t, camera_t)));
    Vector2 mouseWorldPos = bugo_world_to_screen_2d(mouse, *camera);

    camera->offset = mouse;
    camera->target = mouseWorldPos;
    camera->zoom += (dy * 0.125f);
    if (camera->zoom <= 0) camera->zoom = 0.125f;
}

static void gobu_editor_world_viewport_signal_motion(GtkEventControllerMotion* controller, double x, double y, GtkWidget* widget)
{
    gtk_widget_set_can_focus(GTK_GL_AREA(widget), true);
    gtk_widget_grab_focus(widget);

    GobuEditorWorldViewportPrivate* private = gobu_editor_world_viewport_get_instance_private(widget);
    private->mouse = (Vector2){ (float)x, (float)y };

    if (mouse_down) {
        ecs_entity_t camera_t = bugo_ecs_get_camera2d_id();
        Camera2D* camera = (ECS_CAST(const Camera2D*, ecs_get_id(bugo_ecs_world(), camera_t, camera_t)));

        Vector2 delta = GetMouseDelta();
        delta = bugo_math_vector2_scale(delta, -1.0f / camera->zoom);
        camera->target = bugo_math_vector2_add(camera->target, delta);
    }
}

static void gobu_editor_world_viewport_init(GobuEditorWorldViewport* self)
{
    GobuEditorWorldViewportPrivate* private = gobu_editor_world_viewport_get_instance_private(self);

    private->camera.rotation = 0.0f;
    private->camera.zoom = 1.0f;
    private->camera.target = bugo_math_vector2_zero();
    private->camera.offset = bugo_math_vector2_zero();
    private->bGridShow = TRUE;

    // self->embed = gobu_gobu_embed_new();
    g_signal_connect(self, "gobu-embed-render", G_CALLBACK(gobu_editor_world_viewport_signal_render), self);

    GtkDropTarget* target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    gtk_drop_target_set_gtypes(target, (GType[2]) { G_TYPE_FILE, G_TYPE_FILE_INFO }, 2);
    gtk_drop_target_set_preload(target, TRUE);
    g_signal_connect(target, "enter", G_CALLBACK(gobu_editor_world_viewport_signal_drop_enter), self);
    g_signal_connect(target, "leave", G_CALLBACK(gobu_editor_world_viewport_signal_drop_leave), self);
    g_signal_connect(target, "motion", G_CALLBACK(gobu_editor_world_viewport_signal_drop_motion), self);
    g_signal_connect(target, "drop", G_CALLBACK(gobu_editor_world_viewport_signal_drop_drop), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    GtkGestureClick* clic = gtk_gesture_click_new();
    g_signal_connect(clic, "pressed", G_CALLBACK(gobu_editor_world_viewport_signal_pressed), self);
    g_signal_connect(clic, "released", G_CALLBACK(gobu_editor_world_viewport_signal_released), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(clic));

    GtkGestureZoom* zoom = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect(zoom, "scroll", G_CALLBACK(gobu_editor_world_viewport_signal_zoom), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(zoom));

    GtkEventController* motion = gtk_event_controller_motion_new();
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(motion));
    g_signal_connect(motion, "motion", G_CALLBACK(gobu_editor_world_viewport_signal_motion), self);
}

/**
 * Crea un nuevo viewport de mundo en el editor de Gobu.
 *
 * @return Un nuevo widget que representa el viewport de mundo.
 */
GobuEditorWorldViewport* gobu_editor_world_viewport_new(void)
{
    return g_object_new(GOBU_EDITOR_TYPE_WORLD_VIEWPORT, NULL);
}
