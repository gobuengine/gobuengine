// gapp_itype_enum.h
#ifndef GAPP_OUTLINER_ITEM_H
#define GAPP_OUTLINER_ITEM_H

#include "../config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define TOUTLINER_TYPE_ITEM (toutliner_item_get_type())
G_DECLARE_FINAL_TYPE(TOutlinerItem, toutliner_item, OUTLINER, ITEM, GObject)

TOutlinerItem *toutliner_item_new(ecs_world_t *world, ecs_entity_t entity);

void toutliner_item_set_name(TOutlinerItem *self, const gchar *name);

const gchar *toutliner_item_get_name(TOutlinerItem *self);

ecs_entity_t toutliner_item_get_entity(TOutlinerItem *self);

GListStore *toutliner_item_get_children(TOutlinerItem *self);

void toutliner_item_set_root(TOutlinerItem *self, GListStore *root);

GListStore *toutliner_item_get_root(TOutlinerItem *self);

GtkWidget *toutliner_item_get_expander(TOutlinerItem *self);

G_END_DECLS

#endif // GAPP_OUTLINER_ITEM_H

