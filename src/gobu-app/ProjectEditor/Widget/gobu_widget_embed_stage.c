#include "gobu_widget_embed_stage.h"

struct _GobuWidgetEmbedStage
{
    GtkWidget parent;
};

typedef enum
{
    GOBU_STAGE_SIGNAL_DRAW = 0,
    GOBU_STAGE_SIGNAL_RESIZE,
    GOBU_STAGE_SIGNAL_START,
    GOBU_STAGE_SIGNAL_LAST
} ErmineEmbedSignals_t;

static guint signals[GOBU_STAGE_SIGNAL_LAST] = {0};

G_DEFINE_TYPE_WITH_PRIVATE(GobuWidgetEmbedStage, gobu_widget_embed, GTK_TYPE_GL_AREA);

static gboolean gtk_tick(GobuWidgetEmbedStage *embed, GdkFrameClock *frame_clock, gpointer user_data)
{
    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);

    gint64 frame_time = gdk_frame_clock_get_frame_time(frame_clock);

    if (priv->first_frame_time == 0)
    {
        /* No need for changes on first frame */
        priv->first_frame_time = frame_time;
        return G_SOURCE_CONTINUE;
    }

    gtk_widget_queue_draw(embed);

    return G_SOURCE_CONTINUE;
}

static gboolean gobu_widget_embed_signal_render(GobuWidgetEmbedStage *embed, GdkGLContext *context)
{
    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);

    gtk_gl_area_make_current(embed);
    if (gtk_gl_area_get_error(embed) != NULL)
        return;

    if (!priv->renderInit)
    {
        g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_START], 0, embed);
        priv->renderInit = TRUE;
    }

    // gobu_render_frame_begin(priv->width, priv->height, (GobuColor){0.0f, 0.0f, 0.0f, 1.0f});
    // {
    //     // if (priv->show_grid == TRUE)
    //     // {
    //     //     gobu_shape_draw_checkboard(24, 24, screen_width, screen_height);
    //     // }
    //     gobu_shape_draw_filled_rect((GobuRectangle){100, 100, 50, 50}, (GobuVec2){1.0f, 1.0f}, (GobuVec2){0.5f, 0.5f}, 0, gobu_color_random_rgb(255, 0, 0));
    // }
    // gobu_render_frame_end(priv->width, priv->height);

    g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_DRAW], 0, embed);

    return TRUE;
}

static void gobu_widget_embed_signal_realize(GobuWidgetEmbedStage *embed)
{
    gtk_gl_area_make_current(embed);
    if (gtk_gl_area_get_error(embed) != NULL)
        return;
}

static void gobu_widget_embed_signal_unrealize(GobuWidgetEmbedStage *embed)
{
    gtk_gl_area_make_current(embed);
    if (gtk_gl_area_get_error(embed) != NULL)
        return;

    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);
    gtk_widget_remove_tick_callback(GTK_WIDGET(embed), priv->tick);
}

static void gobu_widget_embed_signal_resize(GobuWidgetEmbedStage *embed, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(embed) != NULL)
        return;

    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);

    priv->width = width;
    priv->height = height;

    g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_RESIZE], 0, embed);
}

//  CLASS PREPARE
static void gobu_widget_embed_class_init(GobuWidgetEmbedStageClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    // ------------------
    // SIGNAL INSTALL
    // ------------------

    signals[GOBU_STAGE_SIGNAL_DRAW] = g_signal_new("gobu-embed-draw", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, 0, G_TYPE_NONE, 1, G_TYPE_POINTER);
    signals[GOBU_STAGE_SIGNAL_RESIZE] = g_signal_new("gobu-embed-resize", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
    signals[GOBU_STAGE_SIGNAL_START] = g_signal_new("gobu-embed-start", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void gobu_widget_embed_init(GobuWidgetEmbedStage *embed)
{
    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);

    priv->fps_stats = FALSE;
    priv->width = 0;
    priv->height = 0;

    gtk_gl_area_set_required_version(GTK_GL_AREA(embed), 3, 3);
    gtk_widget_set_focus_on_click(GTK_WIDGET(embed), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(embed), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(embed), TRUE);

    g_signal_connect(embed, "render", G_CALLBACK(gobu_widget_embed_signal_render), NULL);
    g_signal_connect(embed, "realize", G_CALLBACK(gobu_widget_embed_signal_realize), NULL);
    g_signal_connect(embed, "unrealize", G_CALLBACK(gobu_widget_embed_signal_unrealize), NULL);
    g_signal_connect(embed, "resize", G_CALLBACK(gobu_widget_embed_signal_resize), NULL);

    // Render time-real
    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(embed), gtk_tick, embed, NULL);
}

GobuWidgetEmbedStage *gobu_widget_embed_stage_new(void)
{
    return g_object_new(GTK_TYPE_GOBU_EMBED, NULL);
}

// void gobu_widget_embed_stage_set_color(GobuWidgetEmbedStage *embed, GobuColor color)
// {
//     GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);
//     priv->bg = color;
// }

guint gobu_widget_embed_stage_get_width(GobuWidgetEmbedStage *embed)
{
    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);
    return priv->width;
}

guint gobu_widget_embed_stage_get_height(GobuWidgetEmbedStage *embed)
{
    GobuWidgetEmbedStagePrivate *priv = gobu_widget_embed_get_instance_private(embed);
    return priv->height;
}
