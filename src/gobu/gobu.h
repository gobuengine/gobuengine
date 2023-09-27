/**********************************************************************************
 * gobu.h                                                          *
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

#ifndef __GOBU_H__
#define __GOBU_H__
#include <glib.h>
#include "thirdparty/binn/binn.h"

typedef struct _GobuContext
{
    struct
    {
        gboolean (*load)(const gchar *path);
        gchar *path;
        gchar *name;
        gchar *component;
    } project;

    struct
    {
        void *(*parse)(const gchar *stringify);
        gchar *(*stringify)(void *obj_json);
        void *(*parse_from_file)(const gchar *filename);
        void *(*stringify_from_file)(void *obj_json, const gchar *filename);
    } JSON;
} GobuContext;

GobuContext *GobuContextGet();
void GobuContext_init(void);
void GobuContext_free(void);

#endif // __GOBU_H__