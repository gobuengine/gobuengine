// gobu_level_outliner.h
#ifndef GAPP_LEVEL_OUTLINER_H
#define GAPP_LEVEL_OUTLINER_H

#include "config.h"
#include "pixio/pixio.h"
#include "types/type_outliner_item.h"

G_BEGIN_DECLS

#define GAPP_TYPE_OUTLINER (gapp_outliner_get_type())
G_DECLARE_FINAL_TYPE(GappOutliner, gapp_outliner, GAPP, OUTLINER, GtkBox)

GappOutliner *gapp_outliner_new(void);

void gapp_outliner_set_name_entity(GappOutliner *outliner, ecs_entity_t entity, const char *name);
void gapp_outliner_append_item_children(GappOutliner *outliner, TOutlinerItem *item);

void gapp_outliner_append_item_root(GappOutliner *outliner, TOutlinerItem *item);
GListStore *gapp_outliner_get_store(GappOutliner *outliner);

void gapp_outliner_append_item_root(GappOutliner *outliner, TOutlinerItem *item);

TOutlinerItem *gapp_outliner_fn_find_item_by_entity(GtkSingleSelection *selection, ecs_entity_t entity);

GtkSingleSelection *gapp_outliner_get_selection(GappOutliner *outliner);

G_END_DECLS

#endif // GAPP_LEVEL_OUTLINER_H

