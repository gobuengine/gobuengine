#include <stdint.h>
#include "gapp-ps-view-display.h"

#include "gapp_widget.h"
#include "gb_setting_project.h"
#include "utility/gb_path.h"
#include "utility/gb_string.h"

typedef enum _GappProjectSettingsDisplay
{
    GAPP_PROJECT_SETTINGS_DISPLAY_RESOLUTION_COMMON = 0,
    GAPP_PROJECT_SETTINGS_DISPLAY_WIDTH,
    GAPP_PROJECT_SETTINGS_DISPLAY_HEIGHT,
    GAPP_PROJECT_SETTINGS_DISPLAY_MODE,
    GAPP_PROJECT_SETTINGS_DISPLAY_RESIZABLE,
    GAPP_PROJECT_SETTINGS_DISPLAY_BORDERLESS,
    GAPP_PROJECT_SETTINGS_DISPLAY_ALWAYONTOP,
    GAPP_PROJECT_SETTINGS_DISPLAY_TRANSPARENT,
    GAPP_PROJECT_SETTINGS_DISPLAY_NOFOCUS,
    GAPP_PROJECT_SETTINGS_DISPLAY_HIGHDPI,
    GAPP_PROJECT_SETTINGS_DISPLAY_VSYNC,
    GAPP_PROJECT_SETTINGS_DISPLAY_ASPECTRATION
} GappProjectSettingsDisplay;

const char *const *resolution[] = {"Common Resolutions", "640x480", "800x600", "1024x768", "1280x720", "1280x1024", "1366x768 ( Generic 14-15.6\" Notebook )", "1440x900 (19\" Monitor:16:10 )", "1600x900 (20\" Monitor:16:9 )", "1680x1050 (22\" Monitor:16:10 )", "1920x1080 (21.5-24\" Monitor:16:9 )", "1920x1200", "2560x1440 (27\" Monitor:16:9 )", "2560x1600", "3840x2160", "4096x2160", NULL};

static void signal_toggle_input(GtkCheckButton *button, GappProjectSettingsDisplay type);
static void signal_activate_selected(GtkDropDown *self, GParamSpec *pspec, GappProjectSettingsPrivate *setting);
static void signal_spin_input(GtkSpinButton *self, GappProjectSettingsDisplay type);
static void signal_clicked_resolution_orientation_toggle(GtkButton *button, GappProjectSettingsPrivate *self);

