#include "gbapp_asheets.h"
#include "gapp_gobu_embed.h"

struct _GbAppAsheets
{
    GtkWidget parent;
    gchar* filename;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GbAppAsheets, gbapp_asheets, GTK_TYPE_BOX);

static void gbapp_asheets_class_init(GbAppAsheetsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gbapp_asheets_init(GbAppAsheets* self)
{
    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);
}

static GbAppAsheets* gbapp_asheets_template(GbAppAsheets* self)
{
    GtkWidget* scroll, * vbox_anim, * hbox, * label, * button, * icon, * viewport, * toolbar, * paned, * paned2;

    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);

    toolbar = gapp_widget_toolbar_new();
    gtk_box_append(self, toolbar);
    {
        GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
        // g_signal_connect(btn_s, "clicked", G_CALLBACK(signal_toolbar_click_save_level), self);
        gtk_box_append(toolbar, btn_s);
    }

    paned = gapp_widget_paned_new(GTK_ORIENTATION_VERTICAL, TRUE);
    gtk_box_append(self, paned);
    {
        viewport = gapp_gobu_embed_new();
        gtk_paned_set_start_child(GTK_PANED(paned), viewport);
        {

        }

        paned2 = gapp_widget_paned_new_with_notebook(GTK_ORIENTATION_HORIZONTAL, TRUE,
            gtk_label_new("Animation"), gtk_label_new("Animation List"),
            gtk_label_new("Frames"), gtk_label_new("Frames icons"));
        gtk_paned_set_end_child(GTK_PANED(paned), paned2);

        // scroll = gtk_scrolled_window_new();
        // gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        // {
        //     paned2 = gapp_widget_paned_new_with_notebook(GTK_ORIENTATION_VERTICAL, TRUE,
        //         gtk_label_new("Animation"), gtk_label_new("Animation List"),
        //         gtk_label_new("Frames"), gtk_label_new("Frames icons"));
        //     gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), paned2);
        // }
    }

    return self;
}

/**
 * Crea un nuevo Animation Sprite Sheets.
 *
 * @return Un nuevo widget que Animation Sprite Sheets.
 */
GtkWidget* gbapp_asheets_new(const gchar* filename)
{
    GbAppAsheets* self = g_object_new(GBAPP_TYPE_ASHEETS, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    self->filename = gb_strdup(filename);
    char* name = gb_fs_get_name(filename, false);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 0, name, gbapp_asheets_template(self));

    return self;
}

