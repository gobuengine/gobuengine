#include "browser.h"
#include "gapp_level_editor.h"
#include "gapp_script_editor.h"
#include "gapp_tool_console.h"

#include "gapp_main.h"
#include "gapp_widget.h"

#include "gobu_gobu.h"
#include "gobu_project.h"

enum
{
    ACTION_CREATE_FOLDER,
    ACTION_CREATE_ENTITY,
    ACTION_CREATE_LEVEL,
    ACTION_LAST
};

typedef struct
{
    gchar* title;
    gchar* text;
    int response;
    gpointer data;
} GobuEditorWorldBrowserAction;

struct _GobuEditorWorldBrowser
{
    GtkWidget parent;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GobuEditorWorldBrowser, gb_editor_world_browser, GTK_TYPE_BOX);

static void gb_editor_world_browser_class_init(GobuEditorWorldBrowserClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    // GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    // gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
}

static GobuEditorWorldBrowserAction* gb_editor_world_browser_new_action(const gchar* title, const gchar* text, int response, gpointer data);
static void gb_editor_world_browser_fn_open_folder(GobuEditorWorldBrowser* browser, const gchar* path, gboolean bhistory_saved);
static void gb_editor_world_browser_fn_history_path_forward_clear(GobuEditorWorldBrowser* browser);
static void gb_editor_world_browser_fn_history_path_back(GobuEditorWorldBrowser* browser);
static void gb_editor_world_browser_fn_history_path_forward(GobuEditorWorldBrowser* browser);

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
static GobuEditorWorldBrowserAction* gb_editor_world_browser_new_action(const gchar* title, const gchar* text, int response, gpointer data)
{
    GobuEditorWorldBrowserAction* action = g_new0(GobuEditorWorldBrowserAction, 1);
    action->title = gb_strdup(title);
    action->text = gb_strdup(text);
    action->response = response;
    action->data = data;
    return action;
}

/**
 * Obtiene el icono de un archivo en el explorador de mundos del editor en Gobu.
 *
 * @param image      El widget de imagen en el que se muestra el icono del archivo.
 * @param info_file  La información del archivo para la obtención del icono.
 */
static void gb_editor_world_browser_fn_get_icon_file(GtkWidget* image, GFileInfo* info_file)
{
    GIcon* icon;
    const char* ext_file = g_file_info_get_name(info_file);

    if (gb_fs_is_extension(ext_file, ".png") || gb_fs_is_extension(ext_file, ".jpg"))
    {
        GFile* file = G_FILE(g_file_info_get_attribute_object(info_file, "standard::file"));
        gtk_image_set_from_file(image, g_file_get_path(file));
        return;
    }
    else if (gb_fs_is_extension(ext_file, ".gbscript"))
    {
        icon = EditorCore->resource.icons[GOBU_RESOURCE_ICON_COMPONENT];
    }
    else if (gb_fs_is_extension(ext_file, ".level"))
    {
        icon = EditorCore->resource.icons[GOBU_RESOURCE_ICON_LEVEL];
    }
    else if (gb_fs_is_extension(ext_file, ".prefab"))
    {
        icon = EditorCore->resource.icons[GOBU_RESOURCE_ICON_ENTITY];
    }
    else
    {
        icon = g_file_info_get_icon(info_file);
    }

    gtk_image_set_from_gicon(image, icon);
}

/**
 * Abre una carpeta en el explorador de mundos del editor en Gobu.
 *
 * @param browser          El explorador de mundos en el que se abre la carpeta.
 * @param path             La ruta de la carpeta que se abre.
 * @param bhistory_saved   TRUE si la historia de navegación se guarda, FALSE en caso contrario.
 */
