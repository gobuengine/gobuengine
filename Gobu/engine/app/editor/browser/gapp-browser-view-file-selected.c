#include "gapp-browser-view-file-selected.h"
#include "gapp-browser-view-file-nav.h"
#include "../level-editor/gapp_level_editor.h"

/**
 * Obtenemos el primero archivo seleccionado
 *
 */
GFileInfo *gapp_browser_view_file_get_first_file_selected(GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    GListModel *list = gtk_multi_selection_get_model(private->selection);
    guint items_n = g_list_model_get_n_items(list);

    for (int i = 0; i < items_n; i++)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(private->selection), i) == FALSE)
            continue;

        return G_FILE_INFO(g_list_model_get_item(list, i));
    }

    return NULL;
}

/**
 * Maneja la señal de selección de archivo en la vista de archivos del explorador de mundos del editor en Gobu.
 *
 * @param self         La vista de archivos que generó la señal de selección.
 * @param position     La posición del archivo seleccionado.
 * @param user_data    Datos de usuario opcionales relacionados con la selección de archivo.
 */
void gapp_browser_view_file_signal_selected(GtkGridView *self, guint position, GappBrowser *browser)
{
    /* OBTENEMOS LA INFO DEL ARCHIVO SELECCIONADO CON DOUBLE-CLIC */
    GFileInfo *file_info = g_list_model_get_item(G_LIST_MODEL(gtk_grid_view_get_model(self)), position);

    GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));
    gchar *filename = g_file_get_path(file);

    /* COMPROBAMOS SI ES UN DIRECTORIO Y ENTRAMOS */
    if (g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY)
    {
        gapp_browser_fn_history_path_forward_clear(browser);
        gapp_browser_fn_open_folder(browser, filename, TRUE);
    }
    else if (g_file_info_get_file_type(file_info) == G_FILE_TYPE_REGULAR)
    {
        if (gb_fs_is_extension(filename, ".level"))
        {
            gapp_level_editor_new(filename);
        }
        else if (gb_fs_is_extension(filename, ".gbscript"))
        {
            // gapp_editor_script_new(filename);
        }
        else if (gb_fs_is_extension(filename, ".asprites"))
        {
            // gbapp_asprites_new(filename);
        }

        gb_log_info(TF("Double Clic file: %s", gb_path_basename(filename)));
    }
    g_free(filename);
}

/**
 * Obtenemos una lista de archivos seleccionados.
 *
 */
GListStore *gapp_browser_view_file_get_file_seleted(GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    GListStore *files = g_list_store_new(G_TYPE_FILE_INFO);

    GListModel *list = gtk_multi_selection_get_model(private->selection);
    guint items_n = g_list_model_get_n_items(list);

    for (int i = 0; i < items_n; i++)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(private->selection), i) == FALSE)
            continue;

        GFileInfo *file_info_selected = G_FILE_INFO(g_list_model_get_item(list, i));
        g_list_store_append(files, file_info_selected);
    }

    return files;
}
