#include "gobu_editor_world_viewport.h"

struct _GobuEditorWorldViewport
{
    GtkWidget parent_instance;
    GtkWidget *embed;
    ecs_world_t *world;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuEditorWorldViewport, gobu_editor_world_viewport, GOBU_TYPE_GOBU_EMBED)

typedef struct
{
    Vector2 position;
    float rotation;
    Color color;
} cmp_test;

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

    BeginMode2D(private->camera);
    {
        if (private->bGridShow)
            bugo_draw_shape_grid(200, 50);
            
        ecs_progress(data->world, 0);
    }
    EndMode2D();

    DrawFPS(10, 30);
    bugo_draw_text(g_strdup_printf("Actor: %d", ecs_count(data->world, cmp_test)), 100, 30, 20, YELLOW);
    bugo_draw_text("Mouse right button drag to move, mouse wheel to zoom", 10, 10, 10, WHITE);
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

void test(ecs_iter_t *it)
{
    cmp_test *p = ecs_field(it, cmp_test, 1);

    for (int i = 0; i < it->count; i++)
    {
        Vector2 pos = p[i].position;
        gdouble rotation = p[i].rotation;
        bugo_draw_shape_rect((Rectangle){pos.x, pos.y, 5, 5}, (Vector2){2.5f, 2.5f}, 0.0f, p[i].color);
    }
}

void test_rot(ecs_iter_t *it)
{
    cmp_test *p = ecs_field(it, cmp_test, 1);

    for (int i = 0; i < it->count; i++)
    {
        p[i].position.x += (float)bugo_math_random_range(0, 5);
        p[i].position.y += (float)bugo_math_random_range(0, 5);
        p[i].rotation += 1.0f;

        if (p[i].position.x > 500)
        {
            p[i].position.x = -100;
        }

        if (p[i].position.y > 500)
        {
            p[i].position.y = -100;
        }
    }
}

void spawn_actor(GtkGestureClick *gesture, gint n_press, double x, double y, GobuEditorWorldViewport *viewport)
{
    ECS_COMPONENT_DEFINE(viewport->world, cmp_test);

    for (int i = 0; i < 1000; i++)
    {
        ecs_entity_t e = ecs_new_id(viewport->world);
        ecs_set(viewport->world, e, cmp_test, {.position = (Vector2){x, y}, .rotation = 0, .color = RAYWHITE});
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

    self->world = ecs_init();
    ECS_COMPONENT(self->world, cmp_test);
    ECS_SYSTEM(self->world, test_rot, EcsOnUpdate, cmp_test);
    ECS_SYSTEM(self->world, test, EcsOnStore, cmp_test);

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
