#include "gapp-browser-view-file-drop.h"

/**
 * @brief Función que maneja el evento de soltar un archivo en la vista.
 *
 * @param target El objeto GtkDropTarget que recibió el evento.
 * @param value El valor del archivo que se soltó.
 * @param x La posición horizontal del cursor en la vista.
 * @param y La posición vertical del cursor en la vista.
 * @param user_data Datos adicionales proporcionados por el usuario.
 * @return gboolean Devuelve TRUE si el evento fue manejado correctamente, FALSE en caso contrario.
 */
gboolean gapp_browser_signal_view_file_drop(GtkDropTarget *target, const GValue *value, double x, double y, GappBrowser *browser)
{
    if (G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
    {
        GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);
        
        GList *list_files = g_value_get_boxed(value);
        for (int i = 0; i < g_list_length(list_files); i++)
        {
            GError *error = NULL;
            GFile *file_src = G_FILE(g_list_nth_data(list_files, i));
            GFile *file_dest = g_file_new_for_path(gb_path_join(private->path_current, gb_path_basename(file_src), NULL));
            gchar *name = gb_path_basename(file_src);
            if (gb_fs_copyc(file_src, file_dest, &error))
            {
                gb_log_success(TF("Copy file: [%s]", name));
            }
            else
            {
                gb_log_error(TF("Copy [%s]", name), error->message);
            }
        }
        return TRUE;
    }
    return FALSE;
}

