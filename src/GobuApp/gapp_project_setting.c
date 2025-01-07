#include "gapp_project_setting.h"
#include "gapp_inspector_widgets.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappProjectSetting
{
    GtkBox parent_instance;
};

static GtkWidget *gapp_project_create_settings_page(GtkWidget *stack, const gchar *title);
static GtkWidget *gapp_project_create_settings_group(const gchar *title);
static GtkWidget *gapp_project_create_settings_input_row(const char *label_str, GtkWidget *listbox, GtkWidget *size_group, GtkWidget *input, GtkOrientation orientation);
static void gapp_project_setting_handle_field(GtkWidget *parent, GtkWidget *input, const char *field_name, gpointer data);

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

    // ECS GET SETTINGS
    ecs_entity_t project = gobu_ecs_project_settings();
    gb_core_project_settings1_t *gameinfo = ecs_get_mut(GWORLD, project, gb_core_project_settings1_t);
    gb_core_project_settings1_t *packaging = ecs_get_mut(GWORLD, project, gb_core_project_settings2_t);
    gb_core_project_settings1_t *renderer = ecs_get_mut(GWORLD, project, gb_core_project_settings3_t);

    GtkWidget *input_props;
    {
        GtkWidget *vbox = gapp_project_create_settings_page(stack, "Game properties");
        {
            listbox = gapp_project_create_settings_group("Game info");
            gtk_box_append(vbox, listbox);
            {
                gapp_inspector_create_component_fields(GWORLD, gameinfo, ecs_id(gb_core_project_settings1_t), listbox, gapp_project_setting_handle_field, self);
            }

            listbox = gapp_project_create_settings_group("Packaging");
            gtk_box_append(vbox, listbox);
            {
                gapp_inspector_create_component_fields(GWORLD, packaging, ecs_id(gb_core_project_settings2_t), listbox, gapp_project_setting_handle_field, self);
            }

            listbox = gapp_project_create_settings_group("Resolution and rendering");
            gtk_widget_set_margin_bottom(listbox, 30);
            gtk_box_append(vbox, listbox);
            {
                gapp_inspector_create_component_fields(GWORLD, renderer, ecs_id(gb_core_project_settings3_t), listbox, gapp_project_setting_handle_field, self);
            }
        }
    }
    {
        GtkWidget *vbox = gapp_project_create_settings_page(stack, "Branding and Loading screen");
        {
            listbox = gapp_project_create_settings_group("Loading screen");
            gtk_box_append(vbox, listbox);
        }
    }
    {
        GtkWidget *vbox = gapp_project_create_settings_page(stack, "Icons");
        {
            listbox = gapp_project_create_settings_group("Desktop (Windows, Linux, Mac) icon");
            gtk_box_append(vbox, listbox);
        }
    }
}

// -----------------
// MARK:PRIVATE
// -----------------

static GtkWidget *gapp_project_create_settings_page(GtkWidget *stack, const gchar *title)
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

static GtkWidget *gapp_project_create_settings_group(const gchar *title)
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

static GtkWidget *gapp_project_create_settings_input_row(const char *label_str, GtkWidget *listbox, GtkWidget *size_group, GtkWidget *input, GtkOrientation orientation)
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

static void gapp_project_setting_handle_field(GtkWidget *parent, GtkWidget *input, const char *field_name, gpointer data)
{
    GappProjectSetting *self = data;
    GtkWidget *size_group = g_object_get_data(G_OBJECT(self), "size-group");
    gapp_project_create_settings_input_row(field_name, parent, size_group, input, GTK_ORIENTATION_HORIZONTAL);
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

void gapp_project_settings_set_name(const char *name)
{
    gb_core_project_settings1_t *settings = ecs_get_mut(GWORLD, gobu_ecs_project_settings(), gb_core_project_settings1_t);
    settings->name = gobu_util_string(name);
}

const char *gapp_project_settings_name(void)
{
    gb_core_project_settings1_t *settings = ecs_get_mut(GWORLD, gobu_ecs_project_settings(), gb_core_project_settings1_t);
    return settings->name;
}
