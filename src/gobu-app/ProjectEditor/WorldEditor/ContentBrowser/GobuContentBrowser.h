/**********************************************************************************
 * GobuContentBrowser.h                                                           *
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

#ifndef __GOBU_CONTENT_BROWSER_H__
#define __GOBU_CONTENT_BROWSER_H__
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GobuBrowserContentPrivate GobuBrowserContentPrivate;

#define GTK_TYPE_GOBU_BROWSER_CONTENT (gobu_browser_content_get_type())
G_DECLARE_FINAL_TYPE(GobuBrowserContent, gobu_browser_content, GTK, GOBU_BROWSER_CONTENT, GtkBox)

typedef enum
{
    BROWSER_CONTENT_SPRITE_FILE,
    BROWSER_CONTENT_TILED_FILE
} BrowserContentTypeFile;

struct _GobuBrowserContentPrivate
{
    gchar *path_default;
    gchar *path_current;

    GPtrArray *path_back;
    GPtrArray *path_forward;

    GtkWidget *btn_nav_home;
    GtkWidget *btn_nav_back;
    GtkWidget *btn_nav_forward;

    GtkListItemFactory *factory;
    GtkDirectoryList *directory_list;
    GtkSingleSelection *selection;
    GtkWidget *popover;
};

GobuBrowserContentPrivate *GobuContentBrowserGet(GobuBrowserContent *browser);

G_END_DECLS

#endif // __GOBU_CONTENT_BROWSER_H__