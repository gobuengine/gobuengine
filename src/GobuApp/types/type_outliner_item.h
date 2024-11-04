// type_outliner_item.h
#ifndef TYPE_OUTLINER_ITEM_H
#define TYPE_OUTLINER_ITEM_H

#include "../config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define TOUTLINER_TYPE_ITEM (toutliner_item_get_type())
G_DECLARE_FINAL_TYPE(TOutlinerItem, toutliner_item, TOUTLINER, ITEM, GObject)

TOutlinerItem *toutliner_item_new(ecs_world_t *world, ecs_entity_t entity);

void toutliner_item_set_entity(TOutlinerItem *self, ecs_entity_t entity);
ecs_entity_t toutliner_item_get_entity(TOutlinerItem *self);

void toutliner_item_set_name(TOutlinerItem *self, const gchar *name);
gchar *toutliner_item_get_name(TOutlinerItem *self);

void toutliner_item_set_expander(TOutlinerItem *self, GtkWidget *expander);
GtkWidget *toutliner_item_get_expander(TOutlinerItem *self);

void toutliner_item_set_children(TOutlinerItem *self, GListStore *children);
GListStore *toutliner_item_get_children(TOutlinerItem *self);

void toutliner_item_set_root(TOutlinerItem *self, GListStore *root);
GListStore *toutliner_item_get_root(TOutlinerItem *self);

G_END_DECLS

#endif // TYPE_OUTLINER_ITEM_H
