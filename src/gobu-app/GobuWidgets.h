/**********************************************************************************
 * Widgets.h                                                          *
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

#ifndef __GOBU_WIDGETS_H__
#define __GOBU_WIDGETS_H__
#include <gtk/gtk.h>

#define GobuWidgetSetMarginStart gtk_widget_set_margin_start
#define GobuWidgetSetMarginEnd gtk_widget_set_margin_end
#define GobuWidgetSetMarginTop gtk_widget_set_margin_top
#define GobuWidgetSetMarginBottom gtk_widget_set_margin_bottom

void GobuWidgetThemeInit(void);
void GobuWidgetAlert(GtkWidget *parent, const gchar *message);
void GobuWidgetDialogConfirmDelete(GtkWidget *parent, GFile *file, GCallback c_handler);
GtkWidget *GobuWidgetDialogInput(GtkWidget *parent, const gchar *title, const gchar *text_default);
const gchar *GobuWidgetDialogInputGetText(GtkWidget *dialog);
void GobuWidgetSetMargin(GtkWidget *widget, gint margin);
GtkWidget *GobuWidgetButtonIconLabelNew(const gchar *icon_name, const gchar *label);
GtkWidget *GobuWidgetButtonInfoNew(const gchar *title, const gchar *desc);
GtkWidget *GobuWidgetPanedNew(GtkOrientation orientation, gboolean start);
GtkWidget *GobuWidgetPanedNotebookNew(GtkOrientation orientation, gboolean rink_start, GtkWidget *label_start, GtkWidget *start, GtkWidget *label_end, GtkWidget *end);
GtkWidget *GobuWidgetToolbarNew(void);
void GobuWidgetToolbarSeparatorW(GtkWidget *toolbar);
GtkWidget *GobuWidgetSeparatorH(void);
GtkWidget *GobuWidgetSeparatorV(void);

#endif // __GOBU_WIDGETS_H__