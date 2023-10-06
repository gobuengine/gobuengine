#include "gobu_world_viewport.h"
#include "gobu_widget.h"
#include "gobu/gobu-gobu.h"

struct _GobuWorldViewport
{
    GtkWidget parent;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuWorldViewport, gobu_world_viewport, GTK_TYPE_GL_AREA)

/**
 * Inicializa la clase GobuWorldViewport.
 *
 * Esta función se encarga de inicializar la clase GobuWorldViewport, configurando
 * su comportamiento y atributos de clase.
 *
 * @param klass  La clase GobuWorldViewport que se va a inicializar.
 */
static void gobu_world_viewport_class_init(GobuWorldViewportClass *klass)
{
}


ECS_TAG_DECLARE(event_stage);
ECS_TAG_DECLARE(pre_render_stage);
ECS_TAG_DECLARE(render_stage);
ECS_TAG_DECLARE(post_render_stage);
ECS_TAG_DECLARE(ui_render_stage);


static void camera_movement1(ecs_iter_t *it)
{
}

static void camera_movement2(ecs_iter_t *it)
{
}

static void camera_movement3(ecs_iter_t *it)
{
}

static void camera_movement4(ecs_iter_t *it)
{
}

static void camera_movement_ui(ecs_iter_t *it)
{
}


static void rect_system(ecs_iter_t *it)
{
    // gobu_render_frame_begin(screen_width, screen_height, (GobuColor){0.0f, 0.0f, 0.0f, 1.0f});
    // PositionComponent *p = ecs_field(it, PositionComponent, 1);
    // for(int i = 0; i<it->count; i++) {
    //     GobuRectangle r = (GobuRectangle){p[i].x, p[i].y,50.0f,50.0f};
    //     gobu_shape_draw_filled_rect(r, (GobuVec2){1.0f, 1.0f}, (GobuVec2){0.5f, 0.5f}, 0.0f, (GobuColor){1.0f, 0.0f, 1.0f, 1.0f});
    // }
    // gobu_render_frame_end(screen_width, screen_height);
}

/**
 * Función de manejo de la señal de redimensionamiento para un widget GobuWorldViewport en Gobu.
 *
 * Esta función se encarga de manejar la señal de redimensionamiento para un widget GobuWorldViewport
 * en Gobu, utilizando el widget, el ancho y el alto proporcionados, y datos opcionales.
 *
 * @param viewport  El widget GobuWorldViewport para el cual se maneja la señal de redimensionamiento.
 * @param width     El ancho del widget después del redimensionamiento.
 * @param height    El alto del widget después del redimensionamiento.
 * @param data      Datos opcionales que pueden ser pasados al manejo de la señal.
 */
static void gobu_world_viewport_signal_resize(GtkWidget *viewport, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(viewport) != NULL)
        return;

    GobuWorldViewportPrivate *priv = gobu_world_viewport_get_instance_private(viewport);

    priv->width = width;
    priv->height = height;
}

/**
 * Función de manejo de la señal de "realización" (inicialización) para un objeto GobuWorldViewport en Gobu.
 *
 * Esta función se encarga de manejar la señal de "realización" para un objeto GobuWorldViewport
 * en Gobu, utilizando el objeto GobuWorldViewport y datos opcionales proporcionados.
 *
 * @param viewport  El objeto GobuWorldViewport para el cual se maneja la señal de "realización".
 * @param data      Datos opcionales que pueden ser pasados al manejo de la señal.
 */
