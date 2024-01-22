#include "gtkray.h"
#include <epoxy/gl.h>

enum
{
    SIGNAL_START = 0,
    SIGNAL_RESIZE,
    SIGNAL_RENDER,
    SIGNAL_DROP,
    SIGNAL_LAST
};

struct _GtkRay
{
    GtkWidget parent;
};

static guint w_signals[SIGNAL_LAST] = {0};

G_DEFINE_TYPE_WITH_PRIVATE(GtkRay, gtk_ray, GTK_TYPE_GL_AREA);

static void signal_resize(GtkRay *stage, int width, int height, gpointer data);
static gboolean signal_mouse_wheel(GtkEventControllerScroll *controller, gdouble dx, gdouble dy, GtkWidget *widget);
static void signal_mouse_move(GtkEventControllerMotion *controller, double x, double y, GtkWidget *widget);
static void signal_mouse_button_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget);
static void signal_mouse_button_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget);
static void signal_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget);
static void signal_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget);

static void gtk_ray_class_init(GtkRayClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    w_signals[SIGNAL_START] = g_signal_new("raystart",
                                           G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                           0,
                                           NULL, NULL, NULL,
                                           G_TYPE_NONE, 0);

    w_signals[SIGNAL_RENDER] = g_signal_new("rayrender",
                                            G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                                            0,
                                            NULL, NULL, NULL,
                                            G_TYPE_NONE, 0);

    w_signals[SIGNAL_RESIZE] = g_signal_new("rayresize",
                                            G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                            NULL, NULL, NULL,
                                            0,
                                            G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

    w_signals[SIGNAL_DROP] = g_signal_new("raydrop",
                                          G_TYPE_FROM_CLASS(klass),
                                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                          NULL, NULL, NULL,
                                          0,
                                          G_TYPE_NONE, 3, G_TYPE_LIST_STORE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

static void gtk_ray_signal_focus(GtkWidget *stage, gpointer data)
{
    gtk_widget_set_can_focus(GTK_GL_AREA(stage), true);
    gtk_widget_grab_focus(stage);
}

static void gtk_ray_signal_resize(GtkWidget *stage, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(stage) != NULL)
    {
        g_error("gtk_ray_signal_resize");
        return;
    }

    GtkRayPrivate *priv = gtk_ray_get_instance_private(stage);
    priv->width = width;
    priv->height = height;

    // ViewportSizeCallback(width, height);

    g_signal_emit(stage, w_signals[SIGNAL_RESIZE], 0, width, height);
}

static void gtk_ray_signal_realize(GtkRay *stage, gpointer data)
{
    gtk_gl_area_make_current(stage);
    if (gtk_gl_area_get_error(stage) != NULL)
    {
        g_error("gtk_ray_signal_realize");
        return;
    }
}

static void gtk_ray_signal_unrealize(GtkRay *self)
{
    gtk_gl_area_make_current(self);
    GtkRayPrivate *priv = gtk_ray_get_instance_private(self);
    gtk_widget_remove_tick_callback(GTK_WIDGET(self), priv->tick);
}

static gboolean gtk_ray_signal_render(GtkRay *stage, gpointer data)
{
    GtkRayPrivate *priv = gtk_ray_get_instance_private(stage);

    if (gtk_gl_area_get_error(stage) != NULL)
        return FALSE;

    if (!priv->initialize)
    {
        g_signal_emit(stage, w_signals[SIGNAL_START], 0);
        priv->initialize = TRUE;
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    g_signal_emit(stage, w_signals[SIGNAL_RENDER], 0);

    glFlush();

    return TRUE;
}

static gboolean gtk_ray_drop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer user_data)
{
    if (G_VALUE_HOLDS(value, G_TYPE_LIST_STORE))
    {
        g_signal_emit(user_data, w_signals[SIGNAL_DROP], 0, G_LIST_STORE(g_value_get_object(value)), x, y);
        return TRUE;
    }
    return FALSE;
}

static gboolean gtk_ray_tick(GtkRay *self, GdkFrameClock *frame_clock, gpointer user_data)
{
    gtk_widget_queue_draw(self);
    return G_SOURCE_CONTINUE;
}

static void gtk_ray_init(GtkRay *self)
{
    GtkDropTarget *target;

    GtkRayPrivate *priv = gtk_ray_get_instance_private(self);
    priv->initialize = false;

    gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 3);
    // gtk_gl_area_set_auto_render(GTK_GL_AREA(self), TRUE);
    // gtk_gl_area_attach_buffers(GTK_GL_AREA(self));
    gtk_widget_set_focus_on_click(GTK_GL_AREA(self), true);
    gtk_widget_set_focusable(GTK_GL_AREA(self), true);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "realize", G_CALLBACK(gtk_ray_signal_realize), NULL);
    g_signal_connect(self, "unrealize", G_CALLBACK(gtk_ray_signal_unrealize), NULL);
    g_signal_connect(self, "resize", G_CALLBACK(gtk_ray_signal_resize), NULL);
    g_signal_connect(self, "render", G_CALLBACK(gtk_ray_signal_render), NULL);

    // GtkEventController *wheel = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(wheel));
    // g_signal_connect(wheel, "scroll", G_CALLBACK(signal_mouse_wheel), self);

    // GtkEventController *motion = gtk_event_controller_motion_new();
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(motion));
    // g_signal_connect(motion, "motion", G_CALLBACK(signal_mouse_move), self);

    // GtkGesture *gesture = gtk_gesture_click_new();
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(gesture));
    // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    // g_signal_connect(gesture, "pressed", G_CALLBACK(signal_mouse_button_pressed), self);
    // g_signal_connect(gesture, "released", G_CALLBACK(signal_mouse_button_released), self);

    // GtkEventControllerKey *key = gtk_event_controller_key_new();
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(key));
    // g_signal_connect(key, "key-pressed", G_CALLBACK(signal_key_pressed), self);
    // g_signal_connect(key, "key-released", G_CALLBACK(signal_key_released), self);

    // // TODO: separar del emebed
    // target = gtk_drop_target_new(G_TYPE_LIST_STORE, GDK_ACTION_COPY);
    // g_signal_connect(target, "drop", G_CALLBACK(gtk_ray_drop), self);
    // gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(self), gtk_ray_tick, self, NULL);
}

