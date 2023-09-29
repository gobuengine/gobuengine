#include "GobuEmbedStage.h"

struct _GtkGobuEmbedStage
{
    GtkWidget parent;
};

typedef enum
{
    GOBU_STAGE_SIGNAL_RENDER = 0,
    GOBU_STAGE_SIGNAL_UPDATE,
    GOBU_STAGE_SIGNAL_RESIZE,
    GOBU_STAGE_SIGNAL_START,
    GOBU_STAGE_SIGNAL_LAST
} ErmineEmbedSignals_t;

static guint signals[GOBU_STAGE_SIGNAL_LAST] = {0};

G_DEFINE_TYPE_WITH_PRIVATE(GtkGobuEmbedStage, gtk_gobu_embed, GTK_TYPE_GL_AREA);

static gboolean gtk_tick(GtkGobuEmbedStage *embed, GdkFrameClock *frame_clock, gpointer user_data)
{
    GdkFrameTimings *previous_timings;
    gint64 previous_frame_time;
    gint64 frame_time;
    gint64 history_start, history_len;
    gint64 frame;
    char *s;

    GtkGobuEmbedStagePrivate *priv = gtk_gobu_embed_get_instance_private(embed);

    frame = gdk_frame_clock_get_frame_counter(frame_clock);
    frame_time = gdk_frame_clock_get_frame_time(frame_clock);

    if (priv->first_frame_time == 0)
    {
        /* No need for changes on first frame */
        priv->first_frame_time = frame_time;
        // if (priv->fps_label)
        //     gtk_label_set_label(priv->fps_label, "FPS: ---");
        return G_SOURCE_CONTINUE;
    }

    gtk_widget_queue_draw(embed);

    history_start = gdk_frame_clock_get_history_start(frame_clock);

    // if (priv->fps_label && frame % 60 == 0)
    if (frame % 60 == 0)
    {
        history_len = frame - history_start;
        if (history_len > 0)
        {
            previous_timings = gdk_frame_clock_get_timings(frame_clock, frame - history_len);
            previous_frame_time = gdk_frame_timings_get_frame_time(previous_timings);

            // s = g_strdup_printf("FPS: %-4.1f", (G_USEC_PER_SEC * history_len) / (double)(frame_time - previous_frame_time));
            // gtk_label_set_label(priv->fps_label, s);
            // g_free(s);
        }
    }

    return G_SOURCE_CONTINUE;
}

static gboolean on_render(GtkGobuEmbedStage *embed, GdkGLContext *context)
{
    GtkGobuEmbedStagePrivate *priv = gtk_gobu_embed_get_instance_private(embed);

    if (!priv->renderInit)
    {
        GobuRenderInit();
        priv->renderInit = TRUE;
    }

    g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_UPDATE], 0, embed);

    GobuRenderFrameBegin(priv->width, priv->height, priv->bg);
    {
        if (priv->show_grid)
        {
            GobuShapeDrawCheckboard(24, 24, priv->width, priv->height);
        }

        g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_RENDER], 0, embed);
    }
    GobuRenderFrameEnd(priv->width, priv->height);

    return TRUE;
}

static void on_realize(GtkGobuEmbedStage *embed)
{
    gtk_gl_area_make_current(embed);
    if (gtk_gl_area_get_error(embed) != NULL)
        return;

    g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_START], 0, embed);
}

static void on_unrealize(GtkGobuEmbedStage *embed)
{
    gtk_gl_area_make_current(embed);
    if (gtk_gl_area_get_error(embed) != NULL)
        return;

    GtkGobuEmbedStagePrivate *priv = gtk_gobu_embed_get_instance_private(embed);
    gtk_widget_remove_tick_callback(GTK_WIDGET(embed), priv->tick);

    // ermine_gfx_shutdown(priv->ctx);
}

static void on_resize(GtkGobuEmbedStage *embed, gint width, gint height, gpointer data)
{
    if (gtk_gl_area_get_error(embed) != NULL)
        return;

    GtkGobuEmbedStagePrivate *priv = gtk_gobu_embed_get_instance_private(embed);

    priv->width = width;
    priv->height = height;

    g_signal_emit(embed, signals[GOBU_STAGE_SIGNAL_RESIZE], 0, embed);
}

//  CLASS PREPARE
static void gtk_gobu_embed_class_init(GtkGobuEmbedStageClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    // ------------------
    // SIGNAL INSTALL
    // ------------------

    signals[GOBU_STAGE_SIGNAL_RENDER] = g_signal_new("stage-draw", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[GOBU_STAGE_SIGNAL_UPDATE] = g_signal_new("stage-update", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[GOBU_STAGE_SIGNAL_RESIZE] = g_signal_new("stage-resize", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[GOBU_STAGE_SIGNAL_START] = g_signal_new("stage-start", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS, NULL, NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void gtk_gobu_embed_init(GtkGobuEmbedStage *embed)
{
    GtkGobuEmbedStagePrivate *priv = gtk_gobu_embed_get_instance_private(embed);
    priv->fps_stats = FALSE;
    priv->show_grid = TRUE;
    priv->width = 0;
    priv->height = 0;

    // Render time-real
    priv->tick = gtk_widget_add_tick_callback(GTK_WIDGET(embed), gtk_tick, embed, NULL);
}

GtkGobuEmbedStage *gtk_gobu_embed_stage_new(void)
{
    GtkGobuEmbedStage *embed = g_object_new(GTK_TYPE_GOBU_EMBED, NULL);
    gtk_gl_area_set_required_version(GTK_GL_AREA(embed), 3, 3);
    gtk_widget_set_focus_on_click(GTK_WIDGET(embed), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(embed), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(embed), TRUE);

    g_signal_connect(embed, "render", G_CALLBACK(on_render), NULL);
    g_signal_connect(embed, "realize", G_CALLBACK(on_realize), NULL);
    g_signal_connect(embed, "unrealize", G_CALLBACK(on_unrealize), NULL);
    g_signal_connect(embed, "resize", G_CALLBACK(on_resize), NULL);

    return embed;
}

void gtk_gobu_embed_stage_set_color(GtkGobuEmbedStage *embed, GobuColor color)
{
    GtkGobuEmbedStagePrivate *priv = gtk_gobu_embed_get_instance_private(embed);
    priv->bg = color;
}