// gapp_inspector_entity.h
#ifndef GAPP_INSPECTOR_ENTITY_H
#define GAPP_INSPECTOR_ENTITY_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_INSPECTOR_ENTITY (gapp_inspector_entity_get_type())
G_DECLARE_FINAL_TYPE(GappInspectorEntity, gapp_inspector_entity, GAPP, INSPECTOR_ENTITY, GtkBox)

GappInspectorEntity *gapp_inspector_entity_new(void);

void gapp_inspector_entity_set_entity(GappInspectorEntity *self, ecs_world_t *world, ecs_entity_t entity);

G_END_DECLS

#endif // GAPP_INSPECTOR_ENTITY_H

