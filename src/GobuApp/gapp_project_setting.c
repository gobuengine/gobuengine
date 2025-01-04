#include "gapp_project_setting.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappProjectSetting
{
    GtkBox parent_instance;
};

static GtkWidget *gapp_project_setting_prop_page_new(GtkWidget *stack, const gchar *title);
static GtkWidget *gapp_project_setting_prop_group_new(const gchar *title);
static GtkWidget *gapp_project_setting_prop_group_child_new(const char *label_str, GtkWidget *listbox, GtkWidget *size_group, GtkWidget *input, GtkOrientation orientation);
static GtkWidget *gapp_project_setting_input_text_view_new(void);
static GtkWidget *gapp_project_setting_input_number_new(const char *tooltip, double min, double max, double step);
static GtkWidget *gapp_project_setting_input_select_new(const char *const *strings, const char *tooltip);

// MARK: CLASS
G_DEFINE_TYPE(GappProjectSetting, gapp_project_setting, GTK_TYPE_WINDOW)

static void object_class_dispose(GObject *object)
{
    GappProjectSetting *self = GAPP_PROJECT_SETTING(object);
    G_OBJECT_CLASS(gapp_project_setting_parent_class)->dispose(object);
}

static void gapp_project_setting_class_init(GappProjectSettingClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_project_setting_init(GappProjectSetting *self)
{
    GtkWidget *listbox;

    gtk_window_set_modal(GTK_WINDOW(self), TRUE);
    gtk_window_set_title(GTK_WINDOW(self), "Project Setting");
    gtk_window_set_default_size(GTK_WINDOW(self), 750, 650);
    gtk_window_set_resizable(GTK_WINDOW(self), FALSE);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(GTK_WINDOW(self), box);

    GtkWidget *sidebar = gtk_stack_sidebar_new();
    gtk_widget_set_size_request(GTK_WIDGET(sidebar), 200, -1);
    gtk_box_append(GTK_BOX(box), sidebar);

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
    gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(sidebar), GTK_STACK(stack));
    gtk_widget_set_hexpand(stack, TRUE);
    gtk_box_append(GTK_BOX(box), stack);

    GtkWidget *size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    g_object_set_data_full(G_OBJECT(self), "size-group", size_group, g_object_unref);

    GtkWidget *input_props;
    {
        GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Game properties");
        {
            listbox = gapp_project_setting_prop_group_new("Game info");
            gtk_box_append(vbox, listbox);
            {
                input_props = gtk_entry_new();
                gapp_project_setting_prop_group_child_new("Game name", listbox, size_group, input_props, GTK_ORIENTATION_HORIZONTAL);

                input_props = gapp_project_setting_input_text_view_new();
                gapp_project_setting_prop_group_child_new("Game description", listbox, size_group, input_props, GTK_ORIENTATION_VERTICAL);

                input_props = gtk_entry_new();
                gapp_project_setting_prop_group_child_new("Author", listbox, size_group, input_props, GTK_ORIENTATION_HORIZONTAL);
            }

            listbox = gapp_project_setting_prop_group_new("Packaging");
            gtk_box_append(vbox, listbox);
            {
                input_props = gtk_entry_new();
                gapp_project_setting_prop_group_child_new("Package name", listbox, size_group, input_props, GTK_ORIENTATION_HORIZONTAL);

                input_props = gtk_entry_new();
                gapp_project_setting_prop_group_child_new("Version number", listbox, size_group, input_props, GTK_ORIENTATION_HORIZONTAL);

                input_props = gtk_entry_new();
                gapp_project_setting_prop_group_child_new("Publisher name", listbox, size_group, input_props, GTK_ORIENTATION_HORIZONTAL);
            }

            listbox = gapp_project_setting_prop_group_new("Resolution and rendering");
            gtk_widget_set_margin_bottom(listbox, 30);
            gtk_box_append(vbox, listbox);
            {
                GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
                gtk_widget_set_hexpand(hbox, TRUE);
                gtk_box_append(hbox, gapp_project_setting_input_number_new("Resolution width", 64, 10000, 10));
                gtk_box_append(hbox, gapp_project_setting_input_number_new("Resolution height", 64, 10000, 10));
                gapp_project_setting_prop_group_child_new("Game resolution size (width, height)", listbox, size_group, hbox, GTK_ORIENTATION_VERTICAL);

                const char *const *resolutionModeList[] = {"resize_no_change", "resize_fill_screen", "resize_adjust_width", "resize_adjust_height", NULL};
                input_props = gapp_project_setting_input_select_new(resolutionModeList, "Game resolution");
                gapp_project_setting_prop_group_child_new("Game resolution resize mode", listbox, size_group, input_props, GTK_ORIENTATION_VERTICAL);

                input_props = gapp_project_setting_input_number_new("Target FPS", 20, 240, 1);
                gapp_project_setting_prop_group_child_new("Target FPS", listbox, size_group, input_props, GTK_ORIENTATION_HORIZONTAL);

                const char *const *scaleModeList[] = {"Nearest (no antialiasing, good for pixel perfect games)", "Linear (antialiased rendering, good for most games)", NULL};
                input_props = gapp_project_setting_input_select_new(scaleModeList, "Scale mode");
                gapp_project_setting_prop_group_child_new("Scale mode (also called \"Sampling\")", listbox, size_group, input_props, GTK_ORIENTATION_VERTICAL);
            }
        }
    }
    {
        GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Branding and Loading screen");
        {
            listbox = gapp_project_setting_prop_group_new("Loading screen");
            gtk_box_append(vbox, listbox);
        }
    }
    {
        GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Icons");
        {
            listbox = gapp_project_setting_prop_group_new("Desktop (Windows, Linux, Mac) icon");
            gtk_box_append(vbox, listbox);
        }
    }
}

