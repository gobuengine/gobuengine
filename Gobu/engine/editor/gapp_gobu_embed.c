#include "gapp_gobu_embed.h"
#include "gobu.h"

enum
{
    SIGNAL_START = 0,
    SIGNAL_RESIZE,
    SIGNAL_RENDER,
    SIGNAL_DROP,
    SIGNAL_LAST
};

struct _GappGobuEmbed
{
    GtkWidget parent;
};

static guint w_signals[SIGNAL_LAST] = {0};

G_DEFINE_TYPE_WITH_PRIVATE(GappGobuEmbed, gapp_gobu_embed, GTK_TYPE_GL_AREA);

static void signal_embed_resize(GappGobuEmbed *viewport, int width, int height, gpointer data);
static gboolean signal_embed_mouse_wheel(GtkEventControllerScroll *controller, gdouble dx, gdouble dy, GtkWidget *widget);
static void signal_embed_mouse_move(GtkEventControllerMotion *controller, double x, double y, GtkWidget *widget);
static void signal_embed_mouse_button_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget);
static void signal_embed_mouse_button_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget);
static void signal_embed_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget);
static void signal_embed_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget);

static void gapp_gobu_embed_class_init(GappGobuEmbedClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    w_signals[SIGNAL_START] = g_signal_new("gobu-embed-start",
                                           G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                           0,
                                           NULL, NULL, NULL,
                                           G_TYPE_NONE, 0);

    w_signals[SIGNAL_RENDER] = g_signal_new("gobu-embed-render",
                                            G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                                            0,
                                            NULL, NULL, NULL,
                                            G_TYPE_NONE, 0);

    w_signals[SIGNAL_RESIZE] = g_signal_new("gobu-embed-resize",
                                            G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                            NULL, NULL, NULL,
                                            0,
                                            G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

    w_signals[SIGNAL_DROP] = g_signal_new("gobu-embed-drop",
                                          G_TYPE_FROM_CLASS(klass),
                                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                          NULL, NULL, NULL,
                                          0,
                                          G_TYPE_NONE, 3, G_TYPE_LIST_STORE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

static void gapp_gobu_embed_signal_focus(GtkWidget *viewport, gpointer data)
{
    gtk_widget_set_can_focus(GTK_GL_AREA(viewport), true);
    gtk_widget_grab_focus(viewport);

    printf("focus\n");
}

static void gapp_gobu_embed_signal_resize(GtkWidget *viewport, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gapp_gobu_embed_signal_resize");
        return;
    }

    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(viewport);
    priv->width = width;
    priv->height = height;

    ViewportSizeCallback(width, height);

    g_signal_emit(viewport, w_signals[SIGNAL_RESIZE], 0, width, height);
}

static void gapp_gobu_embed_signal_realize(GappGobuEmbed *viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gapp_gobu_embed_signal_realize");
        return;
    }

    // gtk_gl_area_set_has_depth_buffer(viewport, TRUE);
}

static void gapp_gobu_embed_signal_unrealize(GappGobuEmbed *self)
{
    gtk_gl_area_make_current(self);
    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(self);
    gtk_widget_remove_tick_callback(GTK_WIDGET(self), priv->tick);
}

static gboolean gapp_gobu_embed_signal_render(GappGobuEmbed *viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gapp_gobu_embed_signal_render");
        return;
    }

    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(viewport);

    if (!priv->initialize)
    {
        priv->world = gb_init(&(gb_app_t){.width = priv->width, .height = priv->height, .show_grid = priv->show_grid});
        g_signal_emit(viewport, w_signals[SIGNAL_START], 0);
        priv->initialize = TRUE;
    }

    // TODO: Temporal hasta que se implemente el renderizado por contexto
    ViewportSizeCallback(priv->width, priv->height);

    // g_signal_emit(viewport, w_signals[SIGNAL_RENDER], 0);
    gb_app_progress(priv->world);

    return TRUE;
}

static gboolean gapp_gobu_embed_drop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer user_data)
{
    if (G_VALUE_HOLDS(value, G_TYPE_LIST_STORE))
    {
        g_signal_emit(user_data, w_signals[SIGNAL_DROP], 0, G_LIST_STORE(g_value_get_object(value)), x, y);
        return TRUE;
    }
    return FALSE;
}

static gboolean gapp_gobu_embed_tick(GappGobuEmbed *self, GdkFrameClock *frame_clock, gpointer user_data)
{
    gtk_widget_queue_draw(self);
    return G_SOURCE_CONTINUE;
}

static void gapp_gobu_embed_init(GappGobuEmbed *self)
{
    GtkDropTarget *target;

    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(self);
    priv->initialize = false;

    gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 3);
    // gtk_gl_area_set_auto_render(GTK_GL_AREA(self), TRUE);
    // gtk_gl_area_attach_buffers(GTK_GL_AREA(self));
    gtk_widget_set_focus_on_click(GTK_GL_AREA(self), true);
    gtk_widget_set_focusable(GTK_GL_AREA(self), true);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "realize", G_CALLBACK(gapp_gobu_embed_signal_realize), NULL);
    g_signal_connect(self, "unrealize", G_CALLBACK(gapp_gobu_embed_signal_unrealize), NULL);
    g_signal_connect(self, "resize", G_CALLBACK(gapp_gobu_embed_signal_resize), NULL);
    g_signal_connect(self, "render", G_CALLBACK(gapp_gobu_embed_signal_render), NULL);

    GtkEventController *wheel = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(wheel));
    g_signal_connect(wheel, "scroll", G_CALLBACK(signal_embed_mouse_wheel), self);

    GtkEventController *motion = gtk_event_controller_motion_new();
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(motion));
    g_signal_connect(motion, "motion", G_CALLBACK(signal_embed_mouse_move), self);

    GtkGesture *gesture = gtk_gesture_click_new();
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(gesture));
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    g_signal_connect(gesture, "pressed", G_CALLBACK(signal_embed_mouse_button_pressed), self);
    g_signal_connect(gesture, "released", G_CALLBACK(signal_embed_mouse_button_released), self);

    GtkEventControllerKey *key = gtk_event_controller_key_new();
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(key));
    g_signal_connect(key, "key-pressed", G_CALLBACK(signal_embed_key_pressed), self);
    g_signal_connect(key, "key-released", G_CALLBACK(signal_embed_key_released), self);

    // TODO: separar del emebed
    target = gtk_drop_target_new(G_TYPE_LIST_STORE, GDK_ACTION_COPY);
    g_signal_connect(target, "drop", G_CALLBACK(gapp_gobu_embed_drop), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(self), gapp_gobu_embed_tick, self, NULL);
}

