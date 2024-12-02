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

    bool pathIsDir(const char *pathname);

    bool pathCreateNew(const char *pathname);

    const char *pathUserConfigDir(void);

    char *pathRelativeContent(const char *path);

    char *pathNormalize(const char *path);

    char *pathJoin(const char *first_path, ...);

    char *pathBasename(const char *filename);

    char *pathDirname(const char *filename);

    char *pathCurrentDir(void);

    bool pathExist(const char *filename);

    char *stringDups(const char *format, ...);

    char *stringDup(const char *str);

    char *stringTolower(const char *text);

    char *stringUppercase(const char *text);

    bool stringIsequal(const char *text1, const char *text2);

    char **stringSplit(const char *string, const char *delimiter);

    void stringSplitFree(char **str_array);

    char *stringTrim(char *str);

    char *stringRemoveSpaces(char *str);

    char *stringSanitize(char *str);

    char *stringReplace(const char *str, const char *find, const char *replace);

    bool fsCopy(GFile *src, GFile *dest, GError **error);

    bool fsWrite(const char *filename, const char *contents);

    bool fsExist(const char *filename);

    bool fsTest(const char *filename, GFileTest test);

    const char *fsGetExtname(const char *filename);

    char *fsGetName(const char *filename, bool with_ext);

    bool fsIsExtension(const char *filename, const char *ext);

    char *fsRead(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // GB_COMMON_H
