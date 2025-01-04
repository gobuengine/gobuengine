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

    {
        GtkWidget *vbox = gapp_project_setting_prop_page_new(stack, "Game properties");
        {
            listbox = gapp_project_setting_prop_group_new("Game info");
            gtk_box_append(vbox, listbox);

            listbox = gapp_project_setting_prop_group_new("Packaging");
            gtk_box_append(vbox, listbox);

            listbox = gapp_project_setting_prop_group_new("Resolution and rendering");
            gtk_box_append(vbox, listbox);
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

const char *gapp_project_setting_get_name(ecs_world_t *world)
{
    return "";
}
