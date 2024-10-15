#include "gapp_project_setting.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_project_config.h"
#include "gapp.h"

// -----------------
// MARK:BASE CLASS
// -----------------

struct _GappProjectSetting
{
    GtkWidget parent_instance;
};

static void gobu_project_settin_ui_setup(GappProjectSetting *self);
static void gobu_project_setting_dispose(GObject *object);

G_DEFINE_TYPE(GappProjectSetting, gobu_project_setting, GTK_TYPE_WINDOW)

static void gobu_project_setting_dispose(GObject *object)
{
    GappProjectSetting *self = GOBU_PROJECT_SETTING(object);

    g_clear_pointer(&self, gtk_widget_unparent);

    G_OBJECT_CLASS(gobu_project_setting_parent_class)->dispose(object);
}

static void gobu_project_setting_class_init(GappProjectSettingClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gobu_project_setting_dispose;
}

static void gobu_project_setting_init(GappProjectSetting *self)
{
    gobu_project_settin_ui_setup(self);
}

// -----------------
// MARK:FUNCTION
// -----------------

// ---------------------------
// WIDGET PROPERTY COLOR
// ---------------------------
static void gapp_project_config_s_color_activate(GtkColorDialogButton *self, GParamSpec *ignored, gchar *id_property)
{
    const GdkRGBA *color = gtk_color_dialog_button_get_rgba(self);
    g_object_set(gapp_get_config_instance(), id_property, color, NULL);
}

static GtkWidget *gapp_project_setting_prop_color_new(const char *tooltip, gchar *id_property)
{
    GtkWidget *color_button = gtk_color_dialog_button_new(gtk_color_dialog_new());
    gtk_widget_set_tooltip_text(color_button, tooltip);

    // obtenemos los datos registrados y lo insertamos en el widget
    GdkRGBA *value;
    g_object_get(gapp_get_config_instance(), id_property, &value, NULL);
    gtk_color_dialog_button_set_rgba(GTK_COLOR_DIALOG_BUTTON(color_button), (value ? value : &(GdkRGBA){0.0f, 0.0f, 0.0f, 1.0f}));

    g_signal_connect(color_button, "notify::rgba", G_CALLBACK(gapp_project_config_s_color_activate), g_strdup(id_property));

    return color_button;
}

// ---------------------------
// WIDGET PROPERTY SELECT-OPTION
// ---------------------------
static void gapp_project_config_s_select_activate(GtkDropDown *self, GParamSpec *pspec, gchar *id_property)
{
    guint value = gtk_drop_down_get_selected(self);
    g_object_set(gapp_get_config_instance(), id_property, GINT_TO_POINTER(value), NULL);
}

static GtkWidget *gapp_project_setting_prop_select_new(const char *const *strings, const char *tooltip, gchar *id_property)
{
    GtkWidget *select_option = gtk_drop_down_new_from_strings(strings);
    gtk_widget_set_tooltip_text(select_option, tooltip);

    // obtenemos los datos registrados y lo insertamos en el widget
    guint value;
    g_object_get(gapp_get_config_instance(), id_property, &value, NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(select_option), value);

    g_signal_connect(select_option, "notify::selected", G_CALLBACK(gapp_project_config_s_select_activate), g_strdup(id_property));

    return select_option;
}

// ---------------------------
// WIDGET PROPERTY CHECKBOX
// ---------------------------
static void gapp_project_config_s_check_toggled(GtkCheckButton *self, gchar *id_property)
{
    gboolean value = gtk_check_button_get_active(self);
    g_object_set(gapp_get_config_instance(), id_property, GINT_TO_POINTER(value), NULL);
}

static GtkWidget *gapp_project_setting_prop_check_new(const gchar *label, const char *tooltip, gchar *id_property)
{
    GtkWidget *check = gtk_check_button_new_with_label(label);
    gtk_widget_set_tooltip_text(check, tooltip);

    // obtenemos los datos registrados y lo insertamos en el widget
    gboolean value;
    g_object_get(gapp_get_config_instance(), id_property, &value, NULL);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(check), value);

    g_signal_connect(check, "toggled", G_CALLBACK(gapp_project_config_s_check_toggled), g_strdup(id_property));

    return check;
}

// ---------------------------
// WIDGET PROPERTY NUMBER
// ---------------------------
static void gapp_project_config_s_spin_button_changed(GtkSpinButton *self, gchar *id_property)
{
    double value = gtk_spin_button_get_value(self);
    g_object_set(gapp_get_config_instance(), id_property, GINT_TO_POINTER(value), NULL);
}

