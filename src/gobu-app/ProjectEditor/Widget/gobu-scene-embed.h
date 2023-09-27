#ifndef GOBU_SCENE_EMBED_H
#define GOBU_SCENE_EMBED_H
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkGobuEmbedPrivate GtkGobuEmbedPrivate;

#define GTK_TYPE_GOBU_EMBED (gtk_gobu_embed_get_type())
G_DECLARE_FINAL_TYPE(GtkGobuEmbed, gtk_gobu_embed, GTK, GOBU_EMBED, GtkGLArea)

struct _GtkGobuEmbedPrivate
{
    GtkWidget parent_instance;
    gboolean fps_stats;
    gboolean show_grid;
    guint width;
    guint height;
    // eColor bg;
    // private
    gint tick;
    gint64 first_frame_time;
    gboolean renderInit;
};

GtkGobuEmbed *gtk_gobu_embed_new (void);
// void gtk_gobu_embed_set_color(GtkGobuEmbed *embed, eColor color);

G_END_DECLS

#endif // GOBU_SCENE_EMBED_H
