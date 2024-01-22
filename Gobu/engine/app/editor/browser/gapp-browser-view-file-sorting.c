#include "gapp-browser-view-file-sorting.h"

/**
 * Ordena los archivos de directorio con las carpetas primero.
 *
 * @param a     El primer archivo a comparar.
 * @param b     El segundo archivo a comparar.
 * @param data  Datos adicionales opcionales.
 * @return      Un entero negativo si a debe ir antes que b, un entero positivo si b debe ir antes que a, o 0 si son iguales.
 */
int gapp_browser_view_file_signal_sorting(GFileInfo *a, GFileInfo *b, gpointer data)
{
    GFileType type_a = g_file_info_get_file_type(a);
    GFileType type_b = g_file_info_get_file_type(b);

    if (type_a == G_FILE_TYPE_DIRECTORY && type_b != G_FILE_TYPE_DIRECTORY)
        return -1;
    else if (type_a != G_FILE_TYPE_DIRECTORY && type_b == G_FILE_TYPE_DIRECTORY)
        return 1;

    return 0;
}

