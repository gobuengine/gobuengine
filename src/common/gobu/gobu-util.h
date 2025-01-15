#ifndef GOBU_UTIL_H
#define GOBU_UTIL_H

#include "gobu-type.h"
#include <glib.h>

#define go_util_path_build(...) go_util_path_build_(__VA_ARGS__, NULL)

go_public bool go_util_path_isdir(const char *pathname);
go_public bool go_util_path_create(const char *pathname);
go_public const char *go_util_path_user(void);
go_public char *go_util_path_normalize(const char *path);
go_public char *go_util_path_build_(const char *first_path, ...);
go_public char *go_util_path_basename(const char *filename);
go_public char *go_util_path_dirname(const char *filename);
go_public char *go_util_path_current_dir(void);
go_public bool go_util_path_exist(const char *filename);

go_public char *go_util_string_format(const char *format, ...);
go_public char *go_util_string(const char *str);
go_public char *go_util_string_tolower(const char *text);
go_public char *go_util_string_uppercase(const char *text);
go_public bool go_util_string_isequal(const char *text1, const char *text2);
go_public char **go_util_string_split(const char *string, const char *delimiter);
go_public void go_util_string_split_free(char **str_array);
go_public char *go_util_string_trim(char *str);
go_public char *go_util_string_remove_spaces(char *str);
go_public char *go_util_string_sanitize(char *str);
go_public char *go_util_string_replace(const char *str, const char *find, const char *replace);

go_public bool go_util_copy_file(const char *src, const char *dest);
go_public bool go_util_write_text_file(const char *filename, const char *contents);
go_public bool go_util_exist_file(const char *filename);
// go_public bool go_util_test_file(const char *filename, GFileTest test);
go_public const char *go_util_extname(const char *filename);
go_public char *go_util_name(const char *filename, bool with_ext);
go_public bool go_util_is_extension(const char *filename, const char *ext);
go_public char *go_util_read_text_file(const char *filename);

go_public go_color_t go_color_adjust_contrast(go_color_t color, float mixFactor);
go_public go_color_t go_color_fade(go_color_t color, float alpha);
go_public uint32_t go_color_to_uint(go_color_t color);

#endif // GOBU_UTIL_H

