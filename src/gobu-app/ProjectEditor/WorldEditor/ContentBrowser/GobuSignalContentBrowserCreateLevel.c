#include "GobuSignalContentBrowserCreateLevel.h"
#include "gobu_widget.h"

static void GobuSignalContentBrowserCreateLevelResponse(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuBrowserContentPrivate *private = GobuContentBrowserGet(data);
        const gchar *result = GobuWidgetDialogInputGetText(dialog);
        gchar *path = g_build_filename(private->path_current, g_strdup_printf("%s.glevel", result), NULL);

        g_file_create_readwrite(g_file_new_for_path(path), G_FILE_CREATE_NONE, NULL, NULL);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void GobuSignalContentBrowserCreateLevel(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = GobuWidgetDialogInput(button, "Create New Level", "New Level");
    g_signal_connect(dialog, "response", G_CALLBACK(GobuSignalContentBrowserCreateLevelResponse), data);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}