static void gobu_world_viewport_signal_realize(GobuWorldViewport *viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
        return;

    // GobuWorldViewportPrivate *priv = gobu_world_viewport_get_instance_private(viewport);

    // gobu_render_init();

    // GobuEcsWorld *world = gobu_ecs_world_get();

    // // ecs_set_target_fps(world, 60);

    // ECS_COMPONENT(world, GeometryRender);
    // ECS_COMPONENT(world, PositionComponent);
    // ECS_COMPONENT(world, UIRender);

    // pre_render_stage = ecs_new_w_id(world, EcsPhase);
    // render_stage = ecs_new_w_id(world, EcsPhase);
    // post_render_stage = ecs_new_w_id(world, EcsPhase);
    // event_stage = ecs_new_w_id(world, EcsPhase);
    // ui_render_stage = ecs_new_w_id(world, EcsPhase);

    // ecs_add_pair(world, event_stage, EcsDependsOn, EcsOnUpdate);

    // ecs_add_pair(world, pre_render_stage, EcsDependsOn, EcsOnStore);
    // ecs_add_pair(world, ui_render_stage, EcsDependsOn, pre_render_stage);
    // ecs_add_pair(world, render_stage, EcsDependsOn, ui_render_stage);
    // ecs_add_pair(world, post_render_stage, EcsDependsOn, render_stage);

    // ECS_SYSTEM(world, camera_movement1, pre_render_stage, 0);
    // ECS_SYSTEM(world, camera_movement3, post_render_stage, 0);
    // ECS_SYSTEM(world, camera_movement_ui, ui_render_stage, UIRender);
    // // ECS_SYSTEM(world, camera_movement2, render_stage, GeometryRender);
    // ECS_SYSTEM(world, camera_movement4, event_stage, 0);

    // ECS_SYSTEM(world, rect_system, render_stage, PositionComponent);

    // GobuEcsEntity e = gobu_ecs_entity_new("GeometryRender_1");
    // ecs_set(world, e, GeometryRender, {0});
    // // ecs_set(world, e, PositionComponent, {0});

    // GobuEcsEntity e2 = gobu_ecs_entity_new("Test12");
    // ecs_set(world, e2, PositionComponent, {200, 100});

    // priv->bInit = TRUE;
}

/**
 * Función de manejo de la señal de "unrealización" (desvinculación) para un objeto GobuWorldViewport en Gobu.
 *
 * Esta función se encarga de manejar la señal de "unrealización" para un objeto GobuWorldViewport
 * en Gobu, lo que implica tareas de limpieza o liberación de recursos asociados al objeto.
 *
 * @param self  El objeto GobuWorldViewport que se ha desvinculado.
 */
static void gobu_world_viewport_signal_unrealize(GobuWorldViewport *self)
{
    gtk_gl_area_make_current(self);
    if (gtk_gl_area_get_error(self) != NULL)
        return;

    GobuWorldViewportPrivate *priv = gobu_world_viewport_get_instance_private(self);
    gtk_widget_remove_tick_callback(GTK_WIDGET(self), priv->tick);
}

/**
 * Función de manejo de la señal de renderizado para un objeto GobuWorldViewport en Gobu.
 *
 * Esta función se encarga de manejar la señal de renderizado para un objeto GobuWorldViewport
 * en Gobu, utilizando el objeto GobuWorldViewport y datos opcionales proporcionados.
 *
 * @param viewport  El objeto GobuWorldViewport para el cual se maneja la señal de renderizado.
 * @param data      Datos opcionales que pueden ser pasados al manejo de la señal.
 *
 * @return   Devuelve TRUE si la señal se manejó correctamente, o FALSE en caso contrario.
 */
static gboolean gobu_world_viewport_signal_render(GobuWorldViewport *viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
        return;

    GobuWorldViewportPrivate *priv = gobu_world_viewport_get_instance_private(viewport);

    if (priv->bInit == FALSE)
    {
        gobu_render_init();
        priv->bInit = TRUE;
    }

    gobu_render_frame_begin(priv->width, priv->height, (GobuColor){0.0f, 0.0f, 0.0f, 1.0f});
    {
        if (priv->show_grid == TRUE)
        {
            gobu_shape_draw_checkboard(24, 24, priv->width, priv->height);
        }
    }
    gobu_render_frame_end(priv->width, priv->height);

    gobu_ecs_world_progress();

    return TRUE;
}

