#include "config.h"
#include "gapp_project_manager.h"
#include "gapp_pm_create_project.h"
#include "gapp_pm_open_project.h"
#include "gapp_pm_register_project.h"
#include "gapp_pm_list_item_open_project.h"
#include "gapp_widget.h"
#include "binn/binn_json.h"
#include "utility/gb_path.h"
#include "utility/gb_fs.h"

static GobuProjectManager project_manager = {0};

static void project_config_user_if_valid(void)
{
    const char *config_dir = gb_path_user_config_dir();
    char *path_config = gb_path_join(config_dir, "gobu-engine", NULL);
    if (!gb_path_exist(path_config))
    {
        gb_path_create_new(path_config);
        gb_fs_write_file(gb_path_join(path_config, "project-list.json", NULL), "");
    }
    g_free(path_config);
}

static void signal_setup_view_project(GtkSignalListItemFactory *self, GtkListItem *list_item, gpointer data)
{
    GtkWidget *box, *imagen, *label;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_list_item_set_child(list_item, box);
    gtk_widget_set_size_request(GTK_WIDGET(box), 120, 120);

    imagen = gtk_image_new();
    gtk_image_set_icon_size(imagen, GTK_ICON_SIZE_LARGE);
    gtk_widget_set_vexpand(GTK_WIDGET(imagen), TRUE);
    gtk_box_append(GTK_BOX(box), imagen);

    label = gtk_label_new(NULL);
    gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_CHAR);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(label));
}

static void signal_bind_view_project(GtkSignalListItemFactory *self, GtkListItem *list_item, gpointer data)
{
    GtkWidget *box, *label, *image;

    box = gtk_list_item_get_child(list_item);
    g_return_if_fail(box);

    image = gtk_widget_get_first_child(box);
    label = gtk_widget_get_next_sibling(image);

    GtkStringObject *obj = gtk_list_item_get_item(list_item);
    const char *path_project = gb_path_dirname(gtk_string_object_get_string(obj));

    gchar *path = gb_path_join(path_project, "Saved", "thumbnail.png", NULL);
    if (gb_fs_file_exist(path))
        gtk_image_set_from_file(image, path);
    else
        gtk_image_set_from_icon_name(image, "image-missing-symbolic");

    gtk_label_set_label(GTK_LABEL(label), gb_path_basename(path_project));
    gtk_widget_set_tooltip_text(box, path_project);

    g_free(path);
}

static GtkStringList *create_list_model()
{
    binn *list = gapp_pm_register_project_get_list();

    GtkStringList *sl = gtk_string_list_new(NULL);
    {
        // [{"path":""},...]
        int count = binn_count(list);
        for (int i = 1; i <= count; i++)
        {
            void *obj = binn_list_object(list, i);
            char *path = binn_object_str(obj, "path");
            gtk_string_list_append(sl, path);
        }
        binn_free(list);
    }

    return sl;
}

static GtkWidget *notebook_page_local_project(void)
{
    GtkWidget *search, *grid, *scroll, *vbox;
    GtkSingleSelection *selection;
    GtkFilterListModel *filter_model;
    GtkFilter *filter;

    GtkStringList *sl = create_list_model();

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    {
        filter = GTK_FILTER(gtk_string_filter_new(gtk_property_expression_new(GTK_TYPE_STRING_OBJECT, NULL, "string")));
        filter_model = gtk_filter_list_model_new(G_LIST_MODEL(sl), filter);
        gtk_filter_list_model_set_incremental(filter_model, TRUE);

        search = gtk_search_entry_new();
        g_object_bind_property(search, "text", filter, "search", 0);
        gapp_widget_set_margin(search, 5);
        gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(search));

        scroll = gtk_scrolled_window_new();
        gapp_widget_set_margin(GTK_WIDGET(scroll), 5);
        gtk_widget_set_vexpand(GTK_WIDGET(scroll), TRUE);
        gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(scroll));
        {
            selection = gtk_single_selection_new(G_LIST_MODEL(filter_model));

            GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_setup_view_project), NULL);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_bind_view_project), NULL);

            grid = gtk_grid_view_new(selection, factory);
            gtk_grid_view_set_max_columns(grid, 15);
            gtk_grid_view_set_min_columns(grid, 2);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);

            g_signal_connect(grid, "activate", G_CALLBACK(gapp_pm_list_item_open_project), &project_manager);
        }
    }

    return vbox;
}

void gapp_project_manager_window_new(GtkApplication *app)
{
    GtkWidget *hbox, *notebook, *headerbar, *btn_new_project, *btn_open_other;

    project_config_user_if_valid();

    project_manager.window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(project_manager.window), lang_projectm_title);
    gtk_window_set_default_size(GTK_WINDOW(project_manager.window), 1120, 600);
    gtk_window_set_resizable(GTK_WINDOW(project_manager.window), TRUE);

    headerbar = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(project_manager.window), headerbar);
    {
        hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_header_bar_pack_end(headerbar, hbox);

        btn_new_project = gapp_widget_button_new_icon_with_label("window-new-symbolic", "New");
        gtk_box_append(GTK_BOX(hbox), GTK_WIDGET(btn_new_project));
        g_signal_connect(btn_new_project, "clicked", G_CALLBACK(gapp_pm_signal_dialog_create_project), &project_manager);

        btn_open_other = gapp_widget_button_new_icon_with_label("document-open-symbolic", "Open other");
        gtk_box_append(GTK_BOX(hbox), GTK_WIDGET(btn_open_other));
        g_signal_connect(btn_open_other, "clicked", G_CALLBACK(gapp_pm_signal_dialog_open_project), &project_manager);
    }

    notebook = gtk_notebook_new();
    gtk_notebook_set_show_border(notebook, FALSE);
    gtk_window_set_child(GTK_WINDOW(project_manager.window), GTK_WIDGET(notebook));
    {
        GtkWidget *local_project = notebook_page_local_project();
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), local_project, gtk_label_new("Local projects"));
    }

    gtk_window_present(GTK_WINDOW(project_manager.window));
}
