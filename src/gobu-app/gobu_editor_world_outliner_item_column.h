/**********************************************************************************
 * gobu_world_outliner_item_columns.h                                             *
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

#ifndef __GOBU_WORLD_OUTLINE_ITEM_COLUMN_H__
#define __GOBU_WORLD_OUTLINE_ITEM_COLUMN_H__
#include <gtk/gtk.h>

#define WORLD_OUTLINER_TYPE_ITEM_COLUMN (gobu_world_outline_item_column_get_type())
G_DECLARE_FINAL_TYPE(GobuWorldOutlineItemColumn, gobu_world_outline_item_column, WORLD_OUTLINER, ITEM_COLUMN, GObject)

struct _GobuWorldOutlineItemColumn
{
    GObject parent_instance;
    const char *name;
    gboolean visible;
    GListStore *children;
};

GobuWorldOutlineItemColumn *gobu_world_outline_item_column_new(const char *name, gboolean visible);

GListStore *gobu_world_outline_item_column_new_children(void);

const gchar *gobu_world_outline_item_column_get_name(GobuWorldOutlineItemColumn *item);

gboolean *gobu_world_outline_item_column_get_visible(GobuWorldOutlineItemColumn *item);

#endif // __GOBU_WORLD_OUTLINE_ITEM_COLUMN_H__

