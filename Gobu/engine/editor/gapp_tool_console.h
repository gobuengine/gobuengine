/**********************************************************************************
 * gapp_tool_console.h                                                            *
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

#ifndef __GAPP_TOOL_CONSOLE_H__
#define __GAPP_TOOL_CONSOLE_H__
#include <stdio.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TF gb_strdups

typedef struct _GobuToolConsolePrivate GobuToolConsolePrivate;

#define GAPP_TOOL_TYPE_CONSOLE (gapp_tool_console_get_type())
G_DECLARE_FINAL_TYPE(GobuToolConsole, gapp_tool_console, GAPP_TOOL, CONSOLE, GtkBox)

struct _GobuToolConsolePrivate
{
    GtkWidget parent_instance;
    GtkWidget* console;
    GtkWidget* scroll;
};

GobuToolConsole* gapp_tool_console_new(void);
void gb_print_error(const gchar *msg, const gchar *error);
void gb_print_warning(const gchar *msg);
void gb_print_success(const gchar *msg);
void gb_print_info(const gchar *msg);

G_END_DECLS

#endif // __GAPP_TOOL_CONSOLE_H__

