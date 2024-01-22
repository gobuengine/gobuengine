#include "gapp-project-settings.h"
#include "gapp-ps-view-display.h"
#include "gapp-ps-view-gameinfo.h"
#include "gapp_widget.h"
#include "gb_setting_project.h"

#include "utility/gb_string.h"

extern ProjectEditor editor;

struct _GappProjectSettings
{
    GtkWidget parent;
};

typedef struct handler_data
{
    GappProjectSettings *self;
    int type;
} handler_data;

G_DEFINE_TYPE_WITH_PRIVATE(GappProjectSettings, gapp_project_settings, GTK_TYPE_WIDGET);

static void gapp_project_settings_class_finalize(GObject *object)
{
    GappProjectSettings *setting = GAPP_PROJECT_SETTINGS(object);

    // g_clear_pointer(&setting->paned, gtk_widget_unparent);

    G_OBJECT_CLASS(gapp_project_settings_parent_class)->finalize(object);
}

static void gapp_project_settings_class_init(GappProjectSettingsClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    // GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    // object_class->finalize = gapp_project_settings_class_finalize;

    // gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

//  -------------------------

GtkWidget *project_setting_group_child_append(const char *label_str, GtkWidget *listbox, GtkWidget *input, GtkOrientation orientation)
{
    GtkWidget *listboxrow, *label, *hbox;

    listboxrow = gtk_list_box_row_new();
    gtk_list_box_append(listbox, listboxrow);

    hbox = gtk_box_new(orientation, 1);
    gtk_list_box_row_set_child(listboxrow, hbox);

    label = gtk_label_new(label_str);
    gtk_label_set_xalign(label, 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_size_request(label, 100, -1);
    gtk_box_append(hbox, label);

    gtk_box_append(hbox, input);

    return listboxrow;
}

GtkWidget *project_setting_group_input_entry(const char *placeholder, const char *tooltip)
{
    GtkWidget *entry;

    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_entry_set_input_purpose(entry, GTK_INPUT_PURPOSE_ALPHA | GTK_INPUT_PURPOSE_NUMBER);
    gtk_widget_set_valign(entry, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_tooltip_text(entry, tooltip);

    return entry;
}

static void gapp_project_settings_init(GappProjectSettings *self)
{
    GtkWidget *scroll, *vbox, *list_setting, *list_setting_props, *win;

    GappProjectSettingsPrivate *priv = gapp_project_settings_get_instance_private(self);

    int width, height;
    gtk_window_get_default_size(editor.window, &width, &height);

    win = gtk_window_new();
    gtk_window_set_modal(GTK_WINDOW(win), TRUE);
    gtk_window_set_title(GTK_WINDOW(win), "Project Setting");
    gtk_window_set_default_size(GTK_WINDOW(win), 900, (height - 100));
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(win), editor.window);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(win, box);

    GtkWidget *sidebar = gtk_stack_sidebar_new();
    gtk_widget_set_size_request(GTK_WIDGET(sidebar), 200, -1);
    gtk_box_append(GTK_BOX(box), sidebar);

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
    gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(sidebar), GTK_STACK(stack));
    gtk_widget_set_hexpand(stack, TRUE);
    gtk_box_append(GTK_BOX(box), stack);
    {
        GtkWidget *gameInfo = gapp_project_setting_game_info_new(priv);
        gtk_stack_add_named(GTK_STACK(stack), gameInfo, ("Game Info"));
        GtkStackPage *page1 = gtk_stack_get_page(GTK_STACK(stack), gameInfo);
        gtk_stack_page_set_icon_name(page1, "dialog-information-symbolic");
        gtk_stack_page_set_title(page1, "Game Info");

        GtkWidget *display = gapp_project_setting_display_new(priv);
        gtk_stack_add_named(GTK_STACK(stack), display, ("Display"));
        GtkStackPage *page2 = gtk_stack_get_page(GTK_STACK(stack), display);
        gtk_stack_page_set_icon_name(page2, "preferences-desktop-display-symbolic");
        gtk_stack_page_set_title(page2, "Display");
    }

    gtk_window_present(win);
}

GappProjectSettings *gapp_pe_project_settings_new(void)
{
    return g_object_new(GAPP_TYPE_PROJECT_SETTINGS, NULL);
}
