// gapp_viewport.h
#ifndef GOBU_VIEWPORT_H
#define GOBU_VIEWPORT_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_VIEWPORT (gapp_viewport_get_type())
G_DECLARE_FINAL_TYPE(GappViewport, gapp_viewport, GAPP, VIEWPORT, GtkGLArea)

GappViewport *gapp_viewport_new(void);
ecs_world_t *gapp_viewport_get_world(const GappViewport *self);
ecs_entity_t gapp_viewport_get_root(const GappViewport *self);

G_END_DECLS

#endif // GOBU_VIEWPORT_H
