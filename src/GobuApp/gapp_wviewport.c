#include "gapp_wviewport.h"
#include <epoxy/gl.h>

enum
{
    VIEWPORT_READY,
    VIEWPORT_RENDER,
    N_SIGNALS
};

static guint signals[N_SIGNALS] = {0};

struct _GappViewport
{
    GtkWidget parent;
    gboolean initialized;
    gfx_backend_t *context_render;
    guint loop;
    guint framebuffer_id;
};

static gboolean gapp_tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer user_data);

G_DEFINE_TYPE(GappViewport, gapp_widget_viewport, GTK_TYPE_GL_AREA)

static void gapp_s_resize(GappViewport *area, gint width, gint height);
static gboolean gapp_s_render(GappViewport *area, GdkGLContext *context, gpointer data);
static void gapp_s_realize(GappViewport *area, GappViewport *self);
static void gapp_s_unrealize(GtkWidget *widget, GappViewport *self);

static void gapp_widget_viewport_class_init(GappViewportClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    signals[VIEWPORT_READY] = g_signal_new("viewport-ready", G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                           0, NULL, NULL, NULL, G_TYPE_NONE, 0);

    signals[VIEWPORT_RENDER] = g_signal_new("viewport-render", G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                            0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT, 0);
}

static void gapp_widget_viewport_init(GappViewport *self)
{
    // gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 3);
    // gtk_gl_area_set_auto_render(GTK_GL_AREA(self), FALSE);
    // gtk_gl_area_set_allowed_apis(GTK_GL_AREA(self), GDK_GL_API_GL);
    // gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(self), TRUE);
    gtk_widget_set_focus_on_click(GTK_GL_AREA(self), true);
    gtk_widget_set_focusable(GTK_GL_AREA(self), true);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "realize", G_CALLBACK(gapp_s_realize), self);
    g_signal_connect(self, "unrealize", G_CALLBACK(gapp_s_unrealize), self);
    g_signal_connect(self, "render", G_CALLBACK(gapp_s_render), self);
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

    self->loop = gtk_widget_add_tick_callback(GTK_WIDGET(self), gapp_tick_callback, self, NULL);
}

static void gapp_s_resize(GappViewport *area, gint width, gint height)
{
    if (gtk_gl_area_get_error(GTK_GL_AREA(area)) != NULL)
        return;

    if (area->initialized)
        gfxb_viewport_resize(area->context_render, width, height);
}

static gboolean gapp_s_render(GappViewport *area, GdkGLContext *context, gpointer data)
{
    if (gtk_gl_area_get_error(area) != NULL)
        return FALSE;

    int width = gtk_widget_get_width(GTK_WIDGET(area));
    int height = gtk_widget_get_height(GTK_WIDGET(area));

    if (!area->initialized)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &area->framebuffer_id);

        area->initialized = TRUE;
        area->context_render = gfxb_viewport_create();
        gfxb_viewport_resize(area->context_render, width, height);
        gfxb_viewport_framebuffer(area->context_render, area->framebuffer_id);
        g_signal_emit(area, signals[VIEWPORT_READY], 0);
    }

    gfxb_viewport_begin(area->context_render);
    {
        g_signal_emit(area, signals[VIEWPORT_RENDER], 0, width, height);
    }
    gfxb_viewport_end(area->context_render);
    glFlush();

    return TRUE;
}

static void gapp_s_realize(GappViewport *area, GappViewport *self)
{
    gtk_gl_area_make_current(GTK_GL_AREA(area));

    if (gtk_gl_area_get_error(GTK_GL_AREA(area)) != NULL)
        return;
}

static void gapp_s_unrealize(GtkWidget *widget, GappViewport *self)
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
        return;

    gtk_widget_remove_tick_callback(self, self->loop);
    gfxb_destroy(self->context_render);
}

static gboolean gapp_tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer user_data)
{
    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}

// -- BEGIN API PUBLIC

GappViewport *gapp_widget_viewport_new(void)
{
    return g_object_new(GAPP_WIDGET_TYPE_VIEWPORT, NULL);
}

gfx_backend_t *gapp_widget_viewport_context(GappViewport *self)
{
    g_return_val_if_fail(GAPP_WIDGET_IS_VIEWPORT(self), NULL);
    return self->context_render;
}

// -- END API PUBLIC