static void gb_editor_world_browser_fn_open_folder(GobuEditorWorldBrowser* browser, const gchar* path, gboolean bhistory_saved)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(browser);

    // HISTORIAL DE PATH
    if (bhistory_saved)
        g_ptr_array_add(private->path_back, gb_strdup(private->path_current));

    g_free(private->path_current);
    private->path_current = gb_strdup(path);

    GFile* file = g_file_new_for_path(private->path_current);
    gboolean is_home = (g_strcmp0(gb_path_basename(file), FOLDER_CONTENT_PROJECT) != 0);
    gtk_directory_list_set_file(private->directory_list, file);
    // g_object_unref(file);

    // EL BOTON DEL NAV (NEXT)RIGHT SE DESHABILITA SIEMPRE QUE CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
    //  EL BOTON DE NAV (PREV)LEFT SE HABILITA CUANDO CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
    //  EL BOTON DE NAV (HOME) SE HABILITA SI NO ESTAMOS EN CONTENT
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), is_home);

    gb_print_info(TF("Open folder: %s", gb_path_basename(private->path_current)));
}

/**
 * Limpia el historial de rutas hacia adelante en el explorador de mundos del editor en Gobu.
 *
 * @param browser  El explorador de mundos en el que se limpia el historial.
 */
static void gb_editor_world_browser_fn_history_path_forward_clear(GobuEditorWorldBrowser* browser)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(browser);

    if (private->path_back->len == 0 && private->path_forward != 0)
    {
        g_ptr_array_remove_range(private->path_forward, 0, private->path_forward->len);
    }
}

/**
 * Navega hacia atrás en el historial de rutas en el explorador de mundos del editor en Gobu.
 *
 * @param browser  El explorador de mundos en el que se navega hacia atrás en el historial.
 */
static void gb_editor_world_browser_fn_history_path_back(GobuEditorWorldBrowser* browser)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(browser);

    guint index_last = (private->path_back->len - 1);
    const gchar* path = g_ptr_array_index(private->path_back, index_last);
    g_ptr_array_remove_index(private->path_back, index_last);
    g_ptr_array_add(private->path_forward, gb_strdup(private->path_current));

    gb_editor_world_browser_fn_open_folder(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), !(private->path_back->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), TRUE);
}

/**
 * Navega hacia adelante en el historial de rutas en el explorador de mundos del editor en Gobu.
 *
 * @param browser  El explorador de mundos en el que se navega hacia adelante en el historial.
 */
static void gb_editor_world_browser_fn_history_path_forward(GobuEditorWorldBrowser* browser)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(browser);

    guint index_last = (private->path_forward->len - 1);
    const gchar* path = g_ptr_array_index(private->path_forward, index_last);
    g_ptr_array_remove_index(private->path_forward, index_last);
    g_ptr_array_add(private->path_back, gb_strdup(private->path_current));

    gb_editor_world_browser_fn_open_folder(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), !(private->path_forward->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
}

/**
 * Maneja la señal de borrar elementos en el explorador de mundos del editor en Gobu.
 *
 */
static void signal_delete_file_response(GtkWidget* widget, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GError* error = NULL;
        GFile* file = G_FILE(data);
        if (!g_file_delete(file, NULL, &error) && !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        {
            gapp_widget_alert(widget, error->message);
            gb_print_error(TF("Failed to delete %s\n", g_file_peek_path(file)), error->message);
            return;
        }

        gb_print_success(TF("File delete %s\n", g_file_peek_path(file)));
    }

    gtk_window_destroy(widget);
}

void signal_delete_file(GtkWidget* widget, gpointer data)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(data);

    GFileInfo* file_selected = G_FILE_INFO(gtk_single_selection_get_selected_item(private->selection));
    GFile* file = G_FILE(g_file_info_get_attribute_object(file_selected, "standard::file"));

    gapp_widget_dialog_confirm_delete(widget, g_file_info_get_name(file_selected), file, signal_delete_file_response);
    gtk_popover_popdown(GTK_POPOVER(private->popover));
}

/**
 * Maneja la señal de creación de elementos en el explorador de mundos del editor en Gobu.
 *
 */