static GtkWidget *gapp_project_setting_prop_number_new(const char *tooltip, gchar *id_property)
{
    GtkWidget *number_spin = gtk_spin_button_new_with_range(INTMAX_MIN, INTMAX_MAX, 1);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_widget_set_tooltip_text(number_spin, tooltip);

    // obtenemos los datos registrados y lo insertamos en el widget
    gint value;
    g_object_get(gapp_get_config_instance(), id_property, &value, NULL);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), value);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(gapp_project_config_s_spin_button_changed), g_strdup(id_property));

    return number_spin;
}

// ---------------------------
// WIDGET PROPERTY INPUT TEXT
// ---------------------------
static void gapp_project_config_s_entry_changed(GtkEditable *self, gchar *id_property)
{
    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(self));
    g_object_set(gapp_get_config_instance(), id_property, g_strdup(text), NULL);
}

static GtkWidget *gapp_project_setting_prop_entry_new(const char *placeholder, const char *tooltip, const gchar *id_property)
{
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_entry_set_input_purpose(entry, GTK_INPUT_PURPOSE_ALPHA | GTK_INPUT_PURPOSE_NUMBER);
    gtk_widget_set_valign(entry, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_tooltip_text(entry, tooltip);

    // obtenemos los datos registrados y lo insertamos en el widget
    gchar *value;
    g_object_get(gapp_get_config_instance(), id_property, &value, NULL);
    gapp_widget_entry_set_text(entry, (value ? value : ""));
    g_free(value);

    g_signal_connect(entry, "changed", G_CALLBACK(gapp_project_config_s_entry_changed), g_strdup(id_property));

    return entry;
}

// ---------------------------
// WIDGET PROPERTY TEXT VIEW
// ---------------------------
static void gapp_project_config_s_text_view_changed(GtkTextBuffer *self, gchar *id_property)
{
    gchar *text = gapp_widget_text_view_get_text(self);
    g_object_set(gapp_get_config_instance(), id_property, g_strdup(text), NULL);
    g_free(text);
}

static GtkWidget *gapp_project_setting_prop_text_view_new(const gchar *id_property)
{
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(text_view), -1, 100);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 10);

    // obtenemos los datos registrados y lo insertamos en el widget
    gchar *value;
    g_object_get(gapp_get_config_instance(), id_property, &value, NULL);
    gapp_widget_text_view_set_text(text_view, (value ? value : ""));
    g_free(value);

    g_signal_connect(gtk_text_view_get_buffer(text_view), "changed", G_CALLBACK(gapp_project_config_s_text_view_changed), g_strdup(id_property));

    return text_view;
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
    gtk_size_group_add_widget(size_group, input);

    return listboxrow;
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
// -----------------
// MARK:SIGNAL
// -----------------

// -----------------
// MARK:UI
// -----------------

