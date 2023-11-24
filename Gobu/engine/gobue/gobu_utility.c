#include "gobu_utility.h"


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

/**
 * @brief Escribe el contenido en un archivo.
 *
 * Esta función escribe el contenido proporcionado en un archivo con el nombre especificado.
 *
 * @param filename El nombre del archivo en el que se va a escribir.
 * @param contents El contenido que se va a escribir en el archivo.
 * @return true si se pudo escribir el archivo correctamente, false en caso contrario.
 */
bool gb_fs_write_file(const char* filename, const char* contents)
{
    return g_file_set_contents(filename, contents, strlen(contents), NULL);
}

/**
 * @brief Crea un directorio con la ruta especificada.
 *
 * @param pathname La ruta del directorio a crear.
 * @return true si el directorio se creó correctamente, false en caso contrario.
 */
bool gb_fs_mkdir(const char* pathname)
{
    return g_mkdir_with_parents(pathname, 0) == -1 ? false : true;
}

/**
 * @brief Verifica si un archivo existe en el sistema de archivos.
 *
 * Esta función comprueba si un archivo con el nombre especificado existe en el sistema de archivos.
 *
 * @param filename El nombre del archivo a verificar.
 * @return true si el archivo existe, false en caso contrario.
 */
bool gb_fs_file_exist(const char* filename)
{
    return gb_fs_test(filename, G_FS_TEST_EXISTS);
}

/**
 * @brief Prueba el sistema de archivos para el nombre de archivo dado utilizando la prueba especificada.
 *
 * @param filename El nombre del archivo a probar.
 * @param test El tipo de prueba a realizar en el sistema de archivos.
 * @return true si la prueba es exitosa, false en caso contrario.
 */
bool gb_fs_test(const char* filename, GFSTest test)
{
    return g_file_test(filename, (GFileTest)test);
}

/**
 * @brief Obtiene la extensión de un nombre de archivo.
 *
 * Esta función toma un nombre de archivo como entrada y devuelve la extensión del archivo.
 *
 * @param filename El nombre del archivo del cual se desea obtener la extensión.
 * @return La extensión del archivo.
 */
const char* gb_fs_extname(const char* filename)
{
    if (filename[0] == '.') return filename;
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    return dot;
}

/**
 * @brief Obtiene el nombre de un archivo a partir de su ruta.
 *
 * Esta función toma una ruta de archivo y devuelve el nombre del archivo sin la extensión.
 *
 * @param filename La ruta del archivo.
 * @param with_ext Indica si se debe incluir la extensión en el nombre del archivo.
 * @return El nombre del archivo sin la extensión, si with_ext es false.
 *         Si with_ext es true, devuelve el nombre del archivo con la extensión.
 */
char* gb_fs_get_name(const char* filename, bool with_ext)
{
    gchar* basename = gb_path_basename(filename);
    if (with_ext) {
        gchar** sep_name = gb_str_split(basename, ".");
        gchar* name = gb_strdup(sep_name[0]);
        gb_str_split_free(sep_name);
        return name;
    }
    return basename;
}

/**
 * @brief Verifica si la extensión de un archivo coincide con una extensión dada.
 *
 * @param filename El nombre del archivo.
 * @param ext La extensión a comparar.
 * @return true si la extensión del archivo coincide con la extensión dada, false en caso contrario.
 */

bool gb_fs_is_extension(const char* filename, const char* ext)
{
    return (g_strcmp0(gb_fs_extname(filename), ext) == 0);
}

/**
 * @brief Obtiene el contenido de un archivo.
 *
 * Esta función lee el contenido de un archivo y lo devuelve como una cadena de caracteres.
 *
 * @param filename El nombre del archivo a leer.
 * @param length La longitud máxima del contenido a leer.
 * @return Un puntero a la cadena de caracteres que contiene el contenido del archivo.
 */
gchar* gb_fs_get_contents(const char* filename, gsize* length)
{
    GError* error = NULL;
    gchar* contents = NULL;
    *length = 0;

    if (g_file_get_contents(filename, &contents, length, &error) == FALSE)
    {
        g_error("Error reading file: %s", error->message);
        g_error_free(error);
    }
    return contents;
}

