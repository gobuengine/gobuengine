#include "gapp-browser.h"
// #include "gapp_level_editor.h"
// #include "gapp_script_editor.h"
// #include "gapp_extrasprites.h"

#include "gapp-browser-view-file-drop.h"
#include "gapp-browser-view-file-menupopup.h"
#include "gapp-browser-view-file-selected.h"
#include "gapp-browser-view-file-factory.h"
#include "gapp-browser-view-file-sorting.h"
#include "gapp-browser-view-file-nav.h"

#include "gapp_widget.h"
#include "gb_project.h"

#include "config.h"

struct _GappBrowser
{
    GtkWidget parent;
};

extern ProjectEditor editor;

G_DEFINE_TYPE_WITH_PRIVATE(GappBrowser, gapp_browser, GTK_TYPE_BOX);

static void gapp_browser_class_init(GappBrowserClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    // GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    // gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
}

/**
 * Crea una nueva acción para el explorador de mundos en el editor de Gobu.
 *
 * @param title     El título de la acción.
 * @param text      El texto descriptivo de la acción.
 * @param response  El valor de respuesta asociado a la acción.
 * @param data      Datos opcionales relacionados con la acción.
 *
 * @return Una nueva acción para el explorador de mundos.
 */
GappBrowserAction *gapp_browser_fn_new_action(const gchar *title, const gchar *label, const gchar *text, int response, gpointer data)
{
    GappBrowserAction *action = g_new0(GappBrowserAction, 1);

    action->title = gb_strdup(title);
    action->label = gb_strdup(label);
    action->text = gb_strdup(text);
    action->response = response;
    action->data = data;

    return action;
}

/**
 * Maneja la señal de creación de elementos en el explorador de mundos del editor en Gobu.
 *
 */
static void signal_create_item_action_response(GtkDialog *widget, const GappBrowserAction *action)
{
    GappBrowserPrivate *private = gapp_browser_get_instance_private(action->data);
    const gchar *result = gapp_widget_dialog_input_get_text(widget);

    if (action->response == ACTION_CREATE_FOLDER)
    {
        gchar *path_result = gb_path_join(private->path_current, result, NULL);
        gb_path_create_new(path_result);
        g_free(path_result);

        gb_log_info(TF("Create Folder: %s", result));
    }
    else if (action->response == ACTION_CREATE_LEVEL)
    {
        gchar *path_result = gb_path_join(private->path_current, TF("%s.level", result), NULL);
        g_file_create_readwrite(g_file_new_for_path(path_result), G_FILE_CREATE_NONE, NULL, NULL);
        g_free(path_result);

        gb_log_info(TF("Create Level: %s", result));
    }
    else if (action->response == ACTION_CREATE_ASPRITES)
    {
        gchar *path_result = gb_path_join(private->path_current, TF("%s.asprites", result), NULL);
        g_file_create_readwrite(g_file_new_for_path(path_result), G_FILE_CREATE_NONE, NULL, NULL);
        g_free(path_result);

        gb_log_info(TF("Create AnimationSprite: %s", result));
    }
}

void gapp_browser_signal_create_item_action(GtkWidget *button, const GappBrowserAction *action)
{
    GtkWidget *btn_ok = gapp_widget_dialog_input(button, action->title, action->label, action->text);
    g_signal_connect(btn_ok, "clicked", G_CALLBACK(signal_create_item_action_response), action);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

/**
 * Abre una carpeta en el explorador de mundos del editor en Gobu.
 *
 * @param browser          El explorador de mundos en el que se abre la carpeta.
 * @param path             La ruta de la carpeta que se abre.
 * @param bhistory_saved   TRUE si la historia de navegación se guarda, FALSE en caso contrario.
 */
void gapp_browser_fn_open_folder(GappBrowser *browser, const gchar *path, gboolean bhistory_saved)
{
    GappBrowserPrivate *private = gapp_browser_get_instance_private(browser);

    // HISTORIAL DE PATH
    if (bhistory_saved)
        g_ptr_array_add(private->path_back, gb_strdup(private->path_current));

    g_free(private->path_current);
    private->path_current = gb_strdup(path);

    GFile *file = g_file_new_for_path(private->path_current);
    gboolean is_home = (g_strcmp0(gb_path_basename(file), FOLDER_CONTENT_PROJECT) != 0);
    gtk_directory_list_set_file(private->directory_list, file);
    // g_object_unref(file);

    // EL BOTON DEL NAV (NEXT)RIGHT SE DESHABILITA SIEMPRE QUE CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
    //  EL BOTON DE NAV (PREV)LEFT SE HABILITA CUANDO CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
    //  EL BOTON DE NAV (HOME) SE HABILITA SI NO ESTAMOS EN CONTENT
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), is_home);

    gb_log_info(TF("Open folder: %s", gb_path_basename(private->path_current)));
}

