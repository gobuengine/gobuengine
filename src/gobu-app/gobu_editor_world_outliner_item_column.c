#include "gobu_editor_world_outliner_item_column.h"

struct _GobuWorldOutlineItemColumnClass
{
    GObjectClass parent_class;
};

G_DEFINE_TYPE(GobuWorldOutlineItemColumn, gobu_world_outline_item_column, G_TYPE_OBJECT)

static void gobu_world_outline_item_column_init(GobuWorldOutlineItemColumn *item) {}
static void gobu_world_outline_item_column_class_init(GobuWorldOutlineItemColumnClass *item) {}

GobuWorldOutlineItemColumn *gobu_world_outline_item_column_new(const char *name, gboolean visible)
{
    GobuWorldOutlineItemColumn *item;
    item = g_object_new(WORLD_OUTLINER_TYPE_ITEM_COLUMN, NULL);
    item->name = name;
    item->visible = visible;
    return item;
}

GListStore *gobu_world_outline_item_column_new_children(void)
{
    return g_list_store_new(WORLD_OUTLINER_TYPE_ITEM_COLUMN);
}

const gchar *gobu_world_outline_item_column_get_name(GobuWorldOutlineItemColumn *item)
{
    return item->name;
}

gboolean *gobu_world_outline_item_column_get_visible(GobuWorldOutlineItemColumn *item)
{
    return item->visible;
}
