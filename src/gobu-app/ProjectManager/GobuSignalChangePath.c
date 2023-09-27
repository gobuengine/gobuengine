#include "GobuSignalChangePath.h"

static void GobuSignalResponsePath(GtkDialog *dialog, gint response_id, GtkWidget *widget)
{
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        GFile *files = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        gtk_button_set_label(GTK_BUTTON(widget), g_file_get_path(files));
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void GobuSignalChangePath(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkWidget *self = gtk_file_chooser_dialog_new("Select folder",
                                                  GTK_WINDOW(ctx->window),
                                                  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                  "Cancel", GTK_RESPONSE_CANCEL,
                                                  "Select", GTK_RESPONSE_ACCEPT, NULL);

    g_signal_connect(self, "response", G_CALLBACK(GobuSignalResponsePath), widget);

    gtk_widget_show(self);
    gtk_window_set_modal(GTK_WINDOW(self), TRUE);
}
