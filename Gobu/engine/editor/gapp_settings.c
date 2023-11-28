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

    struct
    {
        GtkWidget* name;
        GtkWidget* description;
        GtkWidget* author;
    } game;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappSettings, gapp_settings, GTK_TYPE_WIDGET);

static GtkWidget* gapp_setting_game_info_new(GappSettings* self);
static GtkWidget* gapp_setting_display_new(GappSettings* self);

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

static void gapp_settings_init(GappSettings* self)
{
    GtkWidget* scroll, * vbox, * list_setting, * list_setting_props;

    // Lista de propiedades
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(scroll), 700, -1);
    gtk_widget_set_halign(scroll, GTK_ALIGN_CENTER);
    gapp_widget_set_margin(scroll, 10);
    gtk_widget_set_parent(scroll, GTK_WIDGET(self));
    {
        // Gtk box para la lista de propiedades
        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), vbox);

        GtkWidget* gameInfo = gapp_setting_game_info_new(self);
        gtk_box_append(vbox, gameInfo);

        GtkWidget* display = gapp_setting_display_new(self);
        gtk_box_append(vbox, display);
    }
}

GappSettings* gapp_settings_new(void)
{
    GappSettings* setting = g_object_new(GAPP_TYPE_SETTINGS, NULL);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 1, "World Settings", setting);

    return setting;
}

static GtkWidget* gapp_setting_game_info_new(GappSettings* self)
{
    GtkWidget* expand = gapp_widget_expander_with_widget("<b>Game Info</b>", NULL);
    {
        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_expander_set_child(GTK_EXPANDER(expand), box);

        // Nombre del juego
        self->game.name = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(self->game.name), "Game Name");
        gtk_widget_set_margin_top(self->game.name, 10);
        gtk_widget_set_hexpand(self->game.name, TRUE);
        gtk_box_append(GTK_BOX(box), self->game.name);

        // Description del juego
        GtkWidget* text_view_frame = gtk_frame_new("Description");
        gtk_widget_set_size_request(GTK_WIDGET(text_view_frame), -1, 100);
        gtk_widget_set_margin_top(text_view_frame, 10);
        gtk_box_append(GTK_BOX(box), text_view_frame);
        {
            self->game.description = gtk_text_view_new();
            gtk_frame_set_child(GTK_FRAME(text_view_frame), self->game.description);
            gapp_widget_set_margin(self->game.description, 10);
        }

        // Author del juego
        self->game.author = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(self->game.author), "Author");
        gtk_widget_set_margin_top(self->game.author, 10);
        gtk_widget_set_hexpand(self->game.author, TRUE);
        gtk_box_append(GTK_BOX(box), self->game.author);
    }

    return expand;
}

static GtkWidget* gapp_setting_display_new(GappSettings* self)
{
    GtkWidget* expand = gapp_widget_expander_with_widget("<b>Resolution and rendering</b>", NULL);
    {
        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_hexpand(box, TRUE);
        gtk_expander_set_child(GTK_EXPANDER(expand), box);

        // Resolucion common
        const gchar* resolution[] = { "Common Resolutions", "640x480", "800x600", "1024x768", "1280x720", "1280x1024", "1366x768", "1440x900", "1600x900", "1680x1050", "1920x1080", "1920x1200", "2560x1440", "2560x1600", "3840x2160", "4096x2160", NULL };

        GtkWidget* hbox_res = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
        gtk_box_append(GTK_BOX(box), hbox_res);
        {
            GtkWidget* combo = gtk_combo_box_text_new();
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
            gtk_widget_set_hexpand(combo, TRUE);
            gtk_widget_set_margin_top(combo, 10);
            gtk_widget_set_tooltip_text(combo, "Pick from a list of common screen resolutions");
            gtk_box_append(GTK_BOX(hbox_res), combo);

            // leemos el array de resoluciones
            for (int i = 0; resolution[i] != NULL; i++)
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), resolution[i]);

            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

            // button para invertir la resolucion
            GtkWidget* button = gapp_widget_button_new_icon_with_label("go-up-symbolic", NULL);
            gtk_button_set_has_frame(button, TRUE);
            gtk_widget_set_margin_top(button, 10);
            gtk_widget_set_tooltip_text(button, "Swap between portrait and landscape orientation");
            gtk_box_append(GTK_BOX(hbox_res), button);
        }

        // Resolucion custom
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_widget_set_hexpand(hbox, TRUE);
        gtk_widget_set_margin_top(hbox, 10);
        gtk_box_append(GTK_BOX(box), hbox);
        {
            GtkWidget* vbox;
            // vbox para un label y el spin button
            vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_set_tooltip_text(vbox, "The width of the new view port window in pixels (0 = use the desktop resolution)");
            gtk_box_append(GTK_BOX(hbox), vbox);
            {
                GtkWidget* label = gtk_label_new("Viewport width");
                gtk_widget_set_halign(label, GTK_ALIGN_START);
                gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
                gtk_widget_set_margin_end(label, 10);
                gtk_widget_set_margin_bottom(label, 5);
                gtk_box_append(GTK_BOX(vbox), label);

                GtkWidget* spin_width = gtk_spin_button_new_with_range(0, 10000, 1);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_width), 640);
                gtk_widget_set_hexpand(spin_width, TRUE);
                gtk_box_append(GTK_BOX(vbox), spin_width);
            }

            vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_set_tooltip_text(vbox, "The height of the new view port window in pixels (0 = use the desktop resolution)");
            gtk_box_append(GTK_BOX(hbox), vbox);
            {
                GtkWidget* label = gtk_label_new("Viewport height");
                gtk_widget_set_halign(label, GTK_ALIGN_START);
                gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
                gtk_widget_set_margin_end(label, 10);
                gtk_widget_set_margin_bottom(label, 5);
                gtk_box_append(GTK_BOX(vbox), label);

                GtkWidget* spin_height = gtk_spin_button_new_with_range(0, 10000, 1);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_height), 640);
                gtk_widget_set_hexpand(spin_height, TRUE);
                gtk_box_append(GTK_BOX(vbox), spin_height);
            }
        }

        // Fullscreen or windowed
        GtkWidget* check = gtk_check_button_new_with_label("Fullscreen");
        gtk_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(box), check);

        // Vsync
        check = gtk_check_button_new_with_label("Vsync");
        gtk_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(box), check);

    }

    return expand;
}
