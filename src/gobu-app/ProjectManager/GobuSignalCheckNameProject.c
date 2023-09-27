#include "GobuSignalCheckNameProject.h"

void GobuSignalCheckNameProject(GtkWidget *entry, GobuProjectManager *ctx)
{
    if (gtk_entry_get_text_length(entry) > 0)
    {
        const gchar *name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
        gchar *path_full = g_build_filename(gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser)), name, NULL);

        gboolean test = g_file_test(g_build_filename(path_full, "game.gobuproject", NULL), G_FILE_TEST_EXISTS);
        if (test == FALSE)
        {
            gtk_widget_add_css_class(ctx->btn_create, "suggested-action");
            gtk_widget_set_sensitive(ctx->btn_create, TRUE);
            g_free(path_full);
            return;
        }

        g_free(path_full);
    }

    gtk_widget_remove_css_class(ctx->btn_create, "suggested-action");
    gtk_widget_set_sensitive(ctx->btn_create, FALSE);
}