static void signal_create_item_action_response(GtkDialog* dialog, int response, const GobuEditorWorldBrowserAction* action)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(action->data);
        const gchar* result = gapp_widget_dialog_input_get_text(dialog);

        if (action->response == ACTION_CREATE_FOLDER) {
            gchar* path_result = gb_path_join(private->path_current, result, NULL);
            gb_fs_mkdir(path_result);
            g_free(path_result);

            gb_print_info(TF("Create Folder: %s", result));
        }
        else if (action->response == ACTION_CREATE_LEVEL) {
            gchar* path_result = gb_path_join(private->path_current, TF("%s.level", result), NULL);
            g_file_create_readwrite(g_file_new_for_path(path_result), G_FILE_CREATE_NONE, NULL, NULL);
            g_free(path_result);

            gb_print_info(TF("Create Level: %s", result));
        }
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void signal_create_item_action(GtkWidget* button, const GobuEditorWorldBrowserAction* action)
{
    GtkWidget* dialog = gapp_widget_dialog_input(button, action->title, action->text);
    g_signal_connect(dialog, "response", G_CALLBACK(signal_create_item_action_response), action);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

/**
 * Maneja la señal de activación de un popover en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la activación del popover.
 * @param data    Datos relacionados con la activación del popover en el explorador de mundos.
 */
static void signal_popover(GtkWidget* button, gpointer data)
{
    GtkWidget* popover, * box, * item;

    popover = gtk_popover_new();
    gtk_widget_set_parent(popover, button);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_popover_set_child(GTK_POPOVER(popover), box);
    {
        item = gtk_button_new_with_label("Import to /Game");
        gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
        gtk_box_append(GTK_BOX(box), item);

        gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

        item = gtk_button_new_with_label("New Folder");
        gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
        gtk_box_append(GTK_BOX(box), item);
        g_signal_connect(item, "clicked", G_CALLBACK(signal_create_item_action),
        gb_editor_world_browser_new_action("Create New Folder", "New Folder", ACTION_CREATE_FOLDER, data));

        gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

        item = gtk_button_new_with_label("New Level");
        gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
        gtk_box_append(GTK_BOX(box), item);
        g_signal_connect(item, "clicked", G_CALLBACK(signal_create_item_action),
        gb_editor_world_browser_new_action("Create New Level", "New Level", ACTION_CREATE_LEVEL, data));
    }

    gtk_popover_popup(GTK_POPOVER(popover));
}

/**
 * Maneja la señal de activación de un popover en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la activación del popover.
 * @param data    Datos relacionados con la activación del popover en el explorador de mundos.
 */
static void signal_view_file_popover(GtkGesture* gesture, int n_press, double x, double y, gpointer data)
{
    GtkWidget* widget, * child;
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(data);

    widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);

    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    {
        GtkWidget* popover, * box, * item;

        gint p = GPOINTER_TO_UINT(g_object_get_data(child, "position_id"));
        gtk_single_selection_set_selected(private->selection, p);

        private->popover = gtk_popover_new();
        gtk_popover_set_cascade_popdown(GTK_POPOVER(private->popover), TRUE);
        gtk_widget_set_parent(GTK_POPOVER(private->popover), widget);
        gtk_popover_set_has_arrow(GTK_POPOVER(private->popover), FALSE);
        gtk_popover_set_pointing_to(GTK_POPOVER(private->popover), &(GdkRectangle) { x, y, 1, 1 });

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_popover_set_child(GTK_POPOVER(private->popover), box);
        {
            if (child != NULL && child != widget)
            {
                item = gtk_button_new_with_label("Delete");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(signal_delete_file), data);
            }
            else
            {
                item = gtk_button_new_with_label("Import to /Game");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);

                gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                item = gtk_button_new_with_label("New Folder");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(signal_create_item_action),
                gb_editor_world_browser_new_action("Create New Folder", "New Folder", ACTION_CREATE_FOLDER, data));

                gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                item = gtk_button_new_with_label("New Level");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(signal_create_item_action),
                gb_editor_world_browser_new_action("Create New Level", "New Level", ACTION_CREATE_LEVEL, data));
            }
        }

        gtk_popover_popup(GTK_POPOVER(private->popover));
    }

    if (child == widget)
    {
        gtk_single_selection_set_selected(private->selection, -1);
    }
}

