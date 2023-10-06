/**********************************************************************************
 * gobu_widget.h                                                          *
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

#ifndef __GOBU_WIDGET_H__
#define __GOBU_WIDGET_H__
#include <gtk/gtk.h>

#define gobu_widget_set_margin_start gtk_widget_set_margin_start
#define gobu_widget_set_margin_end gtk_widget_set_margin_end
#define gobu_widget_set_margin_top gtk_widget_set_margin_top
#define gobu_widget_set_margin_bottom gtk_widget_set_margin_bottom

void gobu_widget_theme_init(gboolean dark_mode);

void gobu_widget_alert(GtkWidget *parent, const gchar *message);
void gobu_widget_dialog_confirm_delete(GtkWidget *parent, const gchar *name, gpointer *data, GCallback c_handler);
GtkWidget *gobu_widget_dialog_input(GtkWidget *parent, const gchar *title, const gchar *text_default);
const gchar *gobu_widget_dialog_input_get_text(GtkWidget *dialog);
void gobu_widget_set_margin(GtkWidget *widget, gint margin);

GtkWidget *gobu_widget_button_new_icon_with_label(const gchar *icon_name, const gchar *label);
GtkWidget *gobu_widget_button_new_info(const gchar *title, const gchar *desc);
GtkWidget *gobu_widget_paned_new(GtkOrientation orientation, gboolean start);
GtkWidget *gobu_widget_paned_new_with_notebook(GtkOrientation orientation, gboolean rink_start, GtkWidget *label_start, GtkWidget *start, GtkWidget *label_end, GtkWidget *end);

GtkWidget *gobu_widget_separator_v(void);
GtkWidget *gobu_widget_separator_h(void);

GtkWidget *gobu_widget_toolbar_new(void);
void gobu_widget_toolbar_separator_new(GtkWidget *toolbar);

#endif // __GOBU_WIDGET_H__