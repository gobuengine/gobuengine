// gapp_level_inspector.h
#ifndef GAPP_LEVEL_INSPECTOR_H
#define GAPP_LEVEL_INSPECTOR_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_INSPECTOR (gapp_inspector_get_type())
G_DECLARE_FINAL_TYPE(GappInspector, gapp_inspector, GAPP, INSPECTOR, GtkBox)

GappInspector *gapp_inspector_new(void);

void gapp_inspector_set_entity(GappInspector *self, ecs_world_t *world, ecs_entity_t entity);

G_END_DECLS

#endif // GAPP_LEVEL_INSPECTOR_H

