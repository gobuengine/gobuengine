#ifndef GOBU_WIDGET_EMBED_STAGE_H
#define GOBU_WIDGET_EMBED_STAGE_H
#include <gtk/gtk.h>
#include "gobu/gobu-gobu.h"

G_BEGIN_DECLS

typedef struct _GobuWidgetEmbedStagePrivate GobuWidgetEmbedStagePrivate;

#define GTK_TYPE_GOBU_EMBED (gobu_widget_embed_get_type())
G_DECLARE_FINAL_TYPE(GobuWidgetEmbedStage, gobu_widget_embed, GTK, GOBU_EMBED, GtkGLArea)

struct _GobuWidgetEmbedStagePrivate
{
    GtkWidget parent_instance;
    gboolean fps_stats;
    // gboolean show_grid;
    guint width;
    guint height;
    // GobuColor bg;
    // private
    gint tick;
    gint64 first_frame_time;
    gboolean renderInit;
};

GobuWidgetEmbedStage *gobu_widget_embed_stage_new(void);
// void gobu_widget_embed_stage_set_color(GobuWidgetEmbedStage *embed, GobuColor color);
guint gobu_widget_embed_stage_get_width(GobuWidgetEmbedStage *embed);
guint gobu_widget_embed_stage_get_height(GobuWidgetEmbedStage *embed);

G_END_DECLS

#endif // GOBU_WIDGET_EMBED_STAGE_H
