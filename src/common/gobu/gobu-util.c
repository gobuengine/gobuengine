#include "gobu-util.h"
#include <gio/gio.h>

go_internal bool go_util_test_file(const char *filename, GFileTest test);

go_public bool go_util_path_isdir(const char *pathname)
{
    g_return_val_if_fail(pathname != NULL, false);
    return go_util_test_file(pathname, G_FILE_TEST_IS_DIR);
}

go_public bool go_util_path_create(const char *pathname)
{
    return g_mkdir_with_parents(pathname, 0755) == -1 ? false : true;
}

go_public const char *go_util_path_user(void)
{
    return g_get_user_config_dir();
}

go_public char *go_util_path_normalize(const char *path)
{
    return go_util_string_replace(path, "\\", "/");
}

go_public char *go_util_path_build_(const char *first_path, ...)
{
    gchar *buffer;
    va_list args;

    va_start(args, first_path);
    buffer = g_build_filename_valist(first_path, &args);
    va_end(args);

    return buffer;
}

go_public char *go_util_path_basename(const char *filename)
{
    return g_path_get_basename(filename);
}

go_public char *go_util_path_dirname(const char *filename)
{
    return g_path_get_dirname(filename);
}

go_public char *go_util_path_current_dir(void)
{
    return g_get_current_dir();
}

go_public bool go_util_path_exist(const char *filename)
{
    return go_util_test_file(filename, G_FILE_TEST_EXISTS);
}

go_public char *go_util_string_format(const char *format, ...)
{
    gchar *buffer;
    va_list args;

    va_start(args, format);
    buffer = g_strdup_vprintf(format, args);
    va_end(args);

    return buffer;
}

go_public char *go_util_string(const char *str)
{
    return g_strdup(str);
}

go_public char *go_util_string_tolower(const char *text)
{
    return g_utf8_strdown(text, strlen(text));
}

go_public char *go_util_string_uppercase(const char *text)
{
    return g_utf8_strup(text, strlen(text));
}

go_public bool go_util_string_isequal(const char *text1, const char *text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0)
            result = true;
    }

    return result;
}

go_public char **go_util_string_split(const char *string, const char *delimiter)
{
    return g_strsplit(string, delimiter, -1);
}

go_public void go_util_string_split_free(char **str_array)
{
    g_strfreev(str_array);
}

go_public char *go_util_string_trim(char *str)
{
    return g_strstrip(str);
}

go_public char *go_util_string_remove_spaces(char *str)
{
    return go_util_string_replace(g_strdup(str), " ", "");
}

go_public char *go_util_string_sanitize(char *str)
{
    return go_util_string_replace(g_strdup(str), G_STR_DELIMITERS, "");
}

go_public char *go_util_string_replace(const char *str, const char *find, const char *replace)
{
    char **split = g_strsplit(str, find, -1);
    char *result = g_strjoinv(replace, split);
    g_strfreev(split);
    return result;
}

go_public bool go_util_copy_file(const char *src, const char *dest)
{
    GFile *fsrc = g_file_new_for_path(src);

    if (go_util_path_exist(src))
    {
        if (go_util_path_create(dest))
        {
            GFileEnumerator *enumerator = g_file_enumerate_children(fsrc, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);
            GFileInfo *file_info = NULL;
            while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL)
            {
                const gchar *file_name = g_file_info_get_name(file_info);
                go_util_copy_file(go_util_path_build(src, file_name), go_util_path_build(dest, file_name));
            }
        }
        return TRUE;
    }

    return g_file_copy(fsrc, g_file_new_for_path(dest), G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);
}

go_public bool go_util_write_text_file(const char *filename, const char *contents)
{
    return g_file_set_contents(filename, contents, strlen(contents), NULL);
}

go_public bool go_util_exist_file(const char *filename)
{
    return go_util_test_file(filename, G_FILE_TEST_EXISTS);
}

go_internal bool go_util_test_file(const char *filename, GFileTest test)
{
    return g_file_test(filename, test);
}

go_public const char *go_util_extname(const char *filename)
{
    if (filename[0] == '.')
        return filename;
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return NULL;
    return dot;
}

go_public char *go_util_name(const char *filename, bool with_ext)
{
    gchar *basename = go_util_path_basename(filename);
    if (with_ext)
    {
        gchar **sep_name = go_util_string_split(basename, ".");
        gchar *name = go_util_string(sep_name[0]);
        go_util_string_split_free(sep_name);
        return name;
    }
    return basename;
}

go_public bool go_util_is_extension(const char *filename, const char *ext)
{
    return (g_strcmp0(go_util_extname(filename), ext) == 0);
}

go_public char *go_util_read_text_file(const char *filename)
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

go_public go_color_t go_color_adjust_contrast(go_color_t color, float mixFactor)
{
    // Limitar mixFactor entre 0.0 y 1.0
    if (mixFactor < 0.0f)
        mixFactor = 0.0f;
    if (mixFactor > 1.0f)
        mixFactor = 1.0f;

    // Calcular el brillo percibido del color (luma)
    float brightness = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;

    // Determinar el color de referencia (negro o blanco) para mayor contraste
    go_color_t refColor = (brightness < 128) ? (go_color_t){255, 255, 255, 255} : (go_color_t){0, 0, 0, 255};

    // Mezclar el color de entrada con el color de referencia
    return (go_color_t){
        .r = (uint8_t)(color.r * (1.0f - mixFactor) + refColor.r * mixFactor),
        .g = (uint8_t)(color.g * (1.0f - mixFactor) + refColor.g * mixFactor),
        .b = (uint8_t)(color.b * (1.0f - mixFactor) + refColor.b * mixFactor),
        .a = color.a // Mantener alpha igual
    };
}

go_public go_color_t go_color_fade(go_color_t color, float alpha)
{
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    return (go_color_t){color.r, color.g, color.b, (uint8_t)(color.a * alpha)};
}

go_public uint32_t go_color_to_uint(go_color_t color)
{
    return ((uint32_t)color.r) | ((uint32_t)color.g<<8) | ((uint32_t)color.b<<16) | ((uint32_t)color.a<<24);
}

