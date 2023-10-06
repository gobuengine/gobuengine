/**********************************************************************************
 * gobu-app.h                                                           *
 * ********************************************************************************
 * GOBU ENGINE                                                                    *
 * https://gobuengine.org                                                         *
 * ********************************************************************************
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

#ifndef __GOBU_APP_H__
#define __GOBU_APP_H__
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "config.h"
#include "gobu/gobu-gobu.h"

typedef enum
{
    GOBU_RESOURCE_ICON_COMPONENT = 0,
    GOBU_RESOURCE_ICON_LEVEL,
    GOBU_RESOURCE_ICON_ENTITY,
    GOBU_RESOURCE_ICON_FOLDER,
    GOBU_RESOURCE_ICON_ANIM2D,
    GOBU_RESOURCE_ICONS
} GobuAppResourceIcon;

typedef struct _GobuApp
{
    GtkWidget *window;

    struct
    {
        struct
        {
            GtkStringList *list;
            GHashTable *table;
            binn *data;
        } Script;
    } Module;

    struct
    {
        GdkPixbuf *icons[GOBU_RESOURCE_ICONS];
    } resource;

    struct
    {
        struct
        {
            GtkSourceLanguage *source_language;
            GtkSourceStyleScheme *style_scheme;
        } scripts;

        binn *data;
    } setting;

} GobuApp;

#endif // __GOBU_APP_H__