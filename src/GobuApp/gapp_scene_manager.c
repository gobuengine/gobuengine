#include "gapp_scene_manager.h"
#include "pixio/pixio.h"
#include "gapp_common.h"
#include "gapp_whierarchy.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappSceneManager
{
    GtkBox parent_instance;
};

// MARK: CLASS
G_DEFINE_TYPE(GappSceneManager, gapp_scene_manager, GTK_TYPE_WINDOW)

static void object_class_dispose(GObject *object)
{
    GappSceneManager *self = GAPP_SCENE_MANAGER(object);
    G_OBJECT_CLASS(gapp_scene_manager_parent_class)->dispose(object);
}

static void gapp_scene_manager_class_init(GappSceneManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_scene_manager_init(GappSceneManager *self)
{
    gtk_window_set_modal(GTK_WINDOW(self), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(self), 600, 400);
    gtk_window_set_resizable(GTK_WINDOW(self), FALSE);
}

// -----------------
// MARK:PRIVATE
// -----------------

// -----------------
// MARK:SIGNAL
// -----------------

// -----------------
// MARK: PUBLIC
// -----------------

GappSceneManager *gapp_scene_manager_new(void)
{
    return g_object_new(GAPP_SCENE_TYPE_MANAGER, "title", "Scene Manager", NULL);
}

// MARK: PUBLIC
