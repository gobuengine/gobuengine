#include "gobu_script_editor.h"
#include "config.h"
#include "gobu-app.h"
#include "gobu_widget.h"

static GtkWidget *view_source_buffer, *view_source;
static GtkSingleSelection *list_component_selection;
static guint script_selected_i = 0;

extern GobuApp *APP;

/**
 * Maneja la respuesta a la señal de eliminación de un componente en una lista de scripts en Gobu.
 *
 * @param dialog    El diálogo que generó la respuesta.
 * @param response  El identificador de respuesta de la señal.
 * @param data      Datos relacionados con la eliminación del componente.
 */
static void gobu_script_list_component_signal_remove_response(GtkDialog *dialog, int response, gpointer data)
{
    g_assert(APP->Module.Script.list);
    g_assert(APP->Module.Script.table);
    g_assert(list_component_selection);

    if (response == GTK_RESPONSE_OK)
    {
        const gchar *key = gtk_string_list_get_string(APP->Module.Script.list, script_selected_i);
        g_return_if_fail(g_hash_table_remove(APP->Module.Script.table, key));

        const id = (script_selected_i - 1) == -1 ? 0 : (script_selected_i - 1);
        gtk_string_list_remove(APP->Module.Script.list, script_selected_i);
        gtk_single_selection_set_selected(list_component_selection, id);

        g_print("GOBU: Module-Script = RemoveScript::[ %s ]\n", key);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * Maneja la señal de eliminación de un componente en una lista de scripts en Gobu.
 *
 * @param button  El botón que desencadenó la eliminación del componente.
 * @param data    Datos relacionados con la eliminación del componente.
 */
static void gobu_script_list_component_signal_remove(GtkWidget *button, gpointer data)
{
    const gchar *key = gtk_string_list_get_string(APP->Module.Script.list, script_selected_i);
    gobu_widget_dialog_confirm_delete(button, key, NULL, gobu_script_list_component_signal_remove_response);
}

/**
 * Maneja la respuesta a la señal de creación de componente en una lista de scripts en Gobu.
 *
 * @param dialog    El diálogo que generó la respuesta.
 * @param response  El identificador de respuesta de la señal.
 * @param data      Datos relacionados con la señal de creación de componente.
 */
static void gobu_script_list_component_signal_create_response(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        const gchar *key = gobu_widget_dialog_input_get_text(dialog);
        g_return_if_fail(!g_hash_table_contains(APP->Module.Script.table, key));
        gtk_string_list_append(APP->Module.Script.list, key);
        g_hash_table_insert(APP->Module.Script.table, key, "");

        g_print("GOBU: Module-Script = CreateScript::[ %s ]\n", key);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * Crea un componente en Gobu en respuesta a la señal de un botón.
 *
 * @param button     El botón que desencadenó la creación del componente.
 * @param component  El sistema de componentes en el que se crea el componente.
 */
static void gobu_script_list_component_signal_create(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = gobu_widget_dialog_input(button, "Create new Component", "New");
    g_signal_connect(dialog, "response", G_CALLBACK(gobu_script_list_component_signal_create_response), NULL);
}

/**
 * Guarda un componente virtual en un búfer de texto en Gobu.
 *
 * @param self       El búfer de texto en el que se guarda el componente virtual.
 * @param component  El sistema de componentes relacionado con el componente virtual.
 */
static void gobu_script_list_component_signal_save_virtual(GtkTextBuffer *self, gpointer data)
{
    if (script_selected_i != -1)
    {
        GtkTextIter start, end;

        gtk_text_buffer_get_bounds(view_source_buffer, &start, &end);
        gchar *buffer = gtk_text_buffer_get_text(view_source_buffer, &start, &end, TRUE);
        gchar *key = gtk_string_list_get_string(APP->Module.Script.list, script_selected_i);
        g_hash_table_replace(APP->Module.Script.table, key, buffer);
    }
}

/**
 * Maneja la señal de selección de un componente de lista en un script Gobu.
 *
 * @param self       La vista de lista que emitió la señal.
 * @param position   La posición del componente de lista seleccionado.
 * @param data       Datos relacionados con la señal de selección.
 */
static void gobu_script_list_component_signal_selected(GtkSingleSelection *self, gpointer data)
{
    script_selected_i = gtk_single_selection_get_selected(self);
    gtk_widget_set_sensitive(view_source, script_selected_i != -1);

    if (script_selected_i == -1)
    {
        gtk_text_buffer_set_text(view_source_buffer, "", 0);
        return;
    }

    gchar *key = gtk_string_list_get_string(APP->Module.Script.list, script_selected_i);
    gchar *script_buffer = g_hash_table_lookup(APP->Module.Script.table, key);
    gtk_text_buffer_set_text(view_source_buffer, script_buffer, strlen(script_buffer));
    g_print("GOBU: Module-Script = SelectedScript::[ %s:%d ]\n", key, script_selected_i);
}

/**
 * Configura un componente de señal en Gobu para una fábrica de elementos de lista.
 *
 * @param self       La fábrica de elementos de lista en la que se configura el componente de señal.
 * @param listitem   El elemento de lista al que se le aplica la configuración.
 * @param user_data  Datos de usuario opcionales para la configuración del componente.
 */
static void gobu_script_list_component_signal_setup(GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data)
{
    GtkWidget *box, *icon, *label;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_list_item_set_child(listitem, box);

    icon = gtk_image_new_from_icon_name("application-x-addon-symbolic");
    gtk_box_append(box, icon);

    label = gtk_label_new(NULL);
    gtk_box_append(box, label);
}

/**
 * Vincula un componente de señal en Gobu a un elemento de lista en una fábrica de elementos de lista.
 *
 * @param self       La fábrica de elementos de lista en la que se realiza la vinculación.
 * @param listitem   El elemento de lista al que se vincula el componente de señal.
 * @param user_data  Datos de usuario opcionales para la vinculación del componente.
 */
static void gobu_script_list_component_signal_bind(GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data)
{
    GtkWidget *box = gtk_list_item_get_child(listitem);
    GtkStringObject *strobj = gtk_list_item_get_item(listitem);

    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_next_sibling(icon);

    gtk_label_set_text(GTK_LABEL(label), gtk_string_object_get_string(strobj));
}

/**
 * Carga la lista de componentes de un script en Gobu.
 */
static void gobu_script_list_component_load(void)
{
    g_print("GOBU: List Component init\n");

    gchar *file_gcomponent_path = g_build_filename(gobu_project_get_path(), "Scripts", "game.gcomponent", NULL);
    APP->Module.Script.data = gobu_json_load_from_file(file_gcomponent_path);
    g_free(file_gcomponent_path);

    APP->Module.Script.list = gtk_string_list_new(NULL);
    APP->Module.Script.table = g_hash_table_new(g_str_hash, g_str_equal);

    int count = binn_count(APP->Module.Script.data);
    for (int i = 1; i <= count; i++)
    {
        binn *cmp = binn_list_object(APP->Module.Script.data, i);
        char *keys = binn_object_str(cmp, "name");
        char *code = binn_object_str(cmp, "code");

        gtk_string_list_append(APP->Module.Script.list, keys);
        g_hash_table_insert(APP->Module.Script.table, keys, code);
    }
}

/**
 * Obtiene una lista de componentes de un script en Gobu.
 *
 * @return Un widget que representa la lista de componentes del script.
 */
static GtkWidget *gobu_script_list_component(void)
{
    GtkWidget *vbox;
    GtkWidget *view, *toolbar, *scroll, *item;
    GtkListItemFactory *factory;
    GListStore *store;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    {
        toolbar = gobu_widget_toolbar_new();
        gtk_box_append(vbox, toolbar);
        {
            item = gobu_widget_button_new_icon_with_label("list-add-symbolic", "Add");
            gtk_box_append(GTK_BOX(toolbar), item);
            g_signal_connect(item, "clicked", G_CALLBACK(gobu_script_list_component_signal_create), NULL);

            gobu_widget_toolbar_separator_new(toolbar);

            item = gobu_widget_button_new_icon_with_label("user-trash-full-symbolic", "Remove");
            gtk_box_append(GTK_BOX(toolbar), item);
            g_signal_connect(item, "clicked", G_CALLBACK(gobu_script_list_component_signal_remove), NULL);
        }
    }

    {
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(scroll), DOCK_L_MIN_SIZE, -1);
        gtk_box_append(vbox, scroll);

        list_component_selection = gtk_single_selection_new(G_LIST_MODEL(APP->Module.Script.list));
        gtk_single_selection_set_autoselect(list_component_selection, FALSE);
        gtk_single_selection_set_can_unselect(list_component_selection, TRUE);
        gtk_single_selection_set_selected(list_component_selection, GTK_INVALID_LIST_POSITION);
        g_signal_connect(list_component_selection, "notify::selected", G_CALLBACK(gobu_script_list_component_signal_selected), NULL);

        factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(gobu_script_list_component_signal_setup), NULL);
        g_signal_connect(factory, "bind", G_CALLBACK(gobu_script_list_component_signal_bind), NULL);

        view = gtk_list_view_new(list_component_selection, factory);
        // gtk_list_view_set_single_click_activate(view, TRUE);
        // gtk_list_view_set_show_separators(view, TRUE);
        gtk_widget_add_css_class(view, "navigation-sidebar");
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    }

    return vbox;
}

/**
 * Muestra la vista de código fuente de un script en Gobu.
 *
 * @return Un widget que representa la vista de código fuente del script.
 */
static GtkWidget *gobu_script_view_source(void)
{
    GtkWidget *view_source_scroll;

    view_source_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(view_source_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(view_source_scroll, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(view_source_scroll), DOCK_L_MIN_SIZE, -1);
    {

        view_source = gtk_source_view_new();
        gtk_source_view_set_show_line_numbers(view_source, TRUE);
        gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view_source), 10);
        gtk_text_view_set_top_margin(GTK_TEXT_VIEW(view_source), 10);

        view_source_buffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view_source)));
        gtk_source_buffer_set_style_scheme(view_source_buffer, APP->setting.scripts.style_scheme);
        gtk_source_buffer_set_language(view_source_buffer, APP->setting.scripts.source_language);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(view_source_scroll), view_source);
        g_signal_connect(view_source_buffer, "changed", G_CALLBACK(gobu_script_list_component_signal_save_virtual), NULL);
    }

    return view_source_scroll;
}

