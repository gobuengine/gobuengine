/**********************************************************************************
 * gtkray.h                                                                       *
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

#ifndef GTK_RAY_H
#define GTK_RAY_H
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkRayPrivate GtkRayPrivate;

#define GTK_TYPE_RAY (gtk_ray_get_type())
G_DECLARE_FINAL_TYPE(GtkRay, gtk_ray, GTK, RAY, GtkGLArea)

struct _GtkRayPrivate
{
    GtkWidget parent_instance;
    guint width;
    guint height;
    gint tick;
    gboolean initialize;
};

GtkRay *gtk_ray_stage_new(void);

int gtk_ray_get_width(GtkRay *embed);

int gtk_ray_get_height(GtkRay *embed);

G_END_DECLS

#endif // GTK_RAY_H

