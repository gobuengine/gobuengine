#include "GobuSignalContentBrowserViewFileSelected.h"
#include "GobuFnContentBrowserHistoryPath.h"
#include "GobuFnContentBrowserOpenFolder.h"

void GobuSignalContentBrowserViewFileSelected(GtkGridView *self, guint position, gpointer user_data)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(user_data);

    /* OBTENEMOS LA INFO DEL ARCHIVO SELECCIONADO CON DOUBLE-CLIC */
    GFileInfo *file_info = g_list_model_get_item(G_LIST_MODEL(gtk_grid_view_get_model(self)), position);

    /* COMPROBAMOS SI ES UN DIRECTORIO Y ENTRAMOS */
    if (g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY)
    {
        GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

        GobuFnContentBrowserHistoryPathForwardClear(user_data);
        GobuFnContentBrowserOpenFolder(user_data, g_file_get_path(file), TRUE);

        // g_object_unref(file);
    }
    g_object_unref(file_info);
}