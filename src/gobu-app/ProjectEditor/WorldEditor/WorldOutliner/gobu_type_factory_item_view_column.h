#ifndef __GOBU_TYPE_WORLD_OUTLINE_FACTORY_ITEM_VIEW_COLUMN_H__
#define __GOBU_TYPE_WORLD_OUTLINE_FACTORY_ITEM_VIEW_COLUMN_H__
#include <gtk/gtk.h>

#define GOBU_FACTORY_ITEM_TYPE_VIEW_COLUMN (gobu_factory_item_view_column_get_type())
G_DECLARE_FINAL_TYPE(GobuFactoryItemViewColumn, gobu_factory_item_view_column, FACTORY, ITEM, GObject)

struct _GobuFactoryItemViewColumn
{
    GObject parent_instance;
    const char *name;
    gboolean visible;
    GListStore *children;
};

GobuFactoryItemViewColumn *gobu_factory_item_view_column_new(const char *name, gboolean visible);

const gchar *gobu_factory_item_view_column_get_name(GobuFactoryItemViewColumn *item);

gboolean *gobu_factory_item_view_column_get_visible(GobuFactoryItemViewColumn *item);

#endif // __GOBU_TYPE_WORLD_OUTLINE_FACTORY_ITEM_VIEW_COLUMN_H__