/**
 * Inicializa el explorador de mundos del editor en Gobu.
 *
 * @param self  El objeto del explorador de mundos que se inicializa.
 */
static void gapp_browser_init(GappBrowser *self)
{
    GtkWidget *vbox;
    GtkWidget *toolbar, *item;
    GtkWidget *scroll, *grid;
    GtkSingleSelection *selection;
    GtkGesture *gesture;
    GtkDropTarget *drop;
    GFile *file;

    GappBrowserPrivate *private = gapp_browser_get_instance_private(self);

    private->path_back = g_ptr_array_new();
    private->path_forward = g_ptr_array_new();
    private->path_current = gb_path_join(gb_project_get_path(), FOLDER_CONTENT_PROJECT, NULL);
    private->path_default = gb_strdup(private->path_current);

    // Toolbar
    // toolbar = gapp_widget_toolbar_new();
    // gapp_widget_set_margin(toolbar, 0);
    // gtk_box_append(self, toolbar);
    // {
    //     item = gapp_widget_button_new_icon_with_label("list-add-symbolic", "Add");
    //     gtk_button_set_has_frame(item, FALSE);
    //     gtk_box_append(GTK_BOX(toolbar), item);
    //     g_signal_connect(item, "clicked", G_CALLBACK(signal_popover), self);
    //     gapp_widget_toolbar_separator_new(toolbar);
    // }

    toolbar = gapp_widget_toolbar_new();
    gapp_widget_set_margin(toolbar, 0);
    gtk_box_append(self, toolbar);
    {
        private->btn_nav_home = gapp_widget_button_new_icon_with_label("go-home-symbolic", "Home");
        gtk_button_set_has_frame(private->btn_nav_home, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), FALSE);
        gtk_box_append(toolbar, private->btn_nav_home);
        g_signal_connect(private->btn_nav_home, "clicked", G_CALLBACK(gapp_browser_view_file_signal_navhome), self);
        gapp_widget_toolbar_separator_new(toolbar);

        private->btn_nav_back = gapp_widget_button_new_icon_with_label("go-previous-symbolic", NULL);
        gtk_button_set_has_frame(private->btn_nav_back, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), FALSE);
        gtk_box_append(toolbar, private->btn_nav_back);
        g_signal_connect(private->btn_nav_back, "clicked", G_CALLBACK(gapp_browser_view_file_signal_navback), self);
        gapp_widget_toolbar_separator_new(toolbar);

        private->btn_nav_forward = gapp_widget_button_new_icon_with_label("go-next-symbolic", NULL);
        gtk_button_set_has_frame(private->btn_nav_forward, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_forward);
        g_signal_connect(private->btn_nav_forward, "clicked", G_CALLBACK(gapp_browser_view_file_signal_navforward), self);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    // View File
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(self, scroll);
    {
        file = g_file_new_for_path(private->path_default);
        private->directory_list = gtk_directory_list_new("standard::*", file);
        gtk_directory_list_set_monitored(private->directory_list, TRUE);
        // gtk_directory_list_set_io_priority(private->directory_list, G_PRIORITY_LOW);
        g_object_unref(file);

        GtkSorter *sorter = GTK_SORTER(gtk_custom_sorter_new((GCompareDataFunc)gapp_browser_view_file_signal_sorting, NULL, NULL));

        private->selection = gtk_multi_selection_new(gtk_sort_list_model_new(G_LIST_MODEL(private->directory_list), sorter));
        // gtk_single_selection_set_can_unselect(private->selection, TRUE);
        // gtk_single_selection_set_autoselect(private->selection, FALSE);

        private->factory = gtk_signal_list_item_factory_new();
        g_signal_connect(private->factory, "setup", G_CALLBACK(gapp_browser_view_file_signal_setup_factory), self);
        g_signal_connect(private->factory, "bind", G_CALLBACK(gapp_browser_view_file_signal_bind_factory), self);

        grid = gtk_grid_view_new(private->selection, private->factory);
        gtk_grid_view_set_max_columns(grid, 15);
        gtk_grid_view_set_min_columns(grid, 2);
        g_signal_connect(grid, "activate", G_CALLBACK(gapp_browser_view_file_signal_selected), self);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);

        gesture = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
        g_signal_connect(gesture, "released", G_CALLBACK(gapp_browser_view_file_signal_menupopup), self);
        gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(gesture));

        drop = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
        g_signal_connect(drop, "drop", G_CALLBACK(gapp_browser_signal_view_file_drop), self);
        gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(drop));
    }
}

/**
 * Crea un nuevo explorador de mundos en el editor de Gobu.
 *
 * @return Un nuevo widget que representa el explorador de mundos.
 */
GtkWidget *gapp_browser_new(void)
{
    return g_object_new(GAPP_TYPE_BROWSER, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}
