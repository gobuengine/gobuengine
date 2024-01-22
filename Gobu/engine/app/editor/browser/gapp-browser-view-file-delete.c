#include "gapp-browser-view-file-delete.h"
#include "gapp-browser-view-file-selected.h"

/**
 * Maneja la señal de borrar elementos en el explorador de mundos del editor en Gobu.
 *
 */
static void view_file_signal_delete_file_response(GtkWidget *widget, int response, GListStore *files)
{
    if (response == GTK_RESPONSE_OK)
    {
        GError *error = NULL;
        guint items_n = g_list_model_get_n_items(files);

        for (guint i = 0; i < items_n; i++)
        {
            GFileInfo *file_info = G_FILE_INFO(g_list_model_get_item(files, i));
            GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

            if (!g_file_delete(file, NULL, &error) && !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
            {
                // gapp_widget_alert(widget, error->message);
                gb_log_error(TF("Failed to delete [ %s ]", g_file_peek_path(file)), error->message);
                return;
            }

            gb_log_success(TF("File delete [ %s ]", g_file_peek_path(file)));
        }
    }

    gtk_window_destroy(widget);
}

void gapp_browser_view_file_signal_delete_file(GtkWidget *widget, GappBrowser *browser)
{
    gchar *name_file = "\n";

    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    GListStore *files_selected = gapp_browser_view_file_get_file_seleted(browser);
    guint items_n = g_list_model_get_n_items(G_LIST_MODEL(files_selected));
    
    for (int i = 0; i < items_n; i++)
    {
        GFileInfo *file_info = G_FILE_INFO(g_list_model_get_item(G_LIST_MODEL(files_selected), i));
        GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));
        name_file = g_strconcat(name_file, g_file_info_get_name(file_info), "\n", NULL);
    }

    gapp_widget_dialog_confirm_delete(widget, name_file, files_selected, view_file_signal_delete_file_response);
    g_free(name_file);

    gtk_popover_popdown(GTK_POPOVER(private->popover));
}

