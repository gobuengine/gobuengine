#include "gapp_viewport.h"
#include "pixio/pixio.h"
#include <epoxy/gl.h>

enum
{
    SIGNAL_VIEWPORT_INIT,
};

struct _GappViewport
{
    GtkWidget parent;
    ecs_world_t *world;
    ecs_entity_t root;
    gboolean initialized;
    gint64 first_frame_time;
    guint tick;
};

G_DEFINE_TYPE(GappViewport, gapp_viewport, GTK_TYPE_GL_AREA)

static gboolean gtk_ray_ticka(GappViewport *self, GdkFrameClock *frame_clock, gpointer user_data);
static void gapp_s_resize(GtkWidget *widget, gint width, gint height);
static gboolean gapp_s_render(GtkGLArea *area, GdkGLContext *context, GappViewport *self);
static void gapp_s_realize(GtkWidget *widget);
static void gapp_s_unrealize(GtkWidget *widget);

static void gapp_viewport_class_init(GappViewportClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    g_signal_new("viewport-ready", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

    g_signal_new("viewport-render", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static void gapp_viewport_init(GappViewport *self)
{
    // gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 0);
    // gtk_gl_area_set_auto_render(GTK_GL_AREA(self), TRUE);
    // gtk_gl_area_set_allowed_apis(GTK_GL_AREA(self), GDK_GL_API_GL);
    // gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(self), TRUE);
    gtk_widget_set_focus_on_click(GTK_GL_AREA(self), true);
    gtk_widget_set_focusable(GTK_GL_AREA(self), true);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "render", G_CALLBACK(gapp_s_render), self);
    g_signal_connect(self, "realize", G_CALLBACK(gapp_s_realize), self);
    g_signal_connect(self, "unrealize", G_CALLBACK(gapp_s_unrealize), self);
    g_signal_connect(self, "resize", G_CALLBACK(gapp_s_resize), self);

    // GtkEventController *zoom = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(zoom));
    // g_signal_connect(zoom, "scroll", G_CALLBACK(signal_viewport_zoom), self);

    // GtkEventController *motion = gtk_event_controller_motion_new();
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(motion));
    // g_signal_connect(motion, "motion", G_CALLBACK(signal_viewport_mouse_move), self);

    // GtkGesture *gesture = gtk_gesture_click_new();
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(gesture));
    // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    // g_signal_connect(gesture, "pressed", G_CALLBACK(signal_viewport_mouse_button_pressed), self);
    // g_signal_connect(gesture, "released", G_CALLBACK(signal_viewport_mouse_button_released), self);

    gtk_widget_add_tick_callback(GTK_WIDGET(self), gtk_ray_ticka, self, NULL);
}

/**
 * gtk_ray_ticka:
 * @self: (transfer none): El #GtkWidget que se va a redibujar.
 * @frame_clock: (transfer none): El #GdkFrameClock que proporciona la señal de tick.
 * @user_data: (transfer none): Datos adicionales pasados a la función (no utilizado).
 *
 * Función de callback para el tick del reloj de cuadros.
 *
 * Esta función se llama en cada tick del reloj de cuadros y solicita
 * un redibujado del widget. Se utiliza para actualizar continuamente
 * la visualización del widget, lo que es útil para animaciones o
 * contenido dinámico.
 *
 * Returns: Siempre devuelve %G_SOURCE_CONTINUE para mantener el callback activo.
 *
 * Since: 1.0
 */
static gboolean gtk_ray_ticka(GappViewport *self, GdkFrameClock *frame_clock, gpointer user_data)
{
    GdkFrameTimings *prev_timings;
    gint64 prev_frame_time, frame_time, frame;
    gint64 history_start, history_len;

    frame = gdk_frame_clock_get_frame_counter(frame_clock);
    frame_time = gdk_frame_clock_get_frame_time(frame_clock);

    if (self->first_frame_time == 0)
    {
        self->first_frame_time = frame_time;
        return G_SOURCE_CONTINUE;
    }

    gtk_widget_queue_draw(self);

    history_start = gdk_frame_clock_get_history_start(frame_clock);
    if (frame % 60 == 0)
    {
        history_len = frame - history_start;
        if (history_len > 0)
        {
            prev_timings = gdk_frame_clock_get_timings(frame_clock, frame - history_len);
            prev_frame_time = gdk_frame_timings_get_frame_time(prev_timings);
        }
    }

    return G_SOURCE_CONTINUE;
}

static void gapp_s_resize(GtkWidget *widget, gint width, gint height)
{
    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
        return;

    pixi_adjust_viewport_size(width, height);
}

static gboolean gapp_s_render(GtkGLArea *area, GdkGLContext *context, GappViewport *self)
{
    if (gtk_gl_area_get_error(area) != NULL)
        return FALSE;

    if (!self->initialized)
    {
        int width = gtk_widget_get_width(GTK_WIDGET(area));
        int height = gtk_widget_get_height(GTK_WIDGET(area));
        self->initialized = true;

        pixi_init(width, height);

        g_signal_emit_by_name(self, "viewport-ready", width, height, 0);
    }

    // // we can start by clearing the buffer
    // pixio_world_process(self->world, 0);
    g_signal_emit_by_name(self, "viewport-render", 0);
    glFlush();

    return TRUE;
}

static void gapp_s_realize(GtkWidget *widget)
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
        return;
}

static void gapp_s_unrealize(GtkWidget *widget)
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
        return;

    pixi_shutdown();
}

// -- BEGIN API PUBLIC

GappViewport *gapp_viewport_new(void)
{
    return g_object_new(GAPP_TYPE_VIEWPORT, NULL);
}

/**
 * Obtiene el mundo ECS asociado con el viewport.
 *
 * @param self Puntero al GappViewport del cual se quiere obtener el mundo.
 * @return Puntero al ecs_world_t asociado con el viewport.
 */
ecs_world_t *gapp_viewport_get_world(const GappViewport *self)
{
    if (self == NULL)
    {
        return NULL;
    }
    return self->world;
}

ecs_entity_t gapp_viewport_get_root(const GappViewport *self)
{
    return self->root;
}

// -- END API PUBLIC
