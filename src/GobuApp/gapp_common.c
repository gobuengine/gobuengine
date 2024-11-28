#include "gapp_common.h"
#include "config.h"

static char *project_path = NULL;

/**
 * Verifica si la ruta especificada corresponde a un directorio.
 *
 * Esta función comprueba si la ruta dada es un directorio existente
 * utilizando la función fsTest() con el flag G_FILE_TEST_IS_DIR.
 *
 * @param pathname La ruta del archivo o directorio a verificar.
 * @return true si la ruta existe y es un directorio, false en caso contrario.
 */
bool pathIsDir(const char *pathname)
{
    g_return_val_if_fail(pathname != NULL, false);
    return fsTest(pathname, G_FILE_TEST_IS_DIR);
}

/**
 * @brief Crea un directorio con la ruta especificada.
 *
 * @param pathname La ruta del directorio a crear.
 * @return true si el directorio se creó correctamente, false en caso contrario.
 */
bool pathCreateNew(const char *pathname)
{
    return g_mkdir_with_parents(pathname, 0755) == -1 ? false : true;
}

/**
 * Obtiene la ruta del directorio de configuración del usuario.
 *
 * Esta función es un wrapper alrededor de g_get_user_config_dir() de GLib,
 * que proporciona la ruta al directorio de configuración específico del usuario.
 *
 * @return La ruta al directorio de configuración del usuario.
 *         El string retornado pertenece a GLib y no debe ser modificado o liberado.
 */
const char *pathUserConfigDir(void)
{
    return g_get_user_config_dir();
}

/**
 * @brief Retorna el contenido de una ruta relativa.
 *
 * Esta función toma una ruta relativa como entrada y devuelve el contenido de ese archivo.
 *
 * @param path La ruta relativa del archivo.
 * @return El contenido del archivo como una cadena de caracteres.
 */
