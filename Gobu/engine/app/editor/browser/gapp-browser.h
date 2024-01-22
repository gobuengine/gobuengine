/**********************************************************************************
 * browser.h                                                                      *
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

#ifndef GBAPP_BROWSER_H
#define GBAPP_BROWSER_H
#include "../project-editor.h"
#include "utility/utility.h"
#include "gapp_widget.h"

G_BEGIN_DECLS

typedef struct _GappBrowserPrivate GappBrowserPrivate;

#define GAPP_TYPE_BROWSER (gapp_browser_get_type())
G_DECLARE_FINAL_TYPE(GappBrowser, gapp_browser, GAPP, BROWSER, GtkBox)

typedef enum
{
    ACTION_CREATE_FOLDER,
    ACTION_CREATE_ENTITY,
    ACTION_CREATE_LEVEL,
    ACTION_CREATE_ASPRITES,
    ACTION_LAST
} BrowserContentMenuItem;

typedef enum
{
    BROWSER_CONTENT_SPRITE_FILE,
    BROWSER_CONTENT_TILED_FILE
} BrowserContentTypeFile;

typedef struct
{
    gchar *title;
    gchar *label;
    gchar *text;
    int response;
    gpointer data;
} GappBrowserAction;

struct _GappBrowserPrivate
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
    GtkMultiSelection *selection;
    GtkPopover *popover;
};

GappBrowserAction *gapp_browser_fn_new_action(const gchar *title, const gchar *label, const gchar *text, int response, gpointer data);

void signal_create_item_action(GtkWidget *button, const GappBrowserAction *action);

void gapp_browser_fn_open_folder(GappBrowser *browser, const gchar *path, gboolean bhistory_saved);

void gapp_browser_signal_create_item_action(GtkWidget *button, const GappBrowserAction *action);

GtkWidget *gapp_browser_new(void);

G_END_DECLS

#endif // GBAPP_BROWSER_H
