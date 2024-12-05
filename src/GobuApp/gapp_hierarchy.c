#include "gapp_hierarchy.h"
#include "pixio/pixio_type.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappHierarchy
{
    GtkBox parent_instance;
};

// MARK: CLASS
G_DEFINE_TYPE(GappHierarchy, gapp_hierarchy, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappHierarchy *self = GAPP_HIERARCHY(object);
    G_OBJECT_CLASS(gapp_hierarchy_parent_class)->dispose(object);
}

static void gapp_hierarchy_class_init(GappHierarchyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_hierarchy_init(GappHierarchy *self)
{
}

GappHierarchy *gapp_hierarchy_new(void)
{
    return g_object_new(GAPP_TYPE_HIERARCHY, NULL);
}

// MARK: PUBLIC

