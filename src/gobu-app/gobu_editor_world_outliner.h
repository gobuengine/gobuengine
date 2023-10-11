/**********************************************************************************
 * gobu_editor_world_outliner.h                                                   *
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

#pragma once
#ifndef __GOBU_EDITOR_WORLD_OUTLINER_H__
#define __GOBU_EDITOR_WORLD_OUTLINER_H__
#include "gobu_project_editor.h"

G_BEGIN_DECLS

#define GOBU_EDITOR_TYPE_WORLD_OUTLINER (gobu_editor_world_outliner_get_type())
G_DECLARE_FINAL_TYPE(GobuEditorWorldOutliner, gobu_editor_world_outliner, GOBU_EDITOR, WORLD_OUTLINER, GtkWidget)

GobuEditorWorldOutliner *gobu_editor_world_outliner_new(void);

void gobu_editor_world_outliner_append(GObject *parent, GObject *child);

G_END_DECLS

#endif // __GOBU_EDITOR_WORLD_OUTLINER_H__
