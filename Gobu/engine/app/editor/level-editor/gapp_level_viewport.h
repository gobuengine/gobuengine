/**********************************************************************************
 * gapp_level_viewport.h                                                            *
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

#ifndef GAPP_LEVEL_EDITOR_VIEWPORT_H
#define GAPP_LEVEL_EDITOR_VIEWPORT_H
#include "gapp_level_editor.h"
#include "gtkray.h"

G_BEGIN_DECLS

typedef struct _GappLevelViewportPrivate GappLevelViewportPrivate;

#define GAPP_LEVEL_TYPE_VIEWPORT (gapp_level_viewport_get_type())
G_DECLARE_FINAL_TYPE(GappLevelViewport, gapp_level_viewport, GAPP_LEVEL, VIEWPORT, GtkRay)

struct _GappLevelViewportPrivate
{
    GtkWidget parent_instance;
};

GappLevelViewport* gapp_level_viewport_new(GappLevelEditor *editor);

G_END_DECLS

#endif // GAPP_LEVEL_EDITOR_VIEWPORT_H