static void gb_fs_copyc(GFile* src, GFile* dest)
{
    // Es un directorio
    if (gb_path_exist(g_file_get_path(src))) {
        if (gb_fs_mkdir(g_file_get_path(dest))) {
            GFileEnumerator* enumerator = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);
            GFileInfo* file_info = NULL;
            while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL)
            {
                gchar* file_name = g_file_info_get_name(file_info);
                GFile* file_src = g_file_new_for_path(gb_path_join(g_file_get_path(src), file_name, NULL));
                GFile* file_dest = g_file_new_for_path(gb_path_join(g_file_get_path(dest), file_name, NULL));
                gb_fs_copyc(file_src, file_dest);
            }
        }
        return;
    }

    // Copiamos el archivo
    GError* error = NULL;
    gchar* name = g_file_get_basename(src);

    if (g_file_copy(src, dest, G_FILE_COPY_NONE, NULL, NULL, NULL, &error))
    {
        gb_print_success(TF("Copy file: [%s]", name));
    }
    else
    {
        gb_print_error(TF("Copy [%s]", name), error->message);
    }
}

/**
 * @brief Función que maneja el evento de soltar un archivo en la vista.
 *
 * @param target El objeto GtkDropTarget que recibió el evento.
 * @param value El valor del archivo que se soltó.
 * @param x La posición horizontal del cursor en la vista.
 * @param y La posición vertical del cursor en la vista.
 * @param user_data Datos adicionales proporcionados por el usuario.
 * @return gboolean Devuelve TRUE si el evento fue manejado correctamente, FALSE en caso contrario.
 */