GtkWidget *gapp_project_setting_display_new(GappProjectSettingsPrivate *self)
{
    GtkWidget *dropdown,
        *button, *check, *hbox, *spin, *label, *vbox,
        *listbox, *viewport, *scroll;

    scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);

    viewport = gtk_viewport_new(NULL, NULL);
    gtk_scrolled_window_set_child(scroll, viewport);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gapp_widget_set_margin_start(vbox, 60);
    gapp_widget_set_margin_end(vbox, 60);
    gapp_widget_set_margin_top(vbox, 30);
    gapp_widget_set_margin_bottom(vbox, 30);
    gtk_viewport_set_child(viewport, vbox);

    label = gtk_label_new("<b>Display</b>");
    gtk_label_set_use_markup(label, TRUE);
    gtk_label_set_xalign(label, 0);
    gapp_widget_set_margin_bottom(label, 10);
    gtk_widget_add_css_class(label, "title-2");
    gtk_box_append(GTK_BOX(vbox), label);

    listbox = gtk_list_box_new();
    gtk_list_box_set_selection_mode(listbox, GTK_SELECTION_NONE);
    gtk_widget_add_css_class(listbox, "rich-list");
    gtk_widget_add_css_class(listbox, "boxed-list");
    gtk_box_append(GTK_BOX(vbox), listbox);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    project_setting_group_child_append("Common Resolutions", listbox, hbox, GTK_ORIENTATION_VERTICAL);
    {
        dropdown = gtk_drop_down_new_from_strings(resolution);
        gtk_drop_down_set_show_arrow(dropdown, TRUE);
        gtk_drop_down_set_selected(dropdown, 0);
        gapp_widget_set_margin_top(dropdown, 10);
        gtk_widget_set_tooltip_text(dropdown, "Pick from a list of common screen resolutions");
        gtk_widget_set_hexpand(dropdown, TRUE);
        gtk_box_append(GTK_BOX(hbox), dropdown);
        g_object_set_data(dropdown, "type", GINT_TO_POINTER(GAPP_PROJECT_SETTINGS_DISPLAY_RESOLUTION_COMMON));
        g_signal_connect(dropdown, "notify::selected", G_CALLBACK(signal_activate_selected), self);

        // button para invertir la resolucion
        button = gapp_widget_button_new_icon_with_label("go-up-symbolic", NULL);
        gtk_button_set_has_frame(button, TRUE);
        gapp_widget_set_margin_top(button, 10);
        gtk_widget_set_tooltip_text(button, "Swap between portrait and landscape orientation");
        gtk_box_append(GTK_BOX(hbox), button);
        g_signal_connect(button, "clicked", G_CALLBACK(signal_clicked_resolution_orientation_toggle), self);
    }

    // Resolucion custom
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_hexpand(hbox, TRUE);
    gapp_widget_set_margin_top(hbox, 10);
    project_setting_group_child_append("Resolutions", listbox, hbox, GTK_ORIENTATION_VERTICAL);
    {
        // vbox para un label y el spin button
        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_tooltip_text(vbox, "The width of the new view port window in pixels (0 = use the desktop resolution)");
        gtk_box_append(GTK_BOX(hbox), vbox);
        {
            label = gtk_label_new("Viewport width");
            gtk_widget_set_halign(label, GTK_ALIGN_START);
            gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
            gtk_widget_set_margin_end(label, 10);
            gtk_widget_set_margin_bottom(label, 5);
            gtk_box_append(GTK_BOX(vbox), label);

            self->display_width = gtk_spin_button_new_with_range(0, INTMAX_MAX, 1);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->display_width), gb_setting_project_width());
            gtk_widget_set_hexpand(self->display_width, TRUE);
            gtk_box_append(GTK_BOX(vbox), self->display_width);
            g_signal_connect(self->display_width, "value-changed", G_CALLBACK(signal_spin_input), GAPP_PROJECT_SETTINGS_DISPLAY_WIDTH);
        }

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_tooltip_text(vbox, "The height of the new view port window in pixels (0 = use the desktop resolution)");
        gtk_box_append(GTK_BOX(hbox), vbox);
        {
            label = gtk_label_new("Viewport height");
            gtk_widget_set_halign(label, GTK_ALIGN_START);
            gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
            gtk_widget_set_margin_end(label, 10);
            gtk_widget_set_margin_bottom(label, 5);
            gtk_box_append(GTK_BOX(vbox), label);

            self->display_height = gtk_spin_button_new_with_range(0, INTMAX_MAX, 1);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->display_height), gb_setting_project_height());
            gtk_widget_set_hexpand(self->display_height, TRUE);
            gtk_box_append(GTK_BOX(vbox), self->display_height);
            g_signal_connect(self->display_height, "value-changed", G_CALLBACK(signal_spin_input), GAPP_PROJECT_SETTINGS_DISPLAY_HEIGHT);
        }
    }

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(vbox, TRUE);
    project_setting_group_child_append("Window", listbox, vbox, GTK_ORIENTATION_VERTICAL);
    {
        // Mode Window dropdown box [Windowed, Minimized, Maximized, Fullscreen]
        const char *const *mode[] = {"Windowed", "Minimized", "Maximized", "Fullscreen", NULL};
        dropdown = gtk_drop_down_new_from_strings(mode);
        gtk_drop_down_set_show_arrow(dropdown, TRUE);
        gtk_drop_down_set_selected(dropdown, gb_setting_project_mode());
        gapp_widget_set_margin_top(dropdown, 10);
        gtk_widget_set_tooltip_text(dropdown, "The initial window mode");
        gtk_box_append(GTK_BOX(vbox), dropdown);
        g_object_set_data(dropdown, "type", GINT_TO_POINTER(GAPP_PROJECT_SETTINGS_DISPLAY_MODE));
        g_signal_connect(dropdown, "notify::selected", G_CALLBACK(signal_activate_selected), self);

        check = gtk_check_button_new_with_label("Allow Window Resize");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_resizable());
        gapp_widget_set_margin_top(check, 10);
        gtk_widget_set_tooltip_text(check, "Should the user be allowed to resize the window used by the game, where not using full screen");
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_RESIZABLE);

        check = gtk_check_button_new_with_label("Use Borderless Window");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_borderless());
        gapp_widget_set_margin_top(check, 10);
        gtk_widget_set_tooltip_text(check, "Should the game use a borderless Slate window instead of a window with system title bar and border");
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_BORDERLESS);

        check = gtk_check_button_new_with_label("Always on Top");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_alwayontop());
        gapp_widget_set_margin_top(check, 10);
        gtk_widget_set_tooltip_text(check, "Should the game use a borderless Slate window instead of a window with system title bar and border");
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_ALWAYONTOP);

        check = gtk_check_button_new_with_label("Transparent");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_transparent());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_TRANSPARENT);

        check = gtk_check_button_new_with_label("No Focus");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_nofocus());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_NOFOCUS);

        check = gtk_check_button_new_with_label("Allow high DPI");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_highdpi());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_HIGHDPI);

        check = gtk_check_button_new_with_label("VSync");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_vsync());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_VSYNC);

        check = gtk_check_button_new_with_label("Should Window Preserve Aspect Ratio");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_setting_project_aspectration());
        gapp_widget_set_margin_top(check, 10);
        gtk_widget_set_tooltip_text(check, "Should the game's window preserve its aspect ration when resized by user");
        gtk_box_append(GTK_BOX(vbox), check);
        g_signal_connect(check, "toggled", G_CALLBACK(signal_toggle_input), GAPP_PROJECT_SETTINGS_DISPLAY_ASPECTRATION);
    }

    return scroll;
}

