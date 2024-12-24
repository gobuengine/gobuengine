// gapp_scene_viewport.h
#ifndef GAPP_SCENE_VIEWPORT_H
#define GAPP_SCENE_VIEWPORT_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_SCENE_VIEWPORT (gapp_scene_viewport_get_type())
G_DECLARE_FINAL_TYPE(GappSceneViewport, gapp_scene_viewport, GAPP, SCENE_VIEWPORT, GtkBox)

GappSceneViewport *gapp_scene_viewport_new(void);

G_END_DECLS

#endif // GAPP_SCENE_VIEWPORT_H

