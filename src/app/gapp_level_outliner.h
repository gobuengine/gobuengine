// gobu_level_outliner.h
#ifndef GAPP_LEVEL_OUTLINER_H
#define GAPP_LEVEL_OUTLINER_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_OUTLINER (gapp_outliner_get_type())
G_DECLARE_FINAL_TYPE(GappOutliner, gapp_outliner, GAPP, OUTLINER, GtkBox)

GappOutliner *gapp_outliner_new(void);

void gapp_outliner_start_process(GappOutliner *outliner, ecs_world_t *world);

void gapp_outliner_set_name_entity(GappOutliner *outliner, ecs_entity_t entity, const char *name);
void gapp_outliner_append_item_children(GappOutliner *outliner, OutlinerItem *item);

void gapp_outliner_append_item_root(GappOutliner *outliner, OutlinerItem *item);
GListStore *gapp_outliner_get_store(GappOutliner *outliner);

G_END_DECLS

#endif // GAPP_LEVEL_OUTLINER_H