static void signal_toggle_input(GtkCheckButton *button, GappProjectSettingsDisplay type)
{
    gboolean active = gtk_check_button_get_active(button);


    switch (type)
    {
    case GAPP_PROJECT_SETTINGS_DISPLAY_RESIZABLE:
        gb_setting_project_set_resizable(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_BORDERLESS:
        gb_setting_project_set_borderless(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_ALWAYONTOP:
        gb_setting_project_set_alwayontop(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_TRANSPARENT:
        gb_setting_project_set_transparent(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_NOFOCUS:
        gb_setting_project_set_nofocus(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_HIGHDPI:
        gb_setting_project_set_highdpi(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_VSYNC:
        gb_setting_project_set_vsync(active);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_ASPECTRATION:
        gb_setting_project_set_aspectration(active);
        break;
    default:
        break;
    }

    gb_setting_project_save();
}

static void signal_activate_selected(GtkDropDown *self, GParamSpec *pspec, GappProjectSettingsPrivate *setting)
{
    GappProjectSettingsDisplay type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(self), "type"));
    int selected = gtk_drop_down_get_selected(self);

    switch (type)
    {
    case GAPP_PROJECT_SETTINGS_DISPLAY_MODE:
        gb_setting_project_set_mode(selected);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_RESOLUTION_COMMON:
        if (selected > 0)
        {
            char **sep1 = gb_str_split(resolution[selected], " ");
            char **sep2 = gb_str_split(sep1[0], "x");
            int width = atoi(sep2[0]);
            int height = atoi(sep2[1]);
            gb_str_split_free(sep1);
            gb_str_split_free(sep2);

            gb_setting_project_set_width(width);
            gb_setting_project_set_height(height);

            gtk_spin_button_set_value(GTK_SPIN_BUTTON(setting->display_width), width);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(setting->display_height), height);
        }
        break;
    default:
        break;
    }

    gb_setting_project_save();
}

static void signal_spin_input(GtkSpinButton *self, GappProjectSettingsDisplay type)
{
    gdouble value = gtk_spin_button_get_value(self);

    switch (type)
    {
    case GAPP_PROJECT_SETTINGS_DISPLAY_WIDTH:
        gb_setting_project_set_width((int)value);
        break;
    case GAPP_PROJECT_SETTINGS_DISPLAY_HEIGHT:
        gb_setting_project_set_height((int)value);
        break;
    default:
        break;
    }

    gb_setting_project_save();
}

static void signal_clicked_resolution_orientation_toggle(GtkButton *button, GappProjectSettingsPrivate *self)
{
    int width = gb_setting_project_width();
    int height = gb_setting_project_height();

    gb_setting_project_set_width(height);
    gb_setting_project_set_height(width);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->display_width), height);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->display_height), width);

    gb_setting_project_save();
}