GtkRay *gtk_ray_stage_new(void)
{
    return g_object_new(GTK_TYPE_RAY, NULL);
}

int gtk_ray_get_width(GtkRay *embed)
{
    GtkRayPrivate *priv = gtk_ray_get_instance_private(embed);
    return priv->width;
}

int gtk_ray_get_height(GtkRay *embed)
{
    GtkRayPrivate *priv = gtk_ray_get_instance_private(embed);
    return priv->height;
}

static gboolean signal_mouse_wheel(GtkEventControllerScroll *controller, gdouble dx, gdouble dy, GtkWidget *widget)
{
    // SetMouseWheelMove(0.0f, dy);
    return FALSE;
}

static void signal_mouse_move(GtkEventControllerMotion *controller, double x, double y, GtkWidget *widget)
{
    gtk_widget_set_can_focus(GTK_GL_AREA(widget), true);
    gtk_widget_grab_focus(widget);
    // SetMouse(x, y);
}

static void signal_mouse_button_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget)
{
    gint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    switch (button)
    {
    case 1:
        // SetMouseButton(MOUSE_BUTTON_LEFT, 1);
        break;
    case 2:
        // SetMouseButton(MOUSE_BUTTON_MIDDLE, 1);
        break;
    case 3:
        // SetMouseButton(MOUSE_BUTTON_RIGHT, 1);
        break;
    default:
    }

    gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

static void signal_mouse_button_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget)
{
    gint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));

    switch (button)
    {
    case 1:
        // SetMouseButton(MOUSE_BUTTON_LEFT, 0);
        break;
    case 2:
        // SetMouseButton(MOUSE_BUTTON_MIDDLE, 0);
        break;
    case 3:
        // SetMouseButton(MOUSE_BUTTON_RIGHT, 0);
        break;
    default:
    }

    gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

static void signal_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget)
{
    // keycallback(keycode, keycode, 1, 0);
}

static void signal_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget)
{
    // keycallback(keycode, keycode, 0, 0);
}

