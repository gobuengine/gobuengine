#include "gobu.h"
#include <glib.h>
#include <gio/gio.h>

static bool gobu_util_test_file(const char *filename, GFileTest test);

bool gobu_util_path_isdir(const char *pathname)
{
    g_return_val_if_fail(pathname != NULL, false);
    return gobu_util_test_file(pathname, G_FILE_TEST_IS_DIR);
}

bool gobu_util_path_create(const char *pathname)
{
    return g_mkdir_with_parents(pathname, 0755) == -1 ? false : true;
}

const char *gobu_util_path_user(void)
{
    return g_get_user_config_dir();
}

char *gobu_util_path_normalize(const char *path)
{
    return gobu_util_string_replace(path, "\\", "/");
}

char *gobu_util_path_build_(const char *first_path, ...)
{
    gchar *buffer;
    va_list args;

    va_start(args, first_path);
    buffer = g_build_filename_valist(first_path, &args);
    va_end(args);

    return buffer;
}

char *gobu_util_path_basename(const char *filename)
{
    return g_path_get_basename(filename);
}

char *gobu_util_path_dirname(const char *filename)
{
    return g_path_get_dirname(filename);
}

char *gobu_util_path_current_dir(void)
{
    return g_get_current_dir();
}

bool gobu_util_path_exist(const char *filename)
{
    return gobu_util_test_file(filename, G_FILE_TEST_EXISTS);
}

char *gobu_util_string_format(const char *format, ...)
{
    gchar *buffer;
    va_list args;

    va_start(args, format);
    buffer = g_strdup_vprintf(format, args);
    va_end(args);

    return buffer;
}

char *gobu_util_string(const char *str)
{
    return g_strdup(str);
}

char *gobu_util_string_tolower(const char *text)
{
    return g_utf8_strdown(text, strlen(text));
}

char *gobu_util_string_uppercase(const char *text)
{
    return g_utf8_strup(text, strlen(text));
}

bool gobu_util_string_isequal(const char *text1, const char *text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0)
            result = true;
    }

    return result;
}

char **gobu_util_string_split(const char *string, const char *delimiter)
{
    return g_strsplit(string, delimiter, -1);
}

void gobu_util_string_split_free(char **str_array)
{
    g_strfreev(str_array);
}

char *gobu_util_string_trim(char *str)
{
    return g_strstrip(str);
}

char *gobu_util_string_remove_spaces(char *str)
{
    return gobu_util_string_replace(g_strdup(str), " ", "");
}

char *gobu_util_string_sanitize(char *str)
{
    return gobu_util_string_replace(g_strdup(str), G_STR_DELIMITERS, "");
}

char *gobu_util_string_replace(const char *str, const char *find, const char *replace)
{
    char **split = g_strsplit(str, find, -1);
    char *result = g_strjoinv(replace, split);
    g_strfreev(split);
    return result;
}

bool gobu_util_copy_file(const char *src, const char *dest)
{
    GFile *fsrc = g_file_new_for_path(src);

    if (gobu_util_path_exist(src))
    {
        if (gobu_util_path_create(dest))
        {
            GFileEnumerator *enumerator = g_file_enumerate_children(fsrc, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);
            GFileInfo *file_info = NULL;
            while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL)
            {
                const gchar *file_name = g_file_info_get_name(file_info);
                gobu_util_copy_file(gobu_util_path_build(src, file_name), gobu_util_path_build(dest, file_name));
            }
        }
        return TRUE;
    }

    return g_file_copy(fsrc, g_file_new_for_path(dest), G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);
}

bool gobu_util_write_text_file(const char *filename, const char *contents)
{
    return g_file_set_contents(filename, contents, strlen(contents), NULL);
}

bool gobu_util_exist_file(const char *filename)
{
    return gobu_util_test_file(filename, G_FILE_TEST_EXISTS);
}

static bool gobu_util_test_file(const char *filename, GFileTest test)
{
    return g_file_test(filename, test);
}

const char *gobu_util_extname(const char *filename)
{
    if (filename[0] == '.')
        return filename;
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return NULL;
    return dot;
}

char *gobu_util_name(const char *filename, bool with_ext)
{
    gchar *basename = gobu_util_path_basename(filename);
    if (with_ext)
    {
        gchar **sep_name = gobu_util_string_split(basename, ".");
        gchar *name = gobu_util_string(sep_name[0]);
        gobu_util_string_split_free(sep_name);
        return name;
    }
    return basename;
}

bool gobu_util_is_extension(const char *filename, const char *ext)
{
    return (g_strcmp0(gobu_util_extname(filename), ext) == 0);
}

char *gobu_util_read_text_file(const char *filename)
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
