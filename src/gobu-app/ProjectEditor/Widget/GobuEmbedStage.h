#ifndef GOBU_EMBED_STAGE_H
#define GOBU_EMBED_STAGE_H
#include <gtk/gtk.h>
#include "gobu/gobu-gobu.h"

G_BEGIN_DECLS

typedef struct _GtkGobuEmbedStagePrivate GtkGobuEmbedStagePrivate;

#define GTK_TYPE_GOBU_EMBED (gtk_gobu_embed_get_type())
G_DECLARE_FINAL_TYPE(GtkGobuEmbedStage, gtk_gobu_embed, GTK, GOBU_EMBED, GtkGLArea)

struct _GtkGobuEmbedStagePrivate
{
    GtkWidget parent_instance;
    gboolean fps_stats;
    gboolean show_grid;
    guint width;
    guint height;
    GobuColor bg;
    // private
    gint tick;
    gint64 first_frame_time;
    gboolean renderInit;
};

GtkGobuEmbedStage *gtk_gobu_embed_stage_new(void);
void gtk_gobu_embed_stage_set_color(GtkGobuEmbedStage *embed, GobuColor color);

G_END_DECLS

#endif // GOBU_EMBED_STAGE_H
