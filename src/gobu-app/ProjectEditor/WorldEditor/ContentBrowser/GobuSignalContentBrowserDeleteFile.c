#include "GobuSignalContentBrowserDeleteFile.h"
#include "gobu_widget.h"

static void GobuSignalContentBrowserDeleteFileResponse(GtkWidget *widget, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GError *error = NULL;
        GFile *file = G_FILE(data);
        if (!g_file_delete(file, NULL, &error) && !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        {
            GobuWidgetAlert(widget, error->message);
            g_warning("Failed to delete %s: %s [%d]\n", g_file_peek_path(file), error->message, error->code);
            return;
        }
    }

    gtk_window_destroy(widget);
}

void GobuSignalContentBrowserDeleteFile(GtkWidget *widget, gpointer data)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(data);

    GFileInfo *file_selected = G_FILE_INFO(gtk_single_selection_get_selected_item(private->selection));
    GFile *file = G_FILE(g_file_info_get_attribute_object(file_selected, "standard::file"));

    GobuWidgetDialogConfirmDelete(widget, file, GobuSignalContentBrowserDeleteFileResponse);
    gtk_popover_popdown(GTK_POPOVER(private->popover));
}
