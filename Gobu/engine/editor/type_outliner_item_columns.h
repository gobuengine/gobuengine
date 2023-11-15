/**********************************************************************************
 * type_outliner_item_columns.h                                                   *
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

#ifndef __GOBU_OUTLINE_ITEM_COLUMN_H__
#define __GOBU_OUTLINE_ITEM_COLUMN_H__
#include <gtk/gtk.h>
#include <stdint.h>
#include "gobu_gobu.h"

G_BEGIN_DECLS

#define OUTLINER_TYPE_ITEM_COLUMN (gapp_outline_item_column_get_type())
G_DECLARE_FINAL_TYPE(GappOutlineItemColumn, gapp_outline_item_column, OUTLINER, ITEM_COLUMN, GObject)

struct _GappOutlineItemColumn
{
    GObject parent_instance;

    const char* name;
    gboolean visible;
    uint64_t entity;
    GListStore* children;
    GListStore *parent;
    // copy world (Experimental)
    ecs_world_t* world;
};

GappOutlineItemColumn* gapp_outline_item_column_new(const char* name, uint64_t entity, gboolean visible);

const gchar* gapp_outline_item_column_get_name(GappOutlineItemColumn* item);

gboolean* gapp_outline_item_column_get_visible(GappOutlineItemColumn* item);

G_END_DECLS

#endif // __GOBU_OUTLINE_ITEM_COLUMN_H__

