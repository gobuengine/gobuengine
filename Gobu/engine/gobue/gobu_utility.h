/**********************************************************************************
 * gb_utility.h                                                                     *
 **********************************************************************************
 * GOBU ENGINE                                                                    *
 * https://gobuengine.org                                                         *
 **********************************************************************************
 * Copyright (c) 2023 Jhonson Ozuna Mejia                                         *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                                *
 *                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
 * SOFTWARE.                                                                      *
 **********************************************************************************/

#ifndef __BUGO_STR_H__
#define __BUGO_STR_H__
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glib.h>

typedef enum
{
    G_FS_TEST_IS_REGULAR = 1 << 0,
    G_FS_TEST_IS_SYMLINK = 1 << 1,
    G_FS_TEST_IS_DIR = 1 << 2,
    G_FS_TEST_IS_EXECUTABLE = 1 << 3,
    G_FS_TEST_EXISTS = 1 << 4
} GFSTest;

char* gb_path_join(const char* first_path, ...);
char* gb_path_basename(const char* filename);
char* gb_path_dirname(const char* filename);
char* gb_path_current_dir(void);
bool gb_path_exist(const char* filename);

bool gb_fs_write_file(const char* filename, const char* contents);
bool gb_fs_mkdir(const char* pathname);
bool gb_fs_file_exist(const char* filename);
bool gb_fs_test(const char* filename, GFSTest test);
const char* gb_fs_extname(const char* filename);
char* gb_fs_get_name(const char* filename, bool with_ext);
bool gb_fs_is_extension(const char* filename, const char* ext);
gchar* gb_fs_get_contents(const char* filename, gsize *length);

char* gb_strdups(const char* format, ...);
char* gb_strdup(const char* str);
char* gb_str_tolower(const char* text);
char* gb_str_uppercase(const char* text);
bool gb_str_isequal(const char* text1, const char* text2);
char** gb_str_split(const char* string, const char* delimiter);
void gb_str_split_free(char** str_array);
char* gb_str_trim(char* str);
char* gb_str_remove_spaces(char* str);
char* gb_str_sanitize(char* str);
char* gb_str_replace(const char* str, const char* find, const char* replace);

#endif // __BUGO_STR_H__
