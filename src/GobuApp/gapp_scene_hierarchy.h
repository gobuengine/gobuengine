// gapp_scene_hierarchy.h
#ifndef GAPP_SCENE_HIERARCHY_H
#define GAPP_SCENE_HIERARCHY_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_SCENE_TYPE_HIERARCHY (gapp_scene_hierarchy_get_type())
G_DECLARE_FINAL_TYPE(GappSceneHierarchy, gapp_scene_hierarchy, GAPP_SCENE, HIERARCHY, GtkBox)

GappSceneHierarchy *gapp_scene_hierarchy_new(void);

G_END_DECLS

#endif // GAPP_SCENE_HIERARCHY_H

