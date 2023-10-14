#include "gobu_gobu_embed.h"

enum
{
    SIGNAL_START = 0,
    SIGNAL_UPDATE,
    SIGNAL_RENDER,
    SIGNAL_DROP,
    SIGNAL_LAST
};

struct _GobuGobuEmbed
{
    GtkWidget parent;
};

static guint w_signals[SIGNAL_LAST] = {0};

G_DEFINE_TYPE_WITH_PRIVATE(GobuGobuEmbed, gobu_gobu_embed, GTK_TYPE_GL_AREA);

static void gobu_gobu_embed_class_init(GobuGobuEmbedClass *klass)
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

    w_signals[SIGNAL_UPDATE] = g_signal_new("gobu-embed-update",
                                            G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                                            NULL, NULL, NULL,
                                            0,
                                            G_TYPE_NONE, 0);

    w_signals[SIGNAL_DROP] = g_signal_new("gobu-embed-drop",
                                          G_TYPE_FROM_CLASS(klass),
                                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                          NULL, NULL, NULL,
                                          0,
                                          G_TYPE_NONE, 3, G_TYPE_FILE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

static void gobu_gobu_embed_signal_resize(GtkWidget *viewport, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gobu_gobu_embed_signal_resize");
        return;
    }

    GobuGobuEmbedPrivate *priv = gobu_gobu_embed_get_instance_private(viewport);

    priv->width = width;
    priv->height = height;

    // SetWindowSize(priv->width, priv->height);
}

static void gobu_gobu_embed_signal_realize(GobuGobuEmbed *viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gobu_gobu_embed_signal_realize");
        return;
    }

    // gtk_gl_area_set_has_depth_buffer(viewport, TRUE);
}

static void gobu_gobu_embed_signal_unrealize(GobuGobuEmbed *self)
{
    gtk_gl_area_make_current(self);
    if (gtk_gl_area_get_error(self) != NULL)
    {
        g_error("gobu_gobu_embed_signal_unrealize");
        return;
    }

    GobuGobuEmbedPrivate *priv = gobu_gobu_embed_get_instance_private(self);
    gtk_widget_remove_tick_callback(GTK_WIDGET(self), priv->tick);
}

static gboolean gobu_gobu_embed_signal_render(GobuGobuEmbed *viewport, gpointer data)
{
    gtk_gl_area_make_current(viewport);
    if (gtk_gl_area_get_error(viewport) != NULL)
    {
        g_error("gobu_gobu_embed_signal_render");
        return;
    }

    GobuGobuEmbedPrivate *priv = gobu_gobu_embed_get_instance_private(viewport);

    if (!priv->initialize)
    {
        bugo_init(priv->width, priv->height);
        g_signal_emit(viewport, w_signals[SIGNAL_START], 0);
        priv->initialize = TRUE;
    }

    g_signal_emit(viewport, w_signals[SIGNAL_UPDATE], 0);

    bugo_gfx_render_begin(priv->width, priv->height);
    {
        bugo_gfx_render_clear_color(priv->background_color);
        g_signal_emit(viewport, w_signals[SIGNAL_RENDER], 0);
    }
    bugo_gfx_render_end(priv->width, priv->height);

    return TRUE;
}

static gboolean gobu_gobu_embed_drop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer user_data)
{
    if (G_VALUE_HOLDS(value, G_TYPE_FILE))
    {
        g_signal_emit(user_data, w_signals[SIGNAL_DROP], 0, G_FILE(g_value_get_object(value)), x, y);
        return TRUE;
    }
    return FALSE;
}

static gboolean gobu_gobu_embed_tick(GobuGobuEmbed *self, GdkFrameClock *frame_clock, gpointer user_data)
{
    gtk_widget_queue_draw(self);
    return G_SOURCE_CONTINUE;
}

static void gobu_gobu_embed_init(GobuGobuEmbed *self)
{
    GtkWidget *box;
    GtkDropTarget *target;

    GobuGobuEmbedPrivate *priv = gobu_gobu_embed_get_instance_private(self);
    priv->width = 1;
    priv->height = 1;
    priv->initialize = false;
    priv->background_color = (Color){38, 38, 38, 255};

    // gtk_gl_area_set_required_version(GTK_GL_AREA(self), 3, 3);
    // gtk_gl_area_set_auto_render(GTK_GL_AREA(self), TRUE);
    gtk_widget_set_focus_on_click(GTK_WIDGET(self), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(self), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(self), TRUE);

    g_signal_connect(self, "realize", G_CALLBACK(gobu_gobu_embed_signal_realize), NULL);
    g_signal_connect(self, "unrealize", G_CALLBACK(gobu_gobu_embed_signal_unrealize), NULL);
    g_signal_connect(self, "resize", G_CALLBACK(gobu_gobu_embed_signal_resize), NULL);
    g_signal_connect(self, "render", G_CALLBACK(gobu_gobu_embed_signal_render), NULL);

    target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    g_signal_connect(target, "drop", G_CALLBACK(gobu_gobu_embed_drop), self);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(target));

    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(self), gobu_gobu_embed_tick, self, NULL);
}

GobuGobuEmbed *gobu_gobu_embed_new(void)
{
    return g_object_new(GOBU_TYPE_GOBU_EMBED, NULL);
}

void gobu_gobu_embed_set_background_color(GobuGobuEmbed *embed, Color bg)
{
    GobuGobuEmbedPrivate *priv = gobu_gobu_embed_get_instance_private(embed);
    priv->background_color = bg;
}