GappGobuEmbed *gapp_gobu_embed_new(void)
{
    return g_object_new(GAPP_TYPE_GOBU_EMBED, NULL);
}

gboolean gapp_gobu_embed_get_init(GappGobuEmbed *embed)
{
    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->initialize;
}

int gapp_gobu_embed_get_width(GappGobuEmbed *embed)
{
    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->width;
}

int gapp_gobu_embed_get_height(GappGobuEmbed *embed)
{
    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->height;
}

ecs_world_t *gapp_gobu_embed_get_world(GappGobuEmbed *embed)
{
    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->world;
}

void gapp_gobu_embed_set_grid(GappGobuEmbed *embed, gboolean show)
{
    GappGobuEmbedPrivate *priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->show_grid = show;
}

/**
 * @brief Función que maneja la señal de zoom en el viewport.
 *
 * Esta función se encarga de manejar la señal de zoom en el viewport de la aplicación.
 *
 * @param controller El controlador de eventos de desplazamiento.
 * @param dx La cantidad de desplazamiento horizontal.
 * @param dy La cantidad de desplazamiento vertical.
 * @param widget El widget del viewport.
 * @return gboolean Devuelve TRUE si el evento fue manejado correctamente, FALSE en caso contrario.
 */
static gboolean signal_embed_mouse_wheel(GtkEventControllerScroll *controller, gdouble dx, gdouble dy, GtkWidget *widget)
{
    SetMouseWheelMove(0.0f, dy);
    return FALSE;
}

/**
 * @brief Maneja el evento de movimiento del mouse en el viewport.
 *
 * @param controller El controlador del evento de movimiento del mouse.
 * @param x La coordenada x del mouse.
 * @param y La coordenada y del mouse.
 * @param widget El widget en el que se produce el evento.
 */
static void signal_embed_mouse_move(GtkEventControllerMotion *controller, double x, double y, GtkWidget *widget)
{
    gtk_widget_set_can_focus(GTK_GL_AREA(widget), true);
    gtk_widget_grab_focus(widget);
    SetMouse(x, y);
}

/**
 * @brief Función que se ejecuta cuando se presiona un botón del mouse en el viewport.
 *
 * @param gesture El gesto de clic asociado al evento.
 * @param n_press El número de veces que se ha presionado el botón del mouse.
 * @param x La coordenada X del punto donde se ha presionado el botón del mouse.
 * @param y La coordenada Y del punto donde se ha presionado el botón del mouse.
 * @param widget El widget en el que se ha producido el evento.
 */
static void signal_embed_mouse_button_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget)
{
    gint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    switch (button)
    {
    case 1:
        SetMouseButton(MOUSE_BUTTON_LEFT, 1);
        break;
    case 2:
        SetMouseButton(MOUSE_BUTTON_MIDDLE, 1);
        break;
    case 3:
        SetMouseButton(MOUSE_BUTTON_RIGHT, 1);
        break;
    default:
    }

    gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

/**
 * @brief Función que se ejecuta cuando se suelta un botón del mouse en el viewport.
 *
 * @param gesture El gesto de clic que se activó.
 * @param n_press El número de veces que se presionó el botón del mouse.
 * @param x La coordenada x del punto donde se soltó el botón del mouse.
 * @param y La coordenada y del punto donde se soltó el botón del mouse.
 * @param widget El widget en el que se soltó el botón del mouse.
 */
static void signal_embed_mouse_button_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *widget)
{
    gint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));

    switch (button)
    {
    case 1:
        SetMouseButton(MOUSE_BUTTON_LEFT, 0);
        break;
    case 2:
        SetMouseButton(MOUSE_BUTTON_MIDDLE, 0);
        break;
    case 3:
        SetMouseButton(MOUSE_BUTTON_RIGHT, 0);
        break;
    default:
    }

    gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

/**
 * @brief Función que se ejecuta cuando se presiona una tecla en el viewport.
 *
 * @param self El controlador de eventos de teclado.
 * @param keyval El valor de la tecla presionada.
 * @param keycode El código de la tecla presionada.
 * @param state El estado de los modificadores de teclado.
 * @param widget El widget en el que se presionó la tecla.
 */
static void signal_embed_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget)
{
    keycallback(keycode, keycode, 1, 0);
}

/**
 * @brief Función que se ejecuta cuando se suelta una tecla en el viewport.
 *
 * @param self El controlador de eventos de teclado.
 * @param keyval El valor de la tecla que se soltó.
 * @param keycode El código de la tecla que se soltó.
 * @param state El estado de los modificadores de teclado.
 * @param widget El widget en el que se soltó la tecla.
 */
static void signal_embed_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget *widget)
{
    keycallback(keycode, keycode, 0, 0);
}
