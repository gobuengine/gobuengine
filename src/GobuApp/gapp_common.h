#ifndef GB_COMMON_H
#define GB_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <glib.h>
#include <gio/gio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool gobu_path_is_dir(const char *pathname);

    bool gobu_path_create_new(const char *pathname);

    const char *gobu_path_user_config_dir(void);

    char *gobu_path_relative_content(const char *path);

    char *gobu_path_join_relative_content(const char *path);

    char *gobu_path_normalize(const char *path);

    char *gobu_path_join(const char *first_path, ...);

    char *gobu_path_basename(const char *filename);

    char *gobu_path_dirname(const char *filename);

    char *gobu_path_current_dir(void);

    bool gobu_path_exist(const char *filename);

    char *gobu_strdups(const char *format, ...);

    char *gobu_strdup(const char *str);

    char *gobu_str_tolower(const char *text);

    char *gobu_str_uppercase(const char *text);

    bool gobu_str_isequal(const char *text1, const char *text2);

    char **gobu_str_split(const char *string, const char *delimiter);

    void gobu_str_split_free(char **str_array);

    char *gobu_str_trim(char *str);

    char *gobu_str_remove_spaces(char *str);

    char *gobu_str_sanitize(char *str);

    char *gobu_str_replace(const char *str, const char *find, const char *replace);

    bool gobu_fs_copyc(GFile *src, GFile *dest, GError **error);

    bool gobu_fs_write_file(const char *filename, const char *contents);

    bool gobu_fs_file_exist(const char *filename);

    bool gobu_fs_test(const char *filename, GFileTest test);

    const char *gobu_fs_extname(const char *filename);

    char *gobu_fs_get_name(const char *filename, bool with_ext);

    bool gobu_fs_is_extension(const char *filename, const char *ext);

    char *gobu_fs_get_contents(const char *filename, gsize *length);

#ifdef __cplusplus
}
#endif

#endif // GB_COMMON_H