// -----------------
// MARK:PRIVATE
// -----------------
static GtkWidget *gapp_project_setting_prop_page_new(GtkWidget *stack, const gchar *title)
{
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_stack_add_named(GTK_STACK(stack), scroll, title);
    gtk_stack_page_set_title(gtk_stack_get_page(GTK_STACK(stack), scroll), title);

    GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
    gtk_viewport_set_scroll_to_focus(viewport, TRUE);
    gtk_scrolled_window_set_child(scroll, viewport);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_viewport_set_child(viewport, vbox);

    return vbox;
}

static GtkWidget *gapp_project_setting_prop_group_new(const gchar *title)
{
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *label = gtk_label_new(title);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_add_css_class(label, "title-2");
    gtk_box_append(vbox, label);

    GtkWidget *listbox = gtk_list_box_new();
    gtk_list_box_set_selection_mode(listbox, GTK_SELECTION_NONE);
    gtk_widget_add_css_class(listbox, "rich-list");
    gtk_widget_add_css_class(listbox, "boxed-list");
    gapp_widget_set_margin(listbox, 10);
    gtk_box_append(vbox, listbox);

    return vbox;
}

static GtkWidget *gapp_project_setting_prop_group_child_new(const char *label_str, GtkWidget *listbox, GtkWidget *size_group, GtkWidget *input, GtkOrientation orientation)
{
    GtkWidget *listboxrow, *label, *box;

    GtkWidget *group = gtk_widget_get_last_child(listbox);

    listboxrow = gtk_list_box_row_new();
    gtk_list_box_row_set_activatable(listboxrow, FALSE);
    gtk_list_box_append(group, listboxrow);

    box = gtk_box_new(orientation, 1);
    gtk_list_box_row_set_child(listboxrow, box);

    if (label_str != NULL)
    {
        label = gtk_label_new(label_str);
        gtk_label_set_xalign(label, 0);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_size_request(label, 100, -1);
        gtk_box_append(box, label);
    }

    gtk_widget_set_hexpand(input, TRUE);
    gtk_box_append(box, input);
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        gtk_size_group_add_widget(size_group, input);

    return listboxrow;
}

static GtkWidget *gapp_project_setting_input_text_view_new(void)
{
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(text_view), -1, 100);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 10);

    return text_view;
}

static GtkWidget *gapp_project_setting_input_number_new(const char *tooltip, double min, double max, double step)
{
    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, step);
    gapp_widget_set_noscroll_focus(number_spin);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), 0);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_widget_set_tooltip_text(number_spin, tooltip);

    return number_spin;
}

static GtkWidget *gapp_project_setting_input_select_new(const char *const *strings, const char *tooltip)
{
    GtkWidget *select_option = gtk_drop_down_new_from_strings(strings);
    gtk_widget_set_tooltip_text(select_option, tooltip);

    return select_option;
}

// -----------------
// MARK:SIGNAL
// -----------------

// -----------------
// MARK:UI
// -----------------

// -----------------
// MARK: PUBLIC
// -----------------

GappProjectSetting *gapp_project_setting_new(void)
{
    return g_object_new(GAPP_PROJECT_TYPE_SETTING, NULL);
}

void gapp_project_setting_show(GappProjectSetting *self)
{
    gtk_window_set_transient_for(GTK_WINDOW(self), GTK_WINDOW(gapp_get_window_instance()));
    gtk_window_present(GTK_WINDOW(self));
}

void gapp_project_settings_set_name(ecs_world_t *world, const char *name)
{
    gb_core_project_settings1_t *settings = ecs_get(world, projectSettings, gb_core_project_settings1_t);
    settings->name = gobu_util_string(name);
}

const char *gapp_project_settings_name(ecs_world_t *world)
{
    gb_core_project_settings1_t *settings = ecs_get_mut(world, projectSettings, gb_core_project_settings1_t);
    return settings->name;
}