/**
 * Inicializa un script en Gobu.
 *
 * @return Un nuevo widget que representa el script inicializado.
 */
static GtkWidget *gobu_script_editor_init(void)
{
    GtkWidget *vbox, *toolbar, *item;
    GtkWidget *hpaned, *vpaned_start, *vpaned_end;
    GtkWidget *view_source, *view_source_scroll, *view_source_buffer;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    {
        toolbar = gobu_widget_toolbar_new();
        gtk_box_append(vbox, toolbar);
        {
            item = gobu_widget_button_new_icon_with_label("media-removable-symbolic", "Save");
            gtk_box_append(toolbar, item);

            gobu_widget_toolbar_separator_new(toolbar);

            item = gobu_widget_button_new_icon_with_label("media-view-subtitles-symbolic", "Build");
            gtk_box_append(toolbar, item);

            gobu_widget_toolbar_separator_new(toolbar);

            item = gobu_widget_button_new_icon_with_label("input-gaming-symbolic", "Preview");
            gtk_box_append(toolbar, item);
        }

        hpaned = gobu_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
        gtk_box_append(vbox, hpaned);
        {
            vpaned_start = gobu_widget_paned_new_with_notebook(GTK_ORIENTATION_VERTICAL, TRUE,
                                                               gtk_label_new("Components"), gobu_script_list_component(),
                                                               NULL, NULL);
            gtk_paned_set_start_child(GTK_PANED(hpaned), vpaned_start);

            vpaned_end = gobu_widget_paned_new_with_notebook(GTK_ORIENTATION_VERTICAL, TRUE,
                                                             gtk_label_new("View"), gobu_script_view_source(), NULL, NULL);
            gtk_paned_set_end_child(GTK_PANED(hpaned), vpaned_end);
        }
    }

    return vbox;
}

/**
 * Libera los recursos del editor de scripts en Gobu.
 */
static void gobu_script_editor_free(void)
{
    g_print("GOBU: List Component free\n");
    g_hash_table_destroy(APP->Module.Script.table);
    g_object_unref(APP->Module.Script.list);
    binn_free(APP->Module.Script.data);
}

/**
 * Crea un nuevo editor de scripts en Gobu.
 *
 * @return Un nuevo widget que representa el editor de scripts.
 */
GtkWidget *gobu_script_editor_new(void)
{
    gobu_script_list_component_load();
    return gobu_script_editor_init();
}
