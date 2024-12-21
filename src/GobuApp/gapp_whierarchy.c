#include "gapp_whierarchy.h"
#include "pixio/pixio_type.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappWidgetHierarchy
{
    GtkBox parent_instance;
};

// MARK: CLASS
G_DEFINE_TYPE(GappWidgetHierarchy, gapp_widget_hierarchy, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappWidgetHierarchy *self = GAPP_WIDGET_HIERARCHY(object);
    G_OBJECT_CLASS(gapp_widget_hierarchy_parent_class)->dispose(object);
}

static void gapp_widget_hierarchy_class_init(GappWidgetHierarchyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_widget_hierarchy_init(GappWidgetHierarchy *self)
{
    gtk_widget_set_margin_start(GTK_WIDGET(self), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(self), 5);

    GtkWidget *search = gtk_search_entry_new();
    gtk_widget_set_hexpand(search, TRUE);
    gtk_box_append(GTK_BOX(self), search);
}

GappWidgetHierarchy *gapp_widget_hierarchy_new(void)
{
    return g_object_new(GAPP_WIDGET_TYPE_HIERARCHY, NULL);
}

// MARK: PUBLIC
