#include "gapp_level_inspector.h"
#include "gapp_main.h"

struct _GappLevelInspector
{
    GtkWidget parent;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelInspector, gapp_level_inspector, GTK_TYPE_BOX);

static void gapp_level_inspector_class_init(GappLevelInspectorClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

//  -------------------------

static void gapp_level_inspector_init(GappLevelInspector* self)
{
    GtkWidget* scroll, * vbox;

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(scroll), -1, 100);
    gtk_box_append(self, scroll);
    {
        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_hexpand(vbox, TRUE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), vbox);
    }
}

GappLevelInspector* gapp_level_inspector_new(void)
{
    return g_object_new(GAPP_LEVEL_TYPE_INSPECTOR, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}

