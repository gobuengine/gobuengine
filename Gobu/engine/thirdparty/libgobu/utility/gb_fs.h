#ifndef GB_FS_H
#define GB_FS_H

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

    bool gb_fs_copyc(GFile *src, GFile *dest, GError **error);

    bool gb_fs_write_file(const char *filename, const char *contents);

    bool gb_fs_file_exist(const char *filename);

    bool gb_fs_test(const char *filename, GFileTest test);

    const char *gb_fs_extname(const char *filename);

    char *gb_fs_get_name(const char *filename, bool with_ext);

    bool gb_fs_is_extension(const char *filename, const char *ext);

    char *gb_fs_get_contents(const char *filename, gsize *length);

#ifdef __cplusplus
}
#endif

#endif // GB_FS_H
