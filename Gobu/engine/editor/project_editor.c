#include "project_editor.h"
#include "gapp_main.h"

#include "browser.h"
#include "gapp_level_editor.h"
#include "gapp_settings.h"

extern GAPP* EditorCore;

static void project_editor_resource_load(void)
{
    const char* const* icon_name[] = { "component.png", "level.png", "entity.png", "folder.png", "anim2d.png", "anim2d.png" };
    char* path_r = gb_path_current_dir();

    for (size_t i = 0; i < GOBU_RESOURCE_ICONS; i++)
    {
        gchar* full_path = gb_path_join(path_r, "Content", "icons", icon_name[i], NULL);
        EditorCore->resource.icons[i] = gdk_pixbuf_new_from_file_at_size(full_path, ICON_DEFAULT_SIZE, ICON_DEFAULT_SIZE, NULL);
        g_assert(EditorCore->resource.icons[i]);
        g_free(full_path);
    }
    g_free(path_r);

    g_print("GOBU-APP: Resource default load\n");
}

static void project_editor_resource_free(void)
{
    g_info("GOBU-APP: Resource Free\n");
    for (size_t i = 0; i < GOBU_RESOURCE_ICONS; i++)
    {
        g_assert(EditorCore->resource.icons[i]);
        g_free(EditorCore->resource.icons[i]);
    }
}

static void project_editor_process(GtkWidget* widget, gpointer user_data)
{
    g_info("GOBU-APP: Project-Editor inicialize\n");
    project_editor_resource_load();
}

static void project_editor_free(GtkWidget* widget, gpointer user_data)
{
    g_info("GOBU-APP: Project-Editor free\n");
    project_editor_resource_free();
}

static void signal_close_page(GtkWidget* button, GtkNotebook* notebook)
{
    int page = gtk_notebook_get_current_page(notebook);
    gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page);
}

void gapp_project_editor_append_page(enumGappNotebook notebook_id, int icon_type, const gchar* name, GtkWidget* child)
{
    GtkWidget* hbox;
    GtkWidget* label, * icon, * button;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    {
        icon = gtk_image_new();
        gtk_image_set_from_gicon(icon, EditorCore->resource.icons[icon_type]);
        gtk_box_append(hbox, icon);

        label = gtk_label_new(name);
        gtk_box_append(hbox, label);

        button = gapp_widget_button_new_icon_with_label("window-close-symbolic", NULL);
        gtk_box_append(hbox, button);
        g_signal_connect(button, "clicked", G_CALLBACK(signal_close_page), EditorCore->notebook[notebook_id]);
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(EditorCore->notebook[notebook_id]), child, hbox);
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(EditorCore->notebook[notebook_id]), child, TRUE);
    // gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(EditorCore->notebook[GAPP_NOTEBOOK_DEFAULT]), level, TRUE);
}

void gapp_project_editor_window_new(GtkApplication* app)
{
    GtkWidget* headerbar, * titlew;
    GtkWidget* hbox;
    GtkWidget* paned, * paned_bottom;

    EditorCore->window = gtk_application_window_new(app);
    // gtk_window_set_title(GTK_WINDOW(EditorCore->window), gb_strdups("[%s] - GobuEngine", gb_setting_project_name()));
    gtk_window_set_default_size(GTK_WINDOW(EditorCore->window), 1280, 720);
    gtk_widget_set_size_request(GTK_WIDGET(EditorCore->window), 1280, 720);
    gtk_window_maximize(GTK_WINDOW(EditorCore->window));
    g_signal_connect(EditorCore->window, "realize", G_CALLBACK(project_editor_process), NULL);
    g_signal_connect(EditorCore->window, "destroy", G_CALLBACK(project_editor_free), NULL);

    headerbar = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(EditorCore->window), headerbar);
    {
        {
            titlew = gtk_label_new(NULL);
            gtk_label_set_markup(titlew, gb_strdups("[ %s ] - GobuEngine", gb_setting_project_name()));
            gtk_header_bar_pack_start(headerbar, titlew);
        }

        hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_header_bar_set_title_widget(headerbar, hbox);
        {
            GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save all");
            gtk_box_append(hbox, btn_s);

            GtkWidget* btn_p = gapp_widget_button_new_icon_with_label("applications-games-symbolic", "Preview");
            gtk_box_append(hbox, btn_p);

            GtkWidget* btn_b = gapp_widget_button_new_icon_with_label("drive-optical-symbolic", "Build");
            gtk_box_append(hbox, btn_b);
        }

        {
            GtkWidget* btn_set = gapp_widget_button_new_icon_with_label("preferences-other-symbolic", "World Settings");
            gtk_header_bar_pack_end(headerbar, btn_set);
            g_signal_connect(btn_set, "clicked", G_CALLBACK(gapp_settings_new), NULL);
        }
    }

    {
        paned = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
        gtk_window_set_child(GTK_WINDOW(EditorCore->window), paned);
        {
            gtk_paned_set_start_child(GTK_PANED(paned), gbapp_browser_new());

            paned_bottom = gapp_widget_paned_new(GTK_ORIENTATION_VERTICAL, FALSE);
            gtk_paned_set_end_child(GTK_PANED(paned), paned_bottom);

            EditorCore->notebook[GAPP_NOTEBOOK_DEFAULT] = gtk_notebook_new();
            gtk_notebook_set_show_border(GTK_NOTEBOOK(EditorCore->notebook[GAPP_NOTEBOOK_DEFAULT]), FALSE);
            gtk_notebook_popup_enable(GTK_NOTEBOOK(EditorCore->notebook[GAPP_NOTEBOOK_DEFAULT]));
            gtk_widget_set_size_request(GTK_WIDGET(EditorCore->notebook[GAPP_NOTEBOOK_DEFAULT]), -1, DOCK_T_MIN_SIZE);
            gtk_paned_set_start_child(GTK_PANED(paned_bottom), EditorCore->notebook[GAPP_NOTEBOOK_DEFAULT]);

            EditorCore->notebook[GAPP_NOTEBOOK_BOTTOM] = gtk_notebook_new();
            gtk_notebook_set_show_border(GTK_NOTEBOOK(EditorCore->notebook[GAPP_NOTEBOOK_BOTTOM]), FALSE);
            gtk_notebook_popup_enable(GTK_NOTEBOOK(EditorCore->notebook[GAPP_NOTEBOOK_BOTTOM]));
            // gtk_widget_set_size_request(GTK_WIDGET(EditorCore->notebook[GAPP_NOTEBOOK_BOTTOM]), -1, 100);
            gtk_paned_set_end_child(GTK_PANED(paned_bottom), EditorCore->notebook[GAPP_NOTEBOOK_BOTTOM]);

            EditorCore->console = gapp_tool_console_new();
        }
    }

    gtk_window_present(GTK_WINDOW(EditorCore->window));
}

