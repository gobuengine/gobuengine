#include "gapp_gobu_embed.h"

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

static guint w_signals[SIGNAL_LAST] = { 0 };

G_DEFINE_TYPE_WITH_PRIVATE(GappGobuEmbed, gapp_gobu_embed, GTK_TYPE_GL_AREA);

static void gapp_gobu_embed_class_init(GappGobuEmbedClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

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
                                          G_TYPE_NONE, 3, G_TYPE_FILE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

static void gapp_gobu_embed_signal_resize(GtkWidget* viewport, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gapp_gobu_embed_signal_resize");
        return;
    }

    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(viewport);
    priv->width = width;
    priv->height = height;

    g_signal_emit(viewport, w_signals[SIGNAL_RESIZE], 0, width, height);
}

static void gapp_gobu_embed_signal_realize(GappGobuEmbed* viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gapp_gobu_embed_signal_realize");
        return;
    }

    // gtk_gl_area_set_has_depth_buffer(viewport, TRUE);
}

static void gapp_gobu_embed_signal_unrealize(GappGobuEmbed* self)
{
    gtk_gl_area_make_current(self);
    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(self);
    gtk_widget_remove_tick_callback(GTK_WIDGET(self), priv->tick);
}

static gboolean gapp_gobu_embed_signal_render(GappGobuEmbed* viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gapp_gobu_embed_signal_render");
        return;
    }

    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(viewport);

    if (!priv->initialize)
    {
        g_signal_emit(viewport, w_signals[SIGNAL_START], 0);
        priv->initialize = TRUE;
    }

    g_signal_emit(viewport, w_signals[SIGNAL_RENDER], 0);

    return TRUE;
}

static gboolean gapp_gobu_embed_drop(GtkDropTarget* target, const GValue* value, double x, double y, gpointer user_data)
{
    if (G_VALUE_HOLDS(value, G_TYPE_FILE))
    {
        g_signal_emit(user_data, w_signals[SIGNAL_DROP], 0, G_FILE(g_value_get_object(value)), x, y);
        return TRUE;
    }
    return FALSE;
}

static gboolean gapp_gobu_embed_tick(GappGobuEmbed* self, GdkFrameClock* frame_clock, gpointer user_data)
{
    gtk_widget_queue_draw(self);
    return G_SOURCE_CONTINUE;
}

static void gapp_gobu_embed_init(GappGobuEmbed* self)
{
    GtkDropTarget* target;

    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(self);
    priv->initialize = false;

    gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 3);
    // gtk_gl_area_set_auto_render(GTK_GL_AREA(self), TRUE);
    gtk_widget_set_focus_on_click(GTK_GL_AREA(self), true);
    gtk_widget_set_focusable(GTK_GL_AREA(self), true);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "realize", G_CALLBACK(gapp_gobu_embed_signal_realize), NULL);
    g_signal_connect(self, "unrealize", G_CALLBACK(gapp_gobu_embed_signal_unrealize), NULL);
    g_signal_connect(self, "resize", G_CALLBACK(gapp_gobu_embed_signal_resize), NULL);
    g_signal_connect(self, "render", G_CALLBACK(gapp_gobu_embed_signal_render), NULL);

    target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    g_signal_connect(target, "drop", G_CALLBACK(gapp_gobu_embed_drop), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(self), gapp_gobu_embed_tick, self, NULL);
}

GappGobuEmbed* gapp_gobu_embed_new(void)
{
    return g_object_new(GAPP_TYPE_GOBU_EMBED, NULL);
}

gboolean gapp_gobu_embed_get_init(GappGobuEmbed* embed)
{
    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->initialize;
}

int gapp_gobu_embed_get_width(GappGobuEmbed* embed)
{
    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->width;
}

int gapp_gobu_embed_get_height(GappGobuEmbed* embed)
{
    GappGobuEmbedPrivate* priv = gapp_gobu_embed_get_instance_private(embed);
    return priv->height;
}