static gboolean signal_view_file_drop(GtkDropTarget* target, const GValue* value, double x, double y, GobuEditorWorldBrowser* browser)
{
    if (G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
    {
        GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(browser);

        GList* list_files = g_value_get_boxed(value);
        for (int i = 0; i < g_list_length(list_files); i++)
        {
            GFile* file_src = G_FILE(g_list_nth_data(list_files, i));
            GFile* file_dest = g_file_new_for_path(gb_path_join(private->path_current, g_file_get_basename(file_src), NULL));
            gb_fs_copyc(file_src, file_dest);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * Maneja la señal de navegación a la página de inicio en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la navegación a la página de inicio.
 * @param data    Datos relacionados con la navegación a la página de inicio en el explorador de mundos.
 */
static void signal_navhome(GtkWidget* button, gpointer data)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(data);
    gb_editor_world_browser_fn_open_folder(data, private->path_default, TRUE);
}

/**
 * Maneja la señal de navegación hacia atrás en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la navegación hacia atrás.
 * @param data    Datos relacionados con la navegación hacia atrás en el explorador de mundos.
 */
static void signal_navback(GtkWidget* button, gpointer data)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(data);
    gb_editor_world_browser_fn_history_path_back(data);
}

/**
 * Maneja la señal de navegación hacia adelante en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la navegación hacia adelante.
 * @param data    Datos relacionados con la navegación hacia adelante en el explorador de mundos.
 */
static void signal_navforward(GtkWidget* button, gpointer data)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(data);
    gb_editor_world_browser_fn_history_path_forward(data);
}

/**
 * Prepara la fuente de arrastre en el explorador de mundos del editor en Gobu.
 *
 * @param source      La fuente de arrastre que se prepara.
 * @param x           La posición X del puntero en el momento del arrastre.
 * @param y           La posición Y del puntero en el momento del arrastre.
 * @param list_item   El elemento de lista asociado al arrastre.
 *
 * @return Un proveedor de contenido Gdk que representa los datos de arrastre preparados.
 */
static GdkContentProvider* signal_drag_source_prepare(GtkDragSource* source, gdouble x, gdouble y, GtkListItem* list_item)
{
    GtkWidget* box = gtk_list_item_get_child(list_item);

    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(g_object_get_data(box, "BrowserContent"));

    guint pos_selected = GPOINTER_TO_UINT(g_object_get_data(box, "position_id"));
    gtk_single_selection_set_selected(private->selection, pos_selected);

    GFileInfo* info = G_FILE_INFO(gtk_list_item_get_item(list_item));
    GFile* file = G_FILE(g_file_info_get_attribute_object(info, "standard::file"));

    return gdk_content_provider_new_union((GdkContentProvider * [2]) {
        gdk_content_provider_new_typed(G_TYPE_FILE, file),
            gdk_content_provider_new_typed(G_TYPE_FILE_INFO, info),
    }, 2);
}

/**
 * Inicia la fuente de arrastre en el explorador de mundos del editor en Gobu.
 *
 * @param source      La fuente de arrastre que se inicia.
 * @param drag        El objeto de arrastre asociado.
 * @param list_item   El elemento de lista que se arrastra.
 *
 * @return Un proveedor de contenido Gdk que representa los datos de arrastre iniciales.
 */
static void signal_drag_source_begin(GtkDragSource* source, GdkDrag* drag, GtkListItem* list_item)
{
    GtkWidget* box = gtk_list_item_get_child(list_item);
    GtkWidget* paintable = gtk_widget_paintable_new(gtk_widget_get_first_child(box));
    gtk_drag_source_set_icon(source, paintable, 0, 0);
    g_object_unref(paintable);
}

/**
 * Maneja la señal de selección de archivo en la vista de archivos del explorador de mundos del editor en Gobu.
 *
 * @param self         La vista de archivos que generó la señal de selección.
 * @param position     La posición del archivo seleccionado.
 * @param user_data    Datos de usuario opcionales relacionados con la selección de archivo.
 */
static void signal_view_file_selected(GtkGridView* self, guint position, gpointer user_data)
{
    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(user_data);

    /* OBTENEMOS LA INFO DEL ARCHIVO SELECCIONADO CON DOUBLE-CLIC */
    GFileInfo* file_info = g_list_model_get_item(G_LIST_MODEL(gtk_grid_view_get_model(self)), position);

    GFile* file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));
    gchar* filename = g_file_get_path(file);

    /* COMPROBAMOS SI ES UN DIRECTORIO Y ENTRAMOS */
    if (g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY)
    {
        gb_editor_world_browser_fn_history_path_forward_clear(user_data);
        gb_editor_world_browser_fn_open_folder(user_data, filename, TRUE);

        // g_object_unref(file);
    }
    else if (g_file_info_get_file_type(file_info) == G_FILE_TYPE_REGULAR) {
        if (gb_fs_is_extension(filename, ".level")) {
            gapp_level_editor_new(filename);
        }
        else if (gb_fs_is_extension(filename, ".gbscript")) {
            gapp_editor_script_new(filename);
        }
        else {
            return;
        }

        gb_print_info(TF("Open file: %s", gb_path_basename(filename)));
    }
}

/**
 * Configura la vista de archivo en el explorador de mundos del editor en Gobu.
 *
 * @param factory     La fábrica de elementos de lista en la que se realiza la configuración.
 * @param list_item   El elemento de lista al que se aplica la configuración de la vista de archivo.
 * @param user_data   Datos de usuario opcionales para la configuración de la vista de archivo.
 */
