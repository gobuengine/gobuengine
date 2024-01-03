#ifndef GB_STRING_H
#define GB_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

    char *gb_strdups(const char *format, ...);

    char *gb_strdup(const char *str);

    char *gb_str_tolower(const char *text);

    char *gb_str_uppercase(const char *text);

    bool gb_str_isequal(const char *text1, const char *text2);

    char **gb_str_split(const char *string, const char *delimiter);

    void gb_str_split_free(char **str_array);

    char *gb_str_trim(char *str);

    char *gb_str_remove_spaces(char *str);

    char *gb_str_sanitize(char *str);

    char *gb_str_replace(const char *str, const char *find, const char *replace);

#ifdef __cplusplus
}
#endif

#endif // GB_STRING_H