char *pathRelativeContent(const char *path)
{
    char *npath = strstr(path, GAPP_PROJECT_CONTENT);
    if (npath != NULL)
    {
        npath += strlen(GAPP_PROJECT_CONTENT);
    }
    else
    {
        npath = stringDup(path);
    }

    return npath;
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
char *pathNormalize(const char *path)
{
    return StringReplace(path, "\\", "/");
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
char *pathJoin(const char *first_path, ...)
{
    gchar *buffer;
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
char *pathBasename(const char *filename)
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
char *pathDirname(const char *filename)
{
    return g_path_get_dirname(filename);
}

/**
 * Retorna la ruta del directorio actual.
 *
 * @return Un puntero a una cadena que representa la ruta del directorio actual.
 */
char *pathCurrentDir(void)
{
    return g_get_current_dir();
}

/**
 * @brief Verifica si existe un folder en la ruta especificada.
 *
 * @param filename El nombre del folder a verificar.
 * @return true si el folder existe, false en caso contrario.
 */
bool pathExist(const char *filename)
{
    return fsTest(filename, G_FILE_TEST_IS_DIR);
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
char *stringDups(const char *format, ...)
{
    gchar *buffer;
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
char *stringDup(const char *str)
{
    return g_strdup(str);
}

/**
 * Convierte una cadena de texto a minúsculas.
 *
 * @param text La cadena de texto a convertir.
 * @return La cadena de texto convertida en minúsculas.
 */
char *StringTolower(const char *text)
{
    return g_utf8_strdown(text, strlen(text));
}

/**
 * Convierte una cadena a mayúsculas.
 *
 * @param text La cadena que se va a convertir.
 * @return La cadena convertida en mayúsculas.
 */
char *StringUppercase(const char *text)
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
bool StringIsequal(const char *text1, const char *text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0)
            result = true;
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
char **StringSplit(const char *string, const char *delimiter)
{
    return g_strsplit(string, delimiter, -1);
}

/**
 * @brief Libera la memoria asignada a un array de cadenas de caracteres.
 *
 * @param str_array El array de cadenas de caracteres a liberar.
 */
void StringSplitFree(char **str_array)
{
    g_strfreev(str_array);
}

/**
 * @brief Elimina los espacios en blanco al principio y al final de una cadena de caracteres.
 *
 * @param str La cadena de caracteres a recortar.
 * @return Un puntero a la cadena de caracteres recortada.
 */
char *StringTrim(char *str)
{
    return g_strstrip(str);
}

/**
 * @brief Elimina los espacios en blanco de una cadena de caracteres.
 *
 * @param str La cadena de caracteres de la cual se eliminarán los espacios en blanco.
 * @return Un puntero a la cadena de caracteres resultante sin espacios en blanco.
 */
char *StringRemoveSpaces(char *str)
{
    return StringReplace(g_strdup(str), " ", "");
}

/**
 * @brief Sanitiza una cadena eliminando cualquier carácter especial o símbolo.
 *
 * @param str La cadena que se va a sanitizar.
 * @return Un puntero a la cadena sanitizada.
 */
char *StringSanitize(char *str)
{
    return StringReplace(g_strdup(str), G_STR_DELIMITERS, "");
}

/**
 * Reemplaza todas las apariciones de una subcadena en una cadena con otra subcadena.
 *
 * @param str La cadena original.
 * @param find La subcadena que se va a reemplazar.
 * @param replace La subcadena para reemplazar las apariciones de 'find'.
 * @return Una nueva cadena con todas las apariciones de 'find' reemplazadas por 'replace'.
 */
char *StringReplace(const char *str, const char *find, const char *replace)
{
    char **split = g_strsplit(str, find, -1);
    char *result = g_strjoinv(replace, split);
    g_strfreev(split);
    return result;
}

/**
 * @brief Copia un archivo desde la ruta de origen a la ruta de destino.
 *
 * Esta función copia el contenido del archivo de origen al archivo de destino.
 *
 * @param src El archivo de origen que se copiará.
 * @param dest El archivo de destino donde se copiará el contenido.
 */
bool fsCopy(GFile *src, GFile *dest, GError **error)
{
    if (pathExist(g_file_get_path(src)))
    {
        if (pathCreateNew(g_file_get_path(dest)))
        {
            GFileEnumerator *enumerator = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);
            GFileInfo *file_info = NULL;
            while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL)
            {
                const gchar *file_name = g_file_info_get_name(file_info);
                GFile *file_src = g_file_new_for_path(pathJoin(g_file_get_path(src), file_name, NULL));
                GFile *file_dest = g_file_new_for_path(pathJoin(g_file_get_path(dest), file_name, NULL));
                fsCopy(file_src, file_dest, error);
            }
        }
        return TRUE;
    }

    return g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, error);
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
bool fsWrite(const char *filename, const char *contents)
{
    return g_file_set_contents(filename, contents, strlen(contents), NULL);
}

/**
 * @brief Verifica si un archivo existe en el sistema de archivos.
 *
 * Esta función comprueba si un archivo con el nombre especificado existe en el sistema de archivos.
 *
 * @param filename El nombre del archivo a verificar.
 * @return true si el archivo existe, false en caso contrario.
 */
bool fsExist(const char *filename)
{
    return fsTest(filename, G_FILE_TEST_EXISTS);
}

/**
 * @brief Prueba el sistema de archivos para el nombre de archivo dado utilizando la prueba especificada.
 *
 * @param filename El nombre del archivo a probar.
 * @param test El tipo de prueba a realizar en el sistema de archivos.
 * @return true si la prueba es exitosa, false en caso contrario.
 */
bool fsTest(const char *filename, GFileTest test)
{
    return g_file_test(filename, test);
}

/**
 * @brief Obtiene la extensión de un nombre de archivo.
 *
 * Esta función toma un nombre de archivo como entrada y devuelve la extensión del archivo.
 *
 * @param filename El nombre del archivo del cual se desea obtener la extensión.
 * @return La extensión del archivo.
 */
const char *fsGetExtname(const char *filename)
{
    if (filename[0] == '.')
        return filename;
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return NULL;
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
char *fsGetName(const char *filename, bool with_ext)
{
    gchar *basename = pathBasename(filename);
    if (with_ext)
    {
        gchar **sep_name = StringSplit(basename, ".");
        gchar *name = stringDup(sep_name[0]);
        StringSplitFree(sep_name);
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
bool fsIsExtension(const char *filename, const char *ext)
{
    return (g_strcmp0(fsGetExtname(filename), ext) == 0);
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
char *fsRead(const char *filename)
{
    GError *error = NULL;
    gchar *contents = NULL;
    gsize *length = 0;

    if (g_file_get_contents(filename, &contents, length, &error) == FALSE)
    {
        g_error("Error reading file: %s", error->message);
        g_error_free(error);
    }
    return contents;
}