static void signal_setup_view_file(GtkListItemFactory* factory, GtkListItem* list_item, gpointer user_data)
{
    GtkWidget* box, * imagen, * label, * entry;
    GtkExpression* expression;
    GtkDragSource* source;
    // expression = gtk_constant_expression_new(GTK_TYPE_LIST_ITEM, list_item);

    //GFileInfo *info_file = gtk_list_item_get_item(list_item);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_list_item_set_child(list_item, box);
    gtk_widget_set_size_request(box, 64, 84);

    imagen = gtk_image_new();
    gtk_image_set_icon_size(imagen, GTK_ICON_SIZE_LARGE);
    gtk_box_append(GTK_BOX(box), imagen);

    label = gtk_label_new(NULL);
    gtk_label_set_wrap_mode(label, PANGO_WRAP_CHAR);
    gtk_label_set_justify(label, GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(box), label);

    g_object_set_data(box, "BrowserContent", user_data);

    source = gtk_drag_source_new();
    gtk_drag_source_set_actions(source, GDK_ACTION_COPY);
    g_signal_connect(source, "prepare", G_CALLBACK(signal_drag_source_prepare), list_item);
    g_signal_connect(source, "drag-begin", G_CALLBACK(signal_drag_source_begin), list_item);
    gtk_widget_add_controller(box, GTK_EVENT_CONTROLLER(source));
}

/**
 * Vincula la vista de archivo en el explorador de mundos del editor en Gobu.
 *
 * @param factory     La fábrica de elementos de lista en la que se realiza la vinculación.
 * @param list_item   El elemento de lista al que se vincula la vista de archivo.
 * @param user_data   Datos de usuario opcionales para la vinculación de la vista de archivo.
 */
void signal_bind_view_file(GtkListItemFactory* factory, GtkListItem* list_item, gpointer user_data)
{
    GtkWidget* box, * label_name, * image;

    box = gtk_list_item_get_child(list_item);
    g_return_if_fail(box);

    GFileInfo* info_file = gtk_list_item_get_item(list_item);
    gint id = gtk_list_item_get_position(list_item);

    image = gtk_widget_get_first_child(box);
    label_name = gtk_widget_get_next_sibling(image);

    const char* name = g_file_info_get_name(info_file);
    gtk_label_set_label(GTK_LABEL(label_name), gb_fs_get_name(name, true));
    gtk_widget_set_tooltip_text(box, name);

    gb_editor_world_browser_fn_get_icon_file(image, info_file);
    // TODO: Buscar una mejor solucion
    g_object_set_data(image, "position_id", GINT_TO_POINTER(id));
    g_object_set_data(label_name, "position_id", GINT_TO_POINTER(id));
    g_object_set_data(box, "position_id", GINT_TO_POINTER(id));
}

/**
 * Inicializa el explorador de mundos del editor en Gobu.
 *
 * @param self  El objeto del explorador de mundos que se inicializa.
 */