static void gobu_project_settin_ui_setup(GappProjectSetting *self)
{
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
    {
        GtkWidget *listbox;

        // Application
        {
            GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Application");

            listbox = gapp_project_setting_prop_group_new("About");
            gtk_box_append(vbox, listbox);
            {
                GtkWidget *entry_nameProject = gapp_project_setting_prop_entry_new("Game entry", "", "name");
                gapp_project_setting_prop_group_child_new("Name", listbox, size_group, entry_nameProject, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *entry_version = gapp_project_setting_prop_entry_new("Version", "The project's version number.", "version");
                gapp_project_setting_prop_group_child_new("Version", listbox, size_group, entry_version, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *description = gapp_project_setting_prop_text_view_new("description");
                gapp_project_setting_prop_group_child_new("Description", listbox, size_group, description, GTK_ORIENTATION_VERTICAL);

                GtkWidget *entry_id = gapp_project_setting_prop_entry_new("com.company.appname", "ID APP", "id");
                gapp_project_setting_prop_group_child_new("ID", listbox, size_group, entry_id, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *entry_author = gapp_project_setting_prop_entry_new("Author", "The project's author", "author");
                gapp_project_setting_prop_group_child_new("Author", listbox, size_group, entry_author, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *entry_email = gapp_project_setting_prop_entry_new("Email", "author@example.com", "email");
                gapp_project_setting_prop_group_child_new("Email", listbox, size_group, entry_email, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *entry_iconSelect = gapp_project_setting_prop_entry_new("Icon", "Icon used for the project, set when project loads. Exporters will also use this icon as a fallback if necessary.", "icon");
                gapp_project_setting_prop_group_child_new("Icon", listbox, size_group, entry_iconSelect, GTK_ORIENTATION_HORIZONTAL);
            }

            listbox = gapp_project_setting_prop_group_new("Startup");
            gtk_box_append(vbox, listbox);
            {
                GtkWidget *entry_mainscene = gapp_project_setting_prop_entry_new("First scene", "Path to the main scene file that will be loaded when the project runs.", "first_scene");
                gapp_project_setting_prop_group_child_new("First scene", listbox, size_group, entry_mainscene, GTK_ORIENTATION_HORIZONTAL);
            }
        }

        // Display
        {
            GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Display");

            listbox = gapp_project_setting_prop_group_new("Viewport");
            gtk_box_append(vbox, listbox);
            {
                GtkWidget *spin_vwidth = gapp_project_setting_prop_number_new("", "viewport_width");
                gapp_project_setting_prop_group_child_new("Viewport Width", listbox, size_group, spin_vwidth, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *spin_height = gapp_project_setting_prop_number_new("", "viewport_height");
                gapp_project_setting_prop_group_child_new("Viewport Height", listbox, size_group, spin_height, GTK_ORIENTATION_HORIZONTAL);

                const char *const *texture_filter[] = {"Nearest", "Bliniear", "Trilinear", NULL};
                GtkWidget *list_filter = gapp_project_setting_prop_select_new(texture_filter, "Texture filter", "texture_filter");
                gapp_project_setting_prop_group_child_new("Sampling", listbox, size_group, list_filter, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *color_bg = gapp_project_setting_prop_color_new("", "background_color");
                gapp_project_setting_prop_group_child_new("Default Clear Color", listbox, size_group, color_bg, GTK_ORIENTATION_HORIZONTAL);
            }

            listbox = gapp_project_setting_prop_group_new("Window");
            gtk_box_append(vbox, listbox);
            {
                GtkWidget *spin_wwidth = gapp_project_setting_prop_number_new("", "window_width");
                gapp_project_setting_prop_group_child_new("Window Width", listbox, size_group, spin_wwidth, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *spin_wheight = gapp_project_setting_prop_number_new("", "window_height");
                gapp_project_setting_prop_group_child_new("Window Height", listbox, size_group, spin_wheight, GTK_ORIENTATION_HORIZONTAL);

                const char *const *mode[] = {"Windowed", "Minimized", "Maximized", "Fullscreen", NULL};
                GtkWidget *list_mode = gapp_project_setting_prop_select_new(mode, "The initial window mode", "window_mode");
                gapp_project_setting_prop_group_child_new("Mode", listbox, size_group, list_mode, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_resizable = gapp_project_setting_prop_check_new("Allow Window Resize", "Should the user be allowed to resize the window used by the game, where not using full screen", "window_resizable");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_resizable, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_borderl = gapp_project_setting_prop_check_new("Use Borderless Window", "Should the game use a borderless Slate window instead of a window with system title bar and border", "window_borderless");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_borderl, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_always = gapp_project_setting_prop_check_new("Always on Top", "", "window_alway_on_top");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_always, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_transparent = gapp_project_setting_prop_check_new("Transparent", "", "window_transparent");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_transparent, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_nofocus = gapp_project_setting_prop_check_new("No Focus", "", "window_no_focus");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_nofocus, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_dpi = gapp_project_setting_prop_check_new("Allow high DPI", "", "window_high_dpi");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_dpi, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_vsync = gapp_project_setting_prop_check_new("VSync", "", "window_vsync");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_vsync, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *check_aspect = gapp_project_setting_prop_check_new("Should Window Preserve Aspect Ratio", "Should the game's window preserve its aspect ration when resized by user", "window_aspect_ratio");
                gapp_project_setting_prop_group_child_new(NULL, listbox, size_group, check_aspect, GTK_ORIENTATION_HORIZONTAL);
            }
        }

        // Rendering
        {
        }

        // Physics
        {
            GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Physics");

            listbox = gapp_project_setting_prop_group_new("");
            gtk_box_append(vbox, listbox);
            {
                GtkWidget *p_gravity = gapp_project_setting_prop_number_new("The default gravity strength in 2D (in pixels per second squared).", "physics_gravity");
                gapp_project_setting_prop_group_child_new("Default Gravity", listbox, size_group, p_gravity, GTK_ORIENTATION_HORIZONTAL);

                GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
                gtk_widget_set_hexpand(hbox, TRUE);
                {
                    gtk_box_append(hbox, gapp_project_setting_prop_number_new("X direction", "physics_gravity_directionx"));
                    gtk_box_append(hbox, gapp_project_setting_prop_number_new("Y direction", "physics_gravity_directiony"));
                }

                gapp_project_setting_prop_group_child_new("Default Gravity Direction", listbox, size_group, hbox, GTK_ORIENTATION_VERTICAL);
            }
        }

        // InputMap
    }
}

// -----------------
// MARK:API
// -----------------

GappProjectSetting *gobu_project_setting_new(void)
{
    return g_object_new(GOBU_TYPE_PROJECT_SETTING, NULL);
}

void gobu_project_setting_show(GappProjectSetting *self, GtkWidget *parent)
{
    gtk_window_set_transient_for(GTK_WINDOW(self), GTK_WINDOW(parent));
    gtk_window_present(GTK_WINDOW(self));
}
