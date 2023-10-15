#include <glib.h>
#include "config.h"
#include "bugo/bugo.h"
#include "gobu_project_editor.h"
#include "gobu_script_editor.h"
#include "gobu_editor_world.h"
#include "gobu-app.h"

extern GobuApp *APP;

static void gobu_project_editor_resource_load(void)
{
    g_info("GOBU: Resource default load\n");
    const char *const *icon_name[] = {"component.png", "level.png", "entity.png", "folder.png", "anim2d.png"};
    gchar *path_r = g_get_current_dir();

    for (size_t i = 0; i < GOBU_RESOURCE_ICONS; i++)
    {
        gchar *full_path = g_build_filename(path_r, "resource", "icons", icon_name[i], NULL);
        APP->resource.icons[i] = gdk_pixbuf_new_from_file_at_size(full_path, ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE, NULL);
        g_assert(APP->resource.icons[i]);
        g_free(full_path);
    }
    g_free(path_r);
}

static void gobu_project_editor_resource_free(void)
{
    g_info("GOBU: Resource Free\n");
    for (size_t i = 0; i < GOBU_RESOURCE_ICONS; i++)
    {
        g_assert(APP->resource.icons[i]);
        g_free(APP->resource.icons[i]);
    }
}

static void gobu_project_editor_process(GtkWidget *widget, gpointer user_data)
{
    g_info("GOBU: Project Editor Init\n");
    gobu_project_editor_resource_load();
}

static void gobu_project_editor_free(GtkWidget *widget, gpointer user_data)
{
    g_info("GOBU: Project Editor Close\n");
    gobu_project_editor_resource_free();
}

void gobu_project_editor_init(GtkApplication *app)
{
    GtkWidget *headerbar, *titlew;
    GtkWidget *stack, *switcher;

    APP->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(APP->window), g_strdup_printf("GobuEngine - %s", bugo_project_get_name()));
    gtk_window_set_default_size(GTK_WINDOW(APP->window), 1280, 720);
    gtk_window_maximize(GTK_WINDOW(APP->window));
    g_signal_connect(APP->window, "realize", G_CALLBACK(gobu_project_editor_process), NULL);
    g_signal_connect(APP->window, "destroy", G_CALLBACK(gobu_project_editor_free), NULL);

    // titlew = gtk_label_new(NULL);
    // gtk_label_set_markup(titlew, "<b>" GOBU_VERSION_STR "</b>");

    // headerbar = gtk_header_bar_new();
    // gtk_window_set_titlebar(GTK_WINDOW(APP->window), headerbar);
    // gtk_header_bar_pack_start(headerbar, titlew);
    // gtk_header_bar_set_show_title_buttons(headerbar, TRUE);

    // switcher = gtk_stack_switcher_new();
    // gtk_header_bar_set_title_widget(headerbar, switcher);

    // stack = gtk_stack_new();
    // gtk_stack_switcher_set_stack(switcher, stack);
    // gtk_stack_set_transition_type(stack, GTK_STACK_TRANSITION_TYPE_NONE);
    gtk_window_set_child(GTK_WINDOW(APP->window), gobu_editor_world_new());

    // gtk_stack_add_titled(stack, gobu_editor_world_new(), "world", gobu_project_get_name());
    // gtk_stack_add_titled(stack, gobu_script_editor_new(), "code", "Scripts");

    gtk_window_present(GTK_WINDOW(APP->window));
}
