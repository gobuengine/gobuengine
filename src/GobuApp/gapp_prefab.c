#include "gapp_prefab.h"
#include "pixio/pixio_type.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappPrefab
{
    GtkBox parent_instance;
};

// MARK: CLASS
G_DEFINE_TYPE(GappPrefab, gapp_prefab, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappPrefab *self = GAPP_PREFAB(object);
    G_OBJECT_CLASS(gapp_prefab_parent_class)->dispose(object);
}

static void gapp_prefab_class_init(GappPrefabClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_prefab_init(GappPrefab *self)
{
    gtk_widget_set_margin_start(GTK_WIDGET(self), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(self), 5);

    GtkWidget *boxtoolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(boxtoolbar, GTK_ALIGN_END);
    gtk_widget_set_hexpand(boxtoolbar, TRUE);
    gtk_box_append(GTK_BOX(self), boxtoolbar);
    {
        GtkWidget *button2 = gtk_button_new_from_icon_name("list-add-symbolic");
        gtk_button_set_has_frame(GTK_BUTTON(button2), FALSE);
        gtk_widget_add_css_class(button2, "expander_button");
        gtk_box_append(GTK_BOX(boxtoolbar), button2);
    }

    gtk_box_append(GTK_BOX(self), gapp_widget_separator_h());

    GtkWidget *search = gtk_search_entry_new();
    gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search");
    gtk_widget_set_hexpand(search, TRUE);
    gtk_box_append(GTK_BOX(self), search);
}

GappPrefab *gapp_prefab_new(void)
{
    return g_object_new(GAPP_TYPE_PREFAB,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        "spacing", 5, NULL);
}

// MARK: PUBLIC
