#include "GobuSignalContentBrowserCreateFolder.h"
#include "GobuWidgets.h"

static void GobuSignalContentBrowserCreateFolderResponse(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuBrowserContentPrivate *private = GobuContentBrowserGet(data);
        const gchar *result = GobuWidgetDialogInputGetText(dialog);
        gchar *path = g_build_filename(private->path_current, result, NULL);
        g_file_make_directory_async(g_file_new_for_path(path), G_PRIORITY_LOW, NULL, NULL, NULL);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void GobuSignalContentBrowserCreateFolder(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = GobuWidgetDialogInput(button, "Create New Folder", "New Folder");
    g_signal_connect(dialog, "response", G_CALLBACK(GobuSignalContentBrowserCreateFolderResponse), data);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}
