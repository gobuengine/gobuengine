#include "gb_path.h"
#include <glib.h>
#include "gb_fs.h"
#include "config.h"

/**
 * @brief Retorna el contenido de una ruta relativa.
 *
 * Esta función toma una ruta relativa como entrada y devuelve el contenido de ese archivo.
 *
 * @param path La ruta relativa del archivo.
 * @return El contenido del archivo como una cadena de caracteres.
 */
char* gb_path_relative_content(const char* path)
{
    char* npath = strstr(path, FOLDER_CONTENT_PROJECT);
    if (npath != NULL) {
        npath += strlen(FOLDER_CONTENT_PROJECT);
    }
    else {
        npath = gb_strdup(path);
    }

    return npath;
}

/**
 * @brief Une el contenido de una ruta relativa al path actual.
 *
 * @param path La ruta relativa a unir.
 * @return Un puntero al contenido de la ruta relativa unida al path actual.
 */
char* gb_path_join_relative_content(const char* path)
{
    return gb_path_normalize(gb_path_join(gb_project_get_path(), "Content", path, NULL));
}

/**
 * @brief Normaliza una ruta de archivo.
 *
 * Esta función toma una ruta de archivo y la normaliza, eliminando cualquier
 * caracter especial o redundante. Devuelve un puntero a la ruta de archivo
 * normalizada.
 *
 * @param path La ruta de archivo a normalizar.
 * @return Un puntero a la ruta de archivo normalizada.
 */
char* gb_path_normalize(const char* path)
{
    return gb_str_replace(path, "\\", "/");
}

/**
 * @brief Combina múltiples rutas en una sola ruta.
 *
 * Esta función toma una ruta inicial y una lista variable de rutas adicionales y las combina en una sola ruta.
 *
 * @param first_path La ruta inicial.
 * @param ... Las rutas adicionales a combinar.
 *
 * @return La ruta combinada.
 */
char* gb_path_join(const char* first_path, ...)
{
    gchar* buffer;
    va_list args;

    va_start(args, first_path);
    buffer = g_build_filename_valist(first_path, &args);
    va_end(args);

    return buffer;
}

/**
 * @brief Obtiene el nombre base de una ruta de archivo.
 *
 * Esta función toma una ruta de archivo como entrada y devuelve el nombre base de la ruta.
 * El nombre base es el último componente de la ruta, sin la extensión del archivo.
 *
 * @param filename La ruta de archivo de la cual se desea obtener el nombre base.
 * @return El nombre base de la ruta de archivo.
 */
char* gb_path_basename(const char* filename)
{
    return g_path_get_basename(filename);
}

/**
 * @brief Retorna el directorio de la ruta de un archivo.
 *
 * Esta función toma una cadena de caracteres que representa la ruta de un archivo y devuelve
 * el directorio de esa ruta. El directorio se determina eliminando el nombre del archivo de la ruta.
 *
 * @param filename La ruta del archivo.
 * @return El directorio de la ruta del archivo.
 */
char* gb_path_dirname(const char* filename)
{
    return g_path_get_dirname(filename);
}

/**
 * Retorna la ruta del directorio actual.
 *
 * @return Un puntero a una cadena que representa la ruta del directorio actual.
 */
char* gb_path_current_dir(void)
{
    return g_get_current_dir();
}

/**
 * @brief Verifica si existe un folder en la ruta especificada.
 *
 * @param filename El nombre del folder a verificar.
 * @return true si el folder existe, false en caso contrario.
 */
bool gb_path_exist(const char* filename)
{
    return gb_fs_test(filename, G_FS_TEST_IS_DIR);
}