static void gb_editor_world_browser_init(GobuEditorWorldBrowser* self)
{
    GtkWidget* vbox;
    GtkWidget* toolbar, * item;

    GobuEditorWorldBrowserPrivate* private = gb_editor_world_browser_get_instance_private(self);

    private->path_back = g_ptr_array_new();
    private->path_forward = g_ptr_array_new();
    private->path_current = gb_path_join(gb_project_get_path(), FOLDER_CONTENT_PROJECT, NULL);
    private->path_default = gb_strdup(private->path_current);

    printf("PATH: %s\n", private->path_current);

    // Toolbar
    {
        toolbar = gapp_widget_toolbar_new();
        gapp_widget_set_margin(toolbar, 0);
        gtk_box_append(self, toolbar);
        {
            item = gapp_widget_button_new_icon_with_label("list-add-symbolic", "Add");
            gtk_button_set_has_frame(item, FALSE);
            gtk_box_append(GTK_BOX(toolbar), item);
            g_signal_connect(item, "clicked", G_CALLBACK(signal_popover), self);
            gapp_widget_toolbar_separator_new(toolbar);

            item = gapp_widget_button_new_icon_with_label("insert-image-symbolic", "Import");
            gtk_button_set_has_frame(item, FALSE);
            gtk_box_append(GTK_BOX(toolbar), item);
            // g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalContentBrowserPopover), self);
            // gapp_widget_toolbar_separator_new(toolbar);

            // item = gapp_widget_button_new_icon_with_label("media-flash-symbolic", "Save All");
            // gtk_button_set_has_frame(item, FALSE);
            // gtk_box_append(GTK_BOX(toolbar), item);
            // g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalContentBrowserPopover), self);
        }
    }

    gtk_box_append(self, gapp_widget_separator_h());

    // Toolbar Nav
    {
        toolbar = gapp_widget_toolbar_new();
        gapp_widget_set_margin(toolbar, 0);
        gtk_box_append(self, toolbar);
        {
            private->btn_nav_home = gapp_widget_button_new_icon_with_label("go-home-symbolic", "Home");
            gtk_button_set_has_frame(private->btn_nav_home, FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), FALSE);
            gtk_box_append(toolbar, private->btn_nav_home);
            g_signal_connect(private->btn_nav_home, "clicked", G_CALLBACK(signal_navhome), self);
            gapp_widget_toolbar_separator_new(toolbar);

            private->btn_nav_back = gapp_widget_button_new_icon_with_label("go-previous-symbolic", NULL);
            gtk_button_set_has_frame(private->btn_nav_back, FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), FALSE);
            gtk_box_append(toolbar, private->btn_nav_back);
            g_signal_connect(private->btn_nav_back, "clicked", G_CALLBACK(signal_navback), self);
            gapp_widget_toolbar_separator_new(toolbar);

            private->btn_nav_forward = gapp_widget_button_new_icon_with_label("go-next-symbolic", NULL);
            gtk_button_set_has_frame(private->btn_nav_forward, FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
            gtk_box_append(GTK_BOX(toolbar), private->btn_nav_forward);
            g_signal_connect(private->btn_nav_forward, "clicked", G_CALLBACK(signal_navforward), self);
        }
    }

    gtk_box_append(self, gapp_widget_separator_h());

    // View File
    {
        GtkWidget* scroll, * grid;
        GtkSingleSelection* selection;
        GtkGesture* gesture;
        GtkDropTarget* drop;
        GFile* file;

        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                       GTK_POLICY_AUTOMATIC,
                                       GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(self, scroll);

        file = g_file_new_for_path(private->path_default);
        private->directory_list = gtk_directory_list_new("standard::*", file);
        // gtk_directory_list_set_monitored(private->directory_list, TRUE);
        // gtk_directory_list_set_io_priority(private->directory_list, G_PRIORITY_LOW);
        g_object_unref(file);

        private->selection = gtk_single_selection_new(G_LIST_MODEL(private->directory_list));
        gtk_single_selection_set_can_unselect(private->selection, TRUE);
        gtk_single_selection_set_autoselect(private->selection, FALSE);

        private->factory = gtk_signal_list_item_factory_new();
        g_signal_connect(private->factory, "setup", G_CALLBACK(signal_setup_view_file), self);
        g_signal_connect(private->factory, "bind", G_CALLBACK(signal_bind_view_file), self);

        // GtkWidget *view = gtk_list_view_new(selection, private->factory);
        grid = gtk_grid_view_new(private->selection, private->factory);
        // gtk_grid_view_set_single_click_activate(grid, TRUE);
        gtk_grid_view_set_max_columns(grid, 15);
        gtk_grid_view_set_min_columns(grid, 2);
        g_signal_connect(grid, "activate", G_CALLBACK(signal_view_file_selected), self);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);

        gesture = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
        g_signal_connect(gesture, "released", G_CALLBACK(signal_view_file_popover), self);
        gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(gesture));

        drop = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
        g_signal_connect(drop, "drop", G_CALLBACK(signal_view_file_drop), self);
        gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(drop));
    }
}

/**
 * Crea un nuevo explorador de mundos en el editor de Gobu.
 *
 * @return Un nuevo widget que representa el explorador de mundos.
 */
GtkWidget* gb_editor_world_browser_new(void)
{
    return g_object_new(GOBU_EDITOR_TYPE_WORLD_BROWSER, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}
