// gapp_scene_manager.h
#ifndef GAPP_SCENE_MANAGER_H
#define GAPP_SCENE_MANAGER_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_SCENE_TYPE_MANAGER (gapp_scene_manager_get_type())
G_DECLARE_FINAL_TYPE(GappSceneManager, gapp_scene_manager, GAPP_SCENE, MANAGER, GtkWindow)

GappSceneManager *gapp_scene_manager_new(void);

G_END_DECLS

#endif // GAPP_SCENE_MANAGER_H

