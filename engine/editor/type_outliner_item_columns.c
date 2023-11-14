#include "type_outliner_item_columns.h"

struct _GappOutlineItemColumnClass
{
    GObjectClass parent_class;
};

G_DEFINE_TYPE(GappOutlineItemColumn, gapp_outline_item_column, G_TYPE_OBJECT)

static void gapp_outline_item_column_init(GappOutlineItemColumn *item) {}
static void gapp_outline_item_column_class_init(GappOutlineItemColumnClass *item) {}

GappOutlineItemColumn* gapp_outline_item_column_new(const char* name, uint64_t entity, gboolean visible)
{
    GappOutlineItemColumn *item = g_object_new(OUTLINER_TYPE_ITEM_COLUMN, NULL);
    item->name = name;
    item->visible = visible;
    item->entity = entity;
    item->children = g_list_store_new(OUTLINER_TYPE_ITEM_COLUMN);
    return item;
}

GListStore *gapp_outline_item_column_new_children(void)
{
    return g_list_store_new(OUTLINER_TYPE_ITEM_COLUMN);
}

const gchar *gapp_outline_item_column_get_name(GappOutlineItemColumn *item)
{
    return item->name;
}

gboolean *gapp_outline_item_column_get_visible(GappOutlineItemColumn *item)
{
    return item->visible;
}
