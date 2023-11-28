#include "gapp_settings.h"
#include "gapp_main.h"
#include "gapp_widget.h"
#include "gobu_project.h"
#include "gobu_utility.h"

extern GAPP* EditorCore;

struct _GappSettings
{
    GtkWidget parent;
    GtkWidget* paned;
    GtkSizeGroup* group_size;
    GtkWidget* setting_props;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappSettings, gapp_settings, GTK_TYPE_WIDGET);

static void gapp_settings_class_finalize(GObject* object)
{
    GappSettings* setting = GAPP_SETTINGS(object);

    g_clear_pointer(&setting->paned, gtk_widget_unparent);

    G_OBJECT_CLASS(gapp_settings_parent_class)->finalize(object);
}


static void gapp_settings_class_init(GappSettingsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(klass);

    object_class->finalize = gapp_settings_class_finalize;

    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

//  -------------------------

static void dir_dir(binn* object, GtkWidget* widget, GappSettings* props)
{
    binn value;
    binn_iter iter;
    char key[256];

    binn_object_foreach(object, key, value)
    {
        if (binn_type(&value) == BINN_OBJECT)
        {
            GtkWidget* group = gapp_widget_expander_with_widget(key, NULL);
            {
                GtkWidget* row = gtk_list_box_row_new();
                gtk_list_box_row_set_selectable(row, FALSE);
                gtk_list_box_row_set_activatable(row, FALSE);
                gtk_list_box_append(GTK_LIST_BOX(widget), row);
                gtk_list_box_row_set_child(row, group);

                GtkWidget* list = gtk_list_box_new();
                gtk_expander_set_child(group, list);
                dir_dir(&value, list, props);
            }
        }
        else {
            GtkWidget* row = gtk_list_box_row_new();
            gtk_list_box_row_set_selectable(row, FALSE);
            gtk_list_box_append(GTK_LIST_BOX(widget), row);

            GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
            gtk_list_box_row_set_child(row, box);

            GtkWidget* label = gtk_label_new(gb_strdups("<span><b>%s</b></span>",key));
            gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
            gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
            gtk_widget_set_margin_end(label, 20);
            gtk_widget_set_halign(label, GTK_ALIGN_START);
            gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
            gtk_box_append(box, label);
            gtk_size_group_add_widget(GTK_SIZE_GROUP(props->group_size), label);

            // Tipos de datos a widgets
            switch (binn_type(&value))
            {
            case BINN_STRING:
                GtkWidget* entry = gtk_entry_new();
                gtk_widget_set_hexpand(entry, TRUE);
                GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
                gtk_entry_buffer_set_text(buffer, binn_object_str(object, key), -1);
                gtk_box_append(box, entry);
                break;

            case BINN_INT32:
            case BINN_INT64:
            case BINN_UINT8:
            case BINN_UINT16:
            case BINN_UINT32:
            case BINN_UINT64:
                GtkWidget* spin_int = gtk_spin_button_new_with_range(0, 9999, 1);
                gtk_widget_set_hexpand(spin_int, TRUE);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_int), binn_object_int32(object, key));
                gtk_box_append(box, spin_int);
                break;

            case BINN_BOOL:
                GtkWidget* check = gtk_check_button_new();
                gtk_widget_set_hexpand(check, TRUE);
                gtk_check_button_set_active(GTK_CHECK_BUTTON(check), binn_object_bool(object, key));
                gtk_box_append(box, check);
                break;

            case BINN_FLOAT32:
            case BINN_FLOAT64:
                GtkWidget* spin_float = gtk_spin_button_new_with_range(0.0f, 9999, 1.0f);
                gtk_widget_set_hexpand(spin_float, TRUE);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_float), binn_object_double(object, key));
                gtk_box_append(box, spin_float);
                break;
            }
        }
    }
}

static void gapp_settings_on_list_setting_activate(GtkListBox* self, GtkListBoxRow* row_parent, GappSettings* props)
{
    gchar* keys = g_object_get_data(G_OBJECT(gtk_list_box_row_get_child(row_parent)), "property_data");

    binn* setting_game = gb_project_get_setting();
    binn* object = binn_object_object(setting_game, keys);

    gtk_list_box_remove_all(GTK_LIST_BOX(props->setting_props));
    dir_dir(object, GTK_LIST_BOX(props->setting_props), props);
}

static void gapp_settings_init(GappSettings* self)
{
    GtkWidget* scroll, * vbox, * list_setting, * list_setting_props;

    self->paned = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
    gtk_widget_set_hexpand(self->paned, TRUE);
    gtk_widget_set_vexpand(self->paned, TRUE);
    gtk_widget_set_parent(self->paned, GTK_WIDGET(self));
    {
        // Lista de propiedades
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(scroll), 250, -1);
        gapp_widget_set_margin(scroll, 10);
        gtk_paned_set_start_child(GTK_PANED(self->paned), scroll);
        {
            list_setting = gtk_list_box_new();
            gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_setting), GTK_SELECTION_SINGLE);
            gtk_widget_add_css_class(list_setting, "navigation-sidebar");

            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_setting);
            {
                binn_iter iter;
                binn value;
                char key[256];
                binn* setting_game = gb_project_get_setting();
                binn_object_foreach(setting_game, key, value)
                {
                    GtkWidget* row = gtk_list_box_row_new();
                    // g_signal_connect(row, "activate", G_CALLBACK(gapp_settings_on_list_setting_activate), &value);
                    gtk_list_box_append(GTK_LIST_BOX(list_setting), row);
                    {
                        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                        g_object_set_data(box, "property_data", gb_strdup(key));
                        gtk_list_box_row_set_child(row, box);

                        GtkWidget* image = gtk_image_new_from_icon_name("view-list-symbolic");
                        gtk_box_append(GTK_BOX(box), image);

                        GtkWidget* label = gtk_label_new(key);
                        gtk_widget_set_margin_start(label, 10);
                        gtk_widget_set_halign(label, GTK_ALIGN_START);
                        gtk_box_append(GTK_BOX(box), label);
                    }
                }
            }
        }
    }

    // Propiedades de la lista seleccionada
    {
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gapp_widget_set_margin(scroll, 10);
        gtk_paned_set_end_child(GTK_PANED(self->paned), scroll);
        {
            self->setting_props = gtk_list_box_new();
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->setting_props);

            self->group_size = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
        }
    }

    g_signal_connect(list_setting, "row-activated", G_CALLBACK(gapp_settings_on_list_setting_activate), self);
}

GappSettings* gapp_settings_new(void)
{
    GappSettings* setting = g_object_new(GAPP_TYPE_SETTINGS, NULL);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 1, "World Settings", setting);

    return setting;
}