/**
 * Maneja el evento de soltar elementos en el viewport del mundo Gobu.
 *
 * Esta función se encarga de manejar el evento de soltar elementos en el
 * viewport del mundo Gobu, utilizando la información proporcionada.
 *
 * @param target  El objeto de destino de la operación de soltar.
 * @param value   El valor relacionado con el elemento soltado.
 * @param x       La posición horizontal donde se soltó el elemento.
 * @param y       La posición vertical donde se soltó el elemento.
 *
 * @return        TRUE si el evento se manejó con éxito, FALSE en caso contrario.
 */
static gboolean gobu_world_viewport_drop(GtkDropTarget *target, const GValue *value, double x, double y)
{
    printf("Drop target: %f, %f\n", x, y);
    return TRUE;
}

/**
 * Función de manejo de ticks para un objeto GobuWorldViewport en Gobu.
 *
 * Esta función se encarga de manejar un tick (paso de tiempo) para un objeto GobuWorldViewport
 * en Gobu, utilizando el reloj de fotogramas GdkFrameClock y datos de usuario opcionales.
 *
 * @param self         El objeto GobuWorldViewport para el cual se maneja el tick.
 * @param frame_clock  El reloj de fotogramas GdkFrameClock utilizado para el timing.
 * @param user_data    Datos de usuario opcionales que pueden ser pasados al manejo de ticks.
 *
 * @return   Devuelve TRUE si se debe seguir recibiendo ticks, o FALSE para detener la recepción.
 */
static gboolean gobu_world_viewport_tick(GobuWorldViewport *self, GdkFrameClock *frame_clock, gpointer user_data)
{
    GobuWorldViewportPrivate *priv = gobu_world_viewport_get_instance_private(self);

    gint64 frame_time = gdk_frame_clock_get_frame_time(frame_clock);

    if (priv->first_frame_time == 0)
    {
        /* No need for changes on first frame */
        priv->first_frame_time = frame_time;
        return G_SOURCE_CONTINUE;
    }

    gtk_widget_queue_draw(self);

    return G_SOURCE_CONTINUE;
}

/**
 * Inicializa un objeto GobuWorldViewport.
 *
 * Esta función se encarga de realizar la inicialización necesaria para un objeto
 * GobuWorldViewport, configurando sus atributos y estado inicial.
 *
 * @param self  El objeto GobuWorldViewport que se va a inicializar.
 */
static void gobu_world_viewport_init(GobuWorldViewport *self)
{
    GtkWidget *box;
    GtkDropTarget *target;

    GobuWorldViewportPrivate *priv = gobu_world_viewport_get_instance_private(self);
    priv->show_grid = TRUE;
    priv->bInit = FALSE;

    gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 3);
    gtk_widget_set_focus_on_click(GTK_WIDGET(self), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "realize", G_CALLBACK(gobu_world_viewport_signal_realize), NULL);
    g_signal_connect(self, "unrealize", G_CALLBACK(gobu_world_viewport_signal_unrealize), NULL);
    g_signal_connect(self, "resize", G_CALLBACK(gobu_world_viewport_signal_resize), NULL);
    g_signal_connect(self, "render", G_CALLBACK(gobu_world_viewport_signal_render), NULL);

    target = gtk_drop_target_new(G_TYPE_STRING, GDK_ACTION_MOVE);
    g_signal_connect(target, "drop", G_CALLBACK(gobu_world_viewport_drop), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(self), gobu_world_viewport_tick, self, NULL);
}

/**
 * Crea un nuevo objeto GobuWorldViewport.
 *
 * Esta función se encarga de crear y devolver un nuevo objeto GobuWorldViewport
 * que se puede utilizar como un viewport para visualizar el mundo Gobu.
 *
 * @return  Un puntero al nuevo objeto GobuWorldViewport.
 */
GobuWorldViewport *gobu_world_viewport_new(void)
{
    return g_object_new(GOBU_TYPE_WORLD_VIEWPORT, NULL);
}
