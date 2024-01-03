#include "gb_string.h"


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
    char** split = g_strsplit(str, find, -1);
    char* result = g_strjoinv(replace, split);
    g_strfreev(split);
    return result;
}

