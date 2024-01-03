#ifndef GB_UTILITY_PATH_H
#define GB_UTILITY_PATH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    char* gb_path_relative_content(const char* path);

    char* gb_path_join_relative_content(const char* path);

    char* gb_path_normalize(const char* path);

    char* gb_path_join(const char* first_path, ...);

    char* gb_path_basename(const char* filename);

    char* gb_path_dirname(const char* filename);

    char* gb_path_current_dir(void);

    bool gb_path_exist(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // GB_PATH_H

