/**********************************************************************************
 * gapp_widget.h                                                                  *
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

#ifndef GAPP_WIDGET_H
#define GAPP_WIDGET_H
#include <gtk/gtk.h>

#define gapp_widget_set_margin_start gtk_widget_set_margin_start
#define gapp_widget_set_margin_end gtk_widget_set_margin_end
#define gapp_widget_set_margin_top gtk_widget_set_margin_top
#define gapp_widget_set_margin_bottom gtk_widget_set_margin_bottom

void gapp_widget_alert(GtkWidget *parent, const gchar *message);
void gapp_widget_dialog_confirm_delete(GtkWidget *parent, const gchar *name, gpointer *data, GCallback c_handler);
GtkWidget *gapp_widget_dialog_input(GtkWidget *parent, const gchar *title, const gchar *label, const gchar *text_default);
const gchar *gapp_widget_dialog_input_get_text(GtkWidget *dialog);
void gapp_widget_set_margin(GtkWidget *widget, gint margin);

GtkWidget *gapp_widget_button_new_icon_with_label(const gchar *icon_name, const gchar *label);
GtkWidget *gapp_widget_button_new_info(const gchar *title, const gchar *desc);
GtkWidget *gapp_widget_paned_new(GtkOrientation orientation, gboolean start);
GtkWidget *gapp_widget_paned_new_with_notebook(GtkOrientation orientation, gboolean rink_start, GtkWidget *label_start, GtkWidget *start, GtkWidget *label_end, GtkWidget *end);

GtkWidget *gapp_widget_separator_v(void);
GtkWidget *gapp_widget_separator_h(void);

GtkWidget *gapp_widget_toolbar_new(void);
void gapp_widget_toolbar_separator_new(GtkWidget *toolbar);

GtkWidget *gapp_widget_expander_with_widget(const gchar *title, GtkWidget *widget);

void gapp_widget_entry_set_text(GtkEntry *entry, const gchar* text);
const gchar* gapp_widget_entry_get_text(GtkEntry* entry);

void gapp_widget_view_set_text(GtkTextView* view, const gchar* text);
gchar* gapp_widget_view_get_text(GtkTextBuffer* buffer);

GtkWidget* gapp_widget_input_number(GtkWidget *box, const gchar* label_input, double min, double max, double step);
GtkWidget* gapp_widget_input_str(GtkWidget *box, const gchar* label_input, const gchar *default_str);

#endif // GAPP_WIDGET_H
