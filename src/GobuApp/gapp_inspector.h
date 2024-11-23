// gapp_inspector.h
#ifndef GAPP_INSPECTOR_H
#define GAPP_INSPECTOR_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_INSPECTOR (gapp_inspector_get_type())
G_DECLARE_FINAL_TYPE(GappInspector, gapp_inspector, GAPP, INSPECTOR, GtkBox)

GappInspector *gapp_inspector_new(void);

void inspectorSetEntity(GappInspector *self, ecs_world_t *world, ecs_entity_t entity);
void inspectorSetEmpty(GappInspector *self, const gchar *message);

G_END_DECLS

#endif // GAPP_INSPECTOR_H

