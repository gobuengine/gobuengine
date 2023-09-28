#include "GobuFnContentBrowserOpenFolder.h"

void GobuFnContentBrowserOpenFolder(GobuBrowserContent *browser, const gchar *path, gboolean history_saved)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(browser);

    // HISTORIAL DE PATH
    if (history_saved)
        g_ptr_array_add(private->path_back, g_strdup(private->path_current));

    g_free(private->path_current);
    private->path_current = g_strdup(path);

    GFile *file = g_file_new_for_path(private->path_current);
    gboolean is_home = (g_strcmp0(g_file_get_basename(file), "Content") != 0);
    gtk_directory_list_set_file(private->directory_list, file);
    // g_object_unref(file);

    // EL BOTON DEL NAV (NEXT)RIGHT SE DESHABILITA SIEMPRE QUE CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
    //  EL BOTON DE NAV (PREV)LEFT SE HABILITA CUANDO CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
    //  EL BOTON DE NAV (HOME) SE HABILITA SI NO ESTAMOS EN CONTENT
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), is_home);
}
