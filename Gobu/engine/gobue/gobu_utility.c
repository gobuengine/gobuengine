#include "gobu_utility.h"
#include <glib.h>

char* gb_path_join(const char* first_path, ...)
{
    gchar* buffer;
    va_list args;

    va_start(args, first_path);
    buffer = g_build_filename_valist(first_path, &args);
    va_end(args);

    return buffer;
}

char* gb_path_basename(const char* filename)
{
    return g_path_get_basename(filename);
}

char* gb_path_dirname(const char* filename)
{
    return g_path_get_dirname(filename);
}

char* gb_path_current_dir(void)
{
    return g_get_current_dir();
}

bool gb_path_exist(const char* filename)
{
    return gb_fs_test(filename, G_FS_TEST_IS_DIR);
}

bool gb_fs_write_file(const char* filename, const char* contents)
{
    return g_file_set_contents(filename, contents, strlen(contents), NULL);
}

bool gb_fs_mkdir(const char* pathname)
{
    return g_mkdir_with_parents(pathname, 0) == -1 ? false : true;
}

//  FILE SYSTEM
bool gb_fs_file_exist(const char* filename)
{
    return gb_fs_test(filename, G_FS_TEST_EXISTS);
}

bool gb_fs_test(const char* filename, GFSTest test)
{
    return g_file_test(filename, test);
}

const char* gb_fs_extname(const char* filename)
{
    if (filename[0] == '.') return filename;
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    return dot;
}

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

bool gb_fs_is_extension(const char* filename, const char* ext)
{
    return (g_strcmp0(gb_fs_extname(filename), ext) == 0);
}

unsigned char* gb_fs_get_contents(const char* filename, int* dataSize)
{
    unsigned char* data = NULL;
    *dataSize = 0;

    if (filename != NULL && g_file_get_contents(filename, &data, dataSize, NULL))
    {
        printf("Error\n");
    }

    return data;
}

// STRING
const char* gb_strdups(const char* format, ...)
{
    gchar* buffer;
    va_list args;

    va_start(args, format);
    buffer = g_strdup_vprintf(format, args);
    va_end(args);

    return buffer;
}

const char* gb_strdup(const char* str)
{
    return g_strdup(str);
}

const char* gb_str_tolower(const char* text)
{
    return g_utf8_strdown(text, strlen(text));
}

bool gb_str_isequal(const char* text1, const char* text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0) result = true;
    }

    return result;
}

char** gb_str_split(const char* string, const char* delimiter)
{
    return g_strsplit(string, delimiter, -1);
}

void gb_str_split_free(char** str_array)
{
    g_strfreev(str_array);
}