/**
 * @brief Duplica una cadena de texto con formato opcional.
 *
 * Esta función duplica una cadena de texto con formato opcional. Toma una cadena de formato y argumentos adicionales,
 * similares a la función printf, y devuelve una cadena recién asignada que es una duplicación de la cadena formateada.
 *
 * @param format La cadena de formato.
 * @param ... Argumentos adicionales para el formato.
 * @return Una cadena recién asignada que es una duplicación de la cadena formateada.
 */
char* gb_strdups(const char* format, ...)
{
    gchar* buffer;
    va_list args;

    va_start(args, format);
    buffer = g_strdup_vprintf(format, args);
    va_end(args);

    return buffer;
}

/**
 * @brief Duplica una cadena de texto.
 *
 * Esta función crea una copia de la cadena de texto de entrada y devuelve un puntero a la memoria recién asignada.
 *
 * @param str La cadena de texto a duplicar.
 * @return Un puntero a la cadena de texto duplicada, o NULL si falla la asignación de memoria.
 */
char* gb_strdup(const char* str)
{
    return g_strdup(str);
}

/**
 * Convierte una cadena de texto a minúsculas.
 *
 * @param text La cadena de texto a convertir.
 * @return La cadena de texto convertida en minúsculas.
 */
char* gb_str_tolower(const char* text)
{
    return g_utf8_strdown(text, strlen(text));
}

/**
 * Convierte una cadena a mayúsculas.
 *
 * @param text La cadena que se va a convertir.
 * @return La cadena convertida en mayúsculas.
 */
char* gb_str_uppercase(const char* text)
{
    return g_utf8_strup(text, strlen(text));
}

/**
 * @brief Compara dos cadenas de texto y devuelve verdadero si son iguales.
 *
 * @param text1 La primera cadena de texto a comparar.
 * @param text2 La segunda cadena de texto a comparar.
 * @return true si las cadenas de texto son iguales, false en caso contrario.
 */
bool gb_str_isequal(const char* text1, const char* text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0) result = true;
    }

    return result;
}

/**
 * Divide una cadena en un array de subcadenas basado en un delimitador.
 *
 * @param string La cadena a dividir.
 * @param delimiter El delimitador utilizado para dividir la cadena.
 * @return Un array de subcadenas.
 */
char** gb_str_split(const char* string, const char* delimiter)
{
    return g_strsplit(string, delimiter, -1);
}

/**
 * @brief Libera la memoria asignada a un array de cadenas de caracteres.
 *
 * @param str_array El array de cadenas de caracteres a liberar.
 */
void gb_str_split_free(char** str_array)
{
    g_strfreev(str_array);
}

/**
 * @brief Elimina los espacios en blanco al principio y al final de una cadena de caracteres.
 *
 * @param str La cadena de caracteres a recortar.
 * @return Un puntero a la cadena de caracteres recortada.
 */
char* gb_str_trim(char* str)
{
    return g_strstrip(str);
}

/**
 * @brief Elimina los espacios en blanco de una cadena de caracteres.
 *
 * @param str La cadena de caracteres de la cual se eliminarán los espacios en blanco.
 * @return Un puntero a la cadena de caracteres resultante sin espacios en blanco.
 */
char* gb_str_remove_spaces(char* str)
{
    return gb_str_replace(g_strdup(str), " ", "");
}

/**
 * @brief Sanitiza una cadena eliminando cualquier carácter especial o símbolo.
 *
 * @param str La cadena que se va a sanitizar.
 * @return Un puntero a la cadena sanitizada.
 */
char* gb_str_sanitize(char* str)
{
    return gb_str_replace(g_strdup(str), G_STR_DELIMITERS, "");
}

/**
 * Reemplaza todas las apariciones de una subcadena en una cadena con otra subcadena.
 *
 * @param str La cadena original.
 * @param find La subcadena que se va a reemplazar.
 * @param replace La subcadena para reemplazar las apariciones de 'find'.
 * @return Una nueva cadena con todas las apariciones de 'find' reemplazadas por 'replace'.
 */
char* gb_str_replace(const char* str, const char* find, const char* replace)
{
    char **split = g_strsplit_set(str, find, -1);
    char *result = g_strjoinv(replace, split);
    g_strfreev(split);
    return result;
}

