#include "gapp_scene.h"
#include "pixio/pixio_type.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_wviewport.h"
#include "gapp.h"

struct _GappScene
{
    GtkBox parent_instance;
};

// MARK: CLASS
G_DEFINE_TYPE(GappScene, gapp_scene, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappScene *self = GAPP_SCENE(object);
    G_OBJECT_CLASS(gapp_scene_parent_class)->dispose(object);
}

static void gapp_scene_class_init(GappSceneClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_scene_init(GappScene *self)
{
    GtkWidget *viewport = gapp_widget_viewport_new();
    gtk_box_append(GTK_BOX(self), viewport);
}

GappScene *gapp_scene_new(void)
{
    return g_object_new(GAPP_TYPE_SCENE, NULL);
}

// MARK: PUBLIC

