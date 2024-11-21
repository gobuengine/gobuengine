// gapp_scene.h
#ifndef GAPP_SCENE_H
#define GAPP_SCENE_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_SCENE (gapp_scene_get_type())
G_DECLARE_FINAL_TYPE(GappScene, gapp_scene, GAPP, SCENE, GtkBox)

GappScene *gapp_scene_new(void);

G_END_DECLS

#endif // GAPP_SCENE_H

