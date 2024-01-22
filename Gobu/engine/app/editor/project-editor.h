/**********************************************************************************
 * gb_project_editor.h                                                            *
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

#ifndef GOBU_PROJECT_EDITOR_H
#define GOBU_PROJECT_EDITOR_H
#include <gtk/gtk.h>

typedef enum
{
    GOBU_RESOURCE_ICON_COMPONENT = 0,
    GOBU_RESOURCE_ICON_LEVEL,
    GOBU_RESOURCE_ICON_ENTITY,
    GOBU_RESOURCE_ICON_FOLDER,
    GOBU_RESOURCE_ICON_ANIM2D,
    GOBU_RESOURCE_ICON_ASPRITE,
    GOBU_RESOURCE_ICONS
} GobuAppResourceIcon;

typedef enum
{
    GAPP_PE_NOTEBOOK_DEFAULT,
    GAPP_PE_NOTEBOOK_BOTTOM,
    GAPP_PE_NOTEBOOK_NUMS
}eProjectEditorNotebook;

typedef struct _ProjectEditor
{
    GtkWindow *window;
    GtkWidget *notebook[GAPP_PE_NOTEBOOK_NUMS];
} ProjectEditor;

void gapp_project_editor_window_new(GtkApplication *app);
void gapp_project_editor_append_page(eProjectEditorNotebook notebook_id, int icon_type, const gchar* name, GtkWidget* child);
GdkPixbuf *project_editor_get_icon(GobuAppResourceIcon icon);

#endif // GOBU_PROJECT_EDITOR_H
