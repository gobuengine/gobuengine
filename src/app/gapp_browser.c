#include "gapp_browser.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_level_editor.h"
#include "gapp_script.h"
#include "gapp.h"

// TODO: Bug en Windows que si tienes una carpeta vacia y abierta, si la borras se crach la app.
// TODO: Soportar multiples archivos arrastrados para moverlo a un folder.
// TODO: Renombrar archivos.
// TODO: Borrar archivos.
// TODO: Mover un archivos utilizando una ventana grafica.
// TODO: MenuPopu cuando presiones clic derecho en un archivo o en el view file.

struct _GappBrowser
{
    GtkWidget parent_instance;

    GtkMultiSelection *selection;
    GtkDirectoryList *directory;
    gchar *pathContent;
};

static void
gapp_browser_dispose(GObject *object);
static int gapp_browser_fn_sorting(GFileInfo *a, GFileInfo *b, gpointer data);
static GListModel *gapp_browser_fn_create_list_model(GFile *file);
static GListModel *gapp_browser_fn_create_list_model_directory(gpointer file_info, gpointer data);

static GtkWidget *gapp_browser_ui_menupopup_file(GappBrowser *self);
static void gapp_browser_s_view_file_setup_factory(GtkListItemFactory *factory, GtkListItem *list_item, gpointer data);
static void gapp_browser_s_view_file_bind_factory(GtkListItemFactory *factory, GtkListItem *list_item, GappBrowser *browser);
static void gapp_browser_ui_setup(GappBrowser *self);

// --- MARK:BASE CLASS ---

G_DEFINE_TYPE(GappBrowser, gapp_browser, GTK_TYPE_BOX)

static void gapp_browser_class_init(GappBrowserClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gapp_browser_dispose;
}

static void gapp_browser_dispose(GObject *object)
{
    GappBrowser *self = GAPP_BROWSER(object);

    g_clear_pointer(&self, gtk_widget_unparent);

    G_OBJECT_CLASS(gapp_browser_parent_class)->dispose(object);
}

static void gapp_browser_init(GappBrowser *self)
{
    g_return_if_fail(GAPP_IS_BROWSER(self));
    gapp_browser_ui_setup(self);
}
// --- END CLASS ---

// MARK: API PRIVATE

/**
 * Configura un GtkDirectoryList para monitoreo con prioridad de inactividad.
 *
 * Esta función habilita el monitoreo en el GtkDirectoryList proporcionado
 * y establece su prioridad de E/S a la prioridad predeterminada de inactividad.
 *
 * @param directory Puntero al GtkDirectoryList a configurar.
 */
static void gapp_browser_set_monitored_directory(GtkDirectoryList *directory)
{
    g_return_if_fail(GTK_IS_DIRECTORY_LIST(directory));

    gtk_directory_list_set_monitored(directory, TRUE);
    gtk_directory_list_set_io_priority(directory, G_PRIORITY_DEFAULT_IDLE);
}

/**
 * Crea un modelo de lista ordenado a partir de los archivos en un directorio.
 *
 * @param file Puntero al GFile que representa el directorio a enumerar.
 * @return Un GListModel ordenado con los archivos del directorio, o NULL en caso de error.
 *
 * Esta función enumera los archivos en el directorio especificado, los almacena
 * en un GListStore, y luego envuelve este store en un GtkSortListModel para
 * proporcionar una lista ordenada de archivos.
 */
static GListModel *gapp_browser_fn_create_list_model(GFile *file)
{
    GtkDirectoryList *list = gtk_directory_list_new("standard::*", file);
    gapp_browser_set_monitored_directory(list);
    return G_LIST_MODEL(gtk_sort_list_model_new(G_LIST_MODEL(list), gtk_custom_sorter_new(gapp_browser_fn_sorting, NULL, NULL)));
}

/**
 * Crea un modelo de lista para un directorio.
 *
 * @param file Puntero al GFile que representa el directorio.
 * @param data Datos adicionales (no utilizados en esta función).
 * @return Un GListModel si el archivo es un directorio, NULL en caso contrario.
 *
 * Esta función verifica si el archivo proporcionado es un directorio.
 * Si lo es, crea y devuelve un modelo de lista para ese directorio.
 * Si no es un directorio, devuelve NULL.
 */
static GListModel *gapp_browser_fn_create_list_model_directory(gpointer file_info, gpointer data)
{
    g_return_val_if_fail(G_FILE_INFO(file_info), NULL);
    GFileType type_a = g_file_info_get_file_type(G_FILE_INFO(file_info));

    if (type_a != G_FILE_TYPE_DIRECTORY)
        return NULL;

    GFile *file = G_FILE(g_file_info_get_attribute_object(G_FILE_INFO(file_info), "standard::file"));
    g_return_val_if_fail(file != NULL, NULL);

    return gapp_browser_fn_create_list_model(file);
}

/**
 * Ordena los archivos de directorio con las carpetas primero.
 *
 * @param a     El primer archivo a comparar.
 * @param b     El segundo archivo a comparar.
 * @param data  Datos adicionales opcionales.
 * @return      Un entero negativo si a debe ir antes que b, un entero positivo si b debe ir antes que a, o 0 si son iguales.
 */
static int gapp_browser_fn_sorting(GFileInfo *a, GFileInfo *b, gpointer data)
{
    GFileType type_a = g_file_info_get_file_type(a);
    GFileType type_b = g_file_info_get_file_type(b);

    if (type_a == G_FILE_TYPE_DIRECTORY && type_b != G_FILE_TYPE_DIRECTORY)
        return -1;
    else if (type_a != G_FILE_TYPE_DIRECTORY && type_b == G_FILE_TYPE_DIRECTORY)
        return 1;

    return 0;
}

/**
 * Obtiene la información del archivo seleccionado en el navegador.
 *
 * Esta función recorre el modelo de selección del navegador para encontrar
 * el primer elemento seleccionado y devuelve su información de archivo.
 *
 * @param self Puntero a la estructura GappBrowser.
 * @return GFileInfo* Información del archivo seleccionado, o NULL si no hay selección.
 */
static GFileInfo *gapp_browser_fn_get_selected_file(GtkSelectionModel *selection)
{
    guint position = 0;
    GListModel *model = gtk_multi_selection_get_model(GTK_MULTI_SELECTION(selection));
    guint n = g_list_model_get_n_items(G_LIST_MODEL(model));

    for (guint i = 0; i < n; i++)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(selection), i))
        {
            position = i;
            break;
        }
    }

    GtkTreeListRow *row = g_list_model_get_item(G_LIST_MODEL(model), position);
    GFileInfo *info = G_FILE_INFO(gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row)));

    return info;
}

/**
 * Convierte un GFileInfo en un GFile.
 *
 * @param info Puntero a GFileInfo del cual extraer el GFile.
 * @return Puntero a GFile extraído del GFileInfo, o NULL si no se puede obtener.
 */
static GFile *gapp_browser_fn_file_info_to_file(GFileInfo *info)
{
    g_return_val_if_fail(G_IS_FILE_INFO(info), NULL);

    GFile *file = G_FILE(g_file_info_get_attribute_object(info, "standard::file"));
    return file;
}

/**
 * Obtiene la ruta completa del elemento seleccionado basado en la entrada del diálogo.
 *
 * Esta función combina el nombre ingresado en el diálogo con la ruta del elemento
 * seleccionado actualmente para crear una ruta completa.
 *
 * @param dialog Puntero a la estructura GappWidgetDialogEntry.
 * @return gchar* La ruta completa del elemento seleccionado, o NULL si hay un error.
 *                El llamador es responsable de liberar la memoria con g_free().
 */
static gchar *gapp_browser_fn_dialog_get_selected_path(GappWidgetDialogEntry *dialog, const gchar *ext)
{
    g_return_val_if_fail(dialog != NULL && dialog->entry != NULL && dialog->data != NULL, NULL);

    GtkSelectionModel *selection;
    GFileInfo *info;
    GFile *file;

    const char *filename = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(dialog->entry)));

    if (filename == NULL || strlen(filename) == 0)
    {
        g_warning("gapp_browser_fn_dialog_get_selected_path: Folder name is empty");
        return NULL;
    }

    selection = GTK_SELECTION_MODEL(dialog->data);
    if (!GTK_IS_SELECTION_MODEL(selection))
    {
        g_warning("gapp_browser_fn_dialog_get_selected_path: Invalid selection model");
        return NULL;
    }

    info = gapp_browser_fn_get_selected_file(selection);
    if (!G_IS_FILE_INFO(info))
    {
        g_warning("gapp_browser_fn_dialog_get_selected_path: Failed to get file info");
        return NULL;
    }

    file = gapp_browser_fn_file_info_to_file(info);
    if (!G_IS_FILE(file))
    {
        g_warning("gapp_browser_fn_dialog_get_selected_path: Failed to get file");
        g_object_unref(info);
        return NULL;
    }

    char *pathfile = g_file_get_path(file);
    if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
        pathfile = g_path_get_dirname(pathfile);

    gchar *pathname = g_build_filename(pathfile, g_strdup_printf("%s%s", filename, ext), NULL);

    g_free(pathfile);
    // g_object_unref(file);
    // g_object_unref(info);

    gapp_widget_dialog_entry_text_destroy(dialog);

    return pathname;
}

/**
 * Verifica si la ruta dada corresponde a la carpeta "Content" del navegador.
 *
 * @param browser El objeto GappBrowser.
 * @param path La ruta a verificar.
 * @return TRUE si la ruta corresponde a la carpeta "Content", FALSE en caso contrario.
 */
static gboolean gapp_browser_is_content_folder(GappBrowser *browser, const gchar *path)
{
    gchar *content_path = g_build_filename(g_file_get_path(gapp_browser_get_folder(browser)), "Content", NULL);
    gboolean result = g_strcmp0(path, content_path) == 0;
    g_free(content_path);
    return result;
}

// MARK: SIGNAL

static GdkContentProvider *gapp_browser_s_file_drag(GtkDragSource *source, double x, double y, GtkListItem *list_item)
{
    g_print("Drag began\n");

    GtkTreeListRow *row = gtk_list_item_get_item(list_item);
    GFileInfo *fileInfo = gtk_tree_list_row_get_item(row);

    return gdk_content_provider_new_union((GdkContentProvider *[1]){
                                              gdk_content_provider_new_typed(G_TYPE_FILE_INFO, fileInfo),
                                          },
                                          1);
}

/**
 * Maneja la operación de arrastrar y soltar archivos en el navegador.
 *
 * Esta función procesa la lista de archivos soltados, copiándolos a la ubicación
 * del elemento seleccionado en el navegador.
 *
 * @param target El GtkDropTarget que recibió la operación de soltar.
 * @param value El GValue que contiene la lista de archivos soltados.
 * @param x Puntero a la coordenada X donde se soltaron los archivos (no utilizado).
 * @param y Puntero a la coordenada Y donde se soltaron los archivos (no utilizado).
 * @param list_item Puntero al GtkListItem donde se soltaron los archivos.
 * @return gboolean TRUE si la operación se manejó correctamente, FALSE en caso contrario.
 */
static gboolean gapp_browser_s_file_drop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer data)
{
    if (G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
    {
        GError *error = NULL;
        char *pathfile = NULL;

        // drop en el content browser
        if (GAPP_IS_BROWSER(data))
        {
            GappBrowser *browser = GAPP_BROWSER(data);
            pathfile = g_build_filename(g_file_get_path(gapp_browser_get_folder(browser)), "Content", NULL);
        }
        else
        {
            GtkTreeListRow *row = gtk_list_item_get_item(GTK_LIST_ITEM(data));
            GFileInfo *info = gtk_tree_list_row_get_item(row);
            pathfile = g_file_get_path(gapp_browser_fn_file_info_to_file(info));
        }

        if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
            pathfile = g_path_get_dirname(pathfile);

        GList *files = g_value_get_boxed(value);
        for (GList *l = files; l != NULL; l = l->next)
        {
            GFile *file_src = G_FILE(l->data);
            GFile *file_dst = g_file_new_for_path(g_build_filename(pathfile, g_file_get_basename(file_src), NULL));
            if (!gobu_fs_copyc(file_src, file_dst, &error))
            {
                g_warning("gapp_browser_s_file_drop: Failed to copy file: %s", error->message);
                g_clear_error(&error);
            }
        }

        g_free(pathfile);

        return TRUE;
    }
    else if (G_VALUE_HOLDS(value, G_TYPE_FILE_INFO))
    {
        GError *error = NULL;

        // Folder destino
        GtkTreeListRow *row = gtk_list_item_get_item(GTK_LIST_ITEM(data));
        GFileInfo *info = gtk_tree_list_row_get_item(row);

        char *pathfile = g_file_get_path(gapp_browser_fn_file_info_to_file(info));
        if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
            pathfile = g_path_get_dirname(pathfile);

        // File origen
        GFileInfo *file = g_value_get_object(value);
        GFile *file_src = gapp_browser_fn_file_info_to_file(file);
        GFile *file_dst = g_file_new_for_path(g_build_filename(pathfile, g_file_get_basename(file_src), NULL));
        if (!g_file_move(file_src, file_dst, G_FILE_COPY_NONE, NULL, NULL, NULL, &error))
        {
            g_warning("gapp_browser_s_file_drop: Failed to move file: %s", error->message);
            g_clear_error(&error);
        }

        return TRUE;
    }

    return FALSE;
}

/**
 * Manejador de evento para el clic en el botón "Añadir" de la barra de herramientas.
 *
 * Esta función se activa cuando se hace clic en el botón "Añadir" de la barra de herramientas.
 * Muestra un menú emergente (popover) asociado con el botón.
 *
 * @param widget El widget que activó el evento (el botón "Añadir").
 * @param popover El widget GtkPopover que se mostrará.
 */
static void gapp_browser_s_toolbar_add_clicked(GtkWidget *widget, GtkWidget *popover)
{
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(GTK_IS_POPOVER(popover));

    gtk_popover_popup(GTK_POPOVER(popover));
}

/**
 * Maneja la respuesta del diálogo para crear una nueva carpeta.
 *
 * Esta función se llama cuando el usuario confirma la creación de una nueva carpeta.
 * Obtiene la ruta completa de la nueva carpeta y la crea.
 *
 * @param button El botón que activó esta función (no utilizado).
 * @param dialog Puntero a la estructura GappWidgetDialogEntry.
 */
static void s_responde_create_folder(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *foldername = gapp_browser_fn_dialog_get_selected_path(dialog, "");
    if (foldername != NULL)
        g_mkdir_with_parents(foldername, 0755);
}

/**
 * Maneja el clic en el botón "Crear carpeta" de la barra de herramientas.
 *
 * Esta función crea un diálogo para que el usuario ingrese el nombre de la nueva carpeta
 * y configura el manejador para la respuesta del diálogo.
 *
 * @param widget El widget que activó el evento (el botón "Crear carpeta").
 * @param self Puntero a la estructura GappBrowser.
 */
static void gapp_browser_s_toolbar_create_folder_clicked(GtkWidget *widget, GappBrowser *self)
{
    GtkWidget *root = gtk_widget_get_root(GTK_WIDGET(self));
    g_return_if_fail(GTK_IS_WINDOW(root));

    GappWidgetDialogEntry *dialog = gapp_widget_dialog_new_entry_text(root, "New folder", "Folder name");
    dialog->data = self->selection;
    g_signal_connect(dialog->button_ok, "clicked", G_CALLBACK(s_responde_create_folder), dialog);
}

// -- Create scene new
static void s_responde_create_scene(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *filename = gapp_browser_fn_dialog_get_selected_path(dialog, ".gscene");
    if (filename != NULL)
        g_file_set_contents(filename, "{}", -1, NULL);
}

static void gapp_browser_s_toolbar_create_scene_clicked(GtkWidget *widget, GappBrowser *self)
{
    GappWidgetDialogEntry *dialog = gapp_widget_dialog_new_entry_text(gtk_widget_get_root(self), "New scene", "Scene name");
    dialog->data = self->selection;
    g_signal_connect(dialog->button_ok, "clicked", G_CALLBACK(s_responde_create_scene), dialog);
}

// -- Create script new
static void s_responde_create_script(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *filename = gapp_browser_fn_dialog_get_selected_path(dialog, BROWSER_FILE_SCRIPT);
    if (filename != NULL)
        g_file_set_contents(filename, "", -1, NULL);
}

static void gapp_browser_s_toolbar_create_script_clicked(GtkWidget *widget, GappBrowser *self)
{
    GappWidgetDialogEntry *dialog = gapp_widget_dialog_new_entry_text(gtk_widget_get_root(self), "New script", "Script name");
    dialog->data = self->selection;
    g_signal_connect(dialog->button_ok, "clicked", G_CALLBACK(s_responde_create_script), dialog);
}

// -- Remoeve file or folder
/**
 * Función de callback para manejar la respuesta del diálogo de confirmación de eliminación de script.
 *
 * @param dialog El diálogo de alerta que generó la respuesta.
 * @param res El resultado asíncrono de la operación del diálogo.
 * @param self Puntero al objeto GappBrowser asociado.
 */
static void s_responde_remove_script(GtkAlertDialog *dialog, GAsyncResult *res, GappBrowser *self)
{
    int response = gtk_alert_dialog_choose_finish(dialog, res, NULL);

    if (response == 1) // Aceptar
    {
        // Construye la ruta base para la carpeta "Content"
        g_autofree char *pathbase = g_build_filename(g_file_get_path(gapp_browser_get_folder(self)), "Content", NULL);

        GListModel *model = gtk_multi_selection_get_model(GTK_MULTI_SELECTION(self->selection));
        guint n = g_list_model_get_n_items(G_LIST_MODEL(model));

        for (guint i = 0; i < n; i++)
        {
            // Verifica si el elemento actual está seleccionado
            if (!gtk_selection_model_is_selected(GTK_SELECTION_MODEL(self->selection), i))
                continue;

            g_autoptr(GtkTreeListRow) row = g_list_model_get_item(G_LIST_MODEL(model), i);
            GFileInfo *info = G_FILE_INFO(gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row)));
            GFile *file = gapp_browser_fn_file_info_to_file(info);

            // Verifica que no sea un directorio y que no sea la carpeta "Content"
            if (!gapp_browser_is_content_folder(self, g_file_get_path(file)))
                g_file_trash(gapp_browser_fn_file_info_to_file(info), NULL, NULL);
        }

        // Libera la memoria asignada para pathbase
        // g_free(pathbase);
    }
}

static void gapp_browser_s_toolbar_remove_clicked(GtkWidget *widget, GappBrowser *self)
{
    gapp_widget_dialog_new_confirm_action(gtk_widget_get_root(self),
                                          "Delete Selected Assets",
                                          "Are you sure you want to delete the selected assets?",
                                          s_responde_remove_script, self);
}

// -- -- --
static void gapp_browser_s_file_popup(GtkGesture *gesture, int n_press, double x, double y, GappBrowser *self)
{
    // GtkWidget *child;

    // GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    // child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);

    // if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    // {
    //     gtk_popover_popup(GTK_POPOVER(popover));
    // }
}

/**
 * Maneja la activación de un archivo en la vista del navegador.
 *
 * @param self La GtkListView que generó el evento.
 * @param position La posición del ítem activado en el modelo.
 * @param browser Puntero al GappBrowser asociado.
 */
static void gapp_browser_s_view_file_activated(GtkListView *self, guint position, GappBrowser *browser)
{
    GListModel *model = gtk_multi_selection_get_model(GTK_MULTI_SELECTION(browser->selection));

    g_autoptr(GtkTreeListRow) row = g_list_model_get_item(G_LIST_MODEL(model), position);
    GFileInfo *info = G_FILE_INFO(gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row)));
    GFile *file = gapp_browser_fn_file_info_to_file(info);

    const char *filename = g_file_info_get_name(info);
    if (gobu_fs_is_extension(filename, BROWSER_FILE_SCRIPT))
    {
        gapp_append_right_panel(gapp_get_editor_instance(), filename, gapp_script_new(g_file_get_path(file)), TRUE);
    }
    else if (gobu_fs_is_extension(filename, BROWSER_FILE_SCENE))
    {
        gapp_append_right_panel(gapp_get_editor_instance(), filename, gobu_level_editor_new(), TRUE);
    }
}

/**
 * Configura la estructura de un elemento de lista para una vista de archivos.
 *
 * @param factory La fábrica de elementos de lista (no utilizado en esta función).
 * @param list_item El elemento de lista a configurar.
 */
static void gapp_browser_s_view_file_setup_factory(GtkListItemFactory *factory, GtkListItem *list_item, gpointer data)
{
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkWidget *expander = gtk_tree_expander_new();
    gtk_list_item_set_child(list_item, expander);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(box, 6);
    gtk_widget_set_margin_end(box, 6);
    gtk_widget_set_margin_top(box, 6);
    gtk_widget_set_margin_bottom(box, 6);
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), box);

    GtkWidget *icon = gtk_image_new();
    gtk_box_append(GTK_BOX(box), icon);

    GtkWidget *label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_set_margin_start(label, 5);
    gtk_widget_set_margin_end(label, 5);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(box), label);

    GtkDropTarget *drop = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
    g_signal_connect(drop, "drop", G_CALLBACK(gapp_browser_s_file_drop), list_item);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop));

    GtkDropTarget *drop_move = gtk_drop_target_new(G_TYPE_FILE_INFO, GDK_ACTION_MOVE);
    g_signal_connect(drop_move, "drop", G_CALLBACK(gapp_browser_s_file_drop), list_item);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop_move));

    GtkDragSource *drag = gtk_drag_source_new();
    gtk_drag_source_set_actions(drag, GDK_ACTION_MOVE);
    g_signal_connect(drag, "prepare", G_CALLBACK(gapp_browser_s_file_drag), list_item);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drag));

    // GtkGesture *gesture = gtk_gesture_click_new();
    // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    // g_signal_connect(gesture, "pressed", G_CALLBACK(gapp_browser_s_file_popup), list_item);
    // gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(gesture));
}

/**
 * Vincula un elemento de lista a un archivo en una vista de árbol.
 *
 * @param factory La fábrica de elementos de lista (no utilizado en esta función).
 * @param list_item El elemento de lista a vincular.
 *
 * Esta función configura un elemento de lista para mostrar información
 * de un archivo, incluyendo su icono y nombre. Utiliza un GtkTreeExpander
 * para manejar la expansión de directorios en la vista de árbol.
 */
static void gapp_browser_s_view_file_bind_factory(GtkListItemFactory *factory, GtkListItem *list_item, GappBrowser *browser)
{
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkTreeListRow *row = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_TREE_LIST_ROW(row));

    GFileInfo *fileInfo = gtk_tree_list_row_get_item(row);
    g_return_if_fail(G_IS_FILE_INFO(fileInfo));

    GtkWidget *expander = gtk_list_item_get_child(list_item);
    g_return_if_fail(GTK_IS_TREE_EXPANDER(expander));
    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(expander), row);

    GtkWidget *box = gtk_tree_expander_get_child(GTK_TREE_EXPANDER(expander));
    g_return_if_fail(GTK_IS_BOX(box));

    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_last_child(box);
    g_return_if_fail(GTK_IS_IMAGE(icon) && GTK_IS_LABEL(label));

    // TODO: Una func para obtener el icono de los archivos...
    const char *ext_file = g_file_info_get_name(fileInfo);
    if (gobu_fs_is_extension(ext_file, ".png") || gobu_fs_is_extension(ext_file, ".jpg"))
        gtk_image_set_from_file(icon, g_file_get_path(gapp_browser_fn_file_info_to_file(fileInfo)));
    else
        gtk_image_set_from_gicon(GTK_IMAGE(icon), g_file_info_get_icon(fileInfo));

    const gchar *name = g_file_info_get_name(fileInfo);
    gtk_label_set_text(GTK_LABEL(label), name);

    // Si es la carpeta Content
    if (gapp_browser_is_content_folder(browser, g_file_get_path(gapp_browser_fn_file_info_to_file(fileInfo))))
        gtk_tree_list_row_set_expanded(row, TRUE);
}

// MARK: UI

static GtkWidget *gapp_browser_ui_menupopup_file(GappBrowser *self)
{
    GtkWidget *item;

    GtkWidget *popover = gtk_popover_new();
    gtk_popover_set_cascade_popdown(GTK_POPOVER(popover), TRUE);
    // gtk_widget_set_parent(popover, widget);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    // gtk_popover_set_pointing_to(GTK_POPOVER(popover), &(GdkRectangle){x, y, 1, 1});

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_popover_set_child(GTK_POPOVER(popover), box);

    // si no tenemos nada seleccionado o si tenemos una carpeta seleccionada.
    item = gapp_widget_button_new_icon_with_label(NULL, "New folder");
    gtk_box_append(GTK_BOX(box), item);
    g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_create_folder_clicked), self);

    item = gapp_widget_button_new_icon_with_label(NULL, "New scene");
    gtk_box_append(GTK_BOX(box), item);
    g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_create_scene_clicked), self);

    item = gapp_widget_button_new_icon_with_label(NULL, "New script");
    gtk_box_append(GTK_BOX(box), item);
    g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_create_script_clicked), self);

    // Si tenemos una carpeta o un archivo seleccionado.
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));

    item = gapp_widget_button_new_icon_with_label(NULL, "Delete...");
    gtk_box_append(GTK_BOX(box), item);

    item = gapp_widget_button_new_icon_with_label(NULL, "Rename");
    gtk_box_append(GTK_BOX(box), item);

    item = gapp_widget_button_new_icon_with_label(NULL, "Move to...");
    gtk_box_append(GTK_BOX(box), item);

    item = gapp_widget_button_new_icon_with_label(NULL, "Duplicate");
    gtk_box_append(GTK_BOX(box), item);

    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));

    item = gapp_widget_button_new_icon_with_label(NULL, "Open explorer...");
    gtk_box_append(GTK_BOX(box), item);

    return popover;
}

static GtkWidget *gapp_browser_ui_popover_menu_toolbar_add(GtkWidget *widget, gpointer data)
{
    GtkWidget *popover;

    popover = gtk_popover_menu_new_from_model(NULL);
    gtk_widget_set_parent(popover, widget);

    return popover;
}

static void gapp_browser_ui_setup(GappBrowser *self)
{
    GtkWidget *item;

    GtkWidget *toolbar = gapp_widget_toolbar_new();
    gtk_box_append(GTK_BOX(self), toolbar);
    {
        // GtkWidget *menu = gapp_browser_ui_menupopup_file(self);

        // item = gtk_menu_button_new();
        // gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(item), "view-more-symbolic");
        // gtk_menu_button_set_has_frame(GTK_MENU_BUTTON(item), FALSE);
        // gtk_menu_button_set_popover(GTK_MENU_BUTTON(item), GTK_WIDGET(menu));
        // gtk_widget_set_tooltip_text(item, "More options...");
        // gtk_box_append(GTK_BOX(toolbar), item);

        item = gapp_widget_button_new_icon_with_label("folder-new-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        gtk_widget_set_tooltip_text(item, "New folder");
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_create_folder_clicked), self);

        item = gapp_widget_button_new_icon_with_label("application-x-appliance-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        gtk_widget_set_tooltip_text(item, "New scene");
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_create_scene_clicked), self);

        item = gapp_widget_button_new_icon_with_label("x-office-spreadsheet-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        gtk_widget_set_tooltip_text(item, "New scripts");
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_create_script_clicked), self);

        item = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        gtk_widget_set_tooltip_text(item, "Delete selected item(s)");
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_s_toolbar_remove_clicked), self);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(self), scroll);
    {
        self->directory = gtk_directory_list_new("standard::*", NULL);
        gapp_browser_set_monitored_directory(self->directory);

        GtkTreeListModel *tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->directory),
                                                               FALSE,
                                                               FALSE,
                                                               gapp_browser_fn_create_list_model_directory,
                                                               NULL,
                                                               NULL);

        self->selection = gtk_multi_selection_new(G_LIST_MODEL(tree_model));

        GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(gapp_browser_s_view_file_setup_factory), self);
        g_signal_connect(factory, "bind", G_CALLBACK(gapp_browser_s_view_file_bind_factory), self);

        GtkWidget *list_view = gtk_list_view_new(self->selection, factory);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);
        g_signal_connect(list_view, "activate", G_CALLBACK(gapp_browser_s_view_file_activated), self);

        GtkDropTarget *drop = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
        g_signal_connect(drop, "drop", G_CALLBACK(gapp_browser_s_file_drop), self);
        gtk_widget_add_controller(list_view, GTK_EVENT_CONTROLLER(drop));

        // GtkGesture *gesture = gtk_gesture_click_new();
        // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
        // g_signal_connect(gesture, "pressed", G_CALLBACK(gapp_browser_s_file_popup), self);
        // gtk_widget_add_controller(list_view, GTK_EVENT_CONTROLLER(gesture));
    }
}

// MARK: API PUBLIC
/**
 * Crea una nueva instancia de GappBrowser.
 *
 * Esta función crea y devuelve un nuevo objeto GappBrowser con orientación vertical.
 * GappBrowser es presumiblemente una subclase de GtkWidget que extiende su funcionalidad
 * para las necesidades específicas de la aplicación.
 *
 * @return Un puntero a la nueva instancia de GappBrowser.
 *         El llamador es responsable de liberar este objeto con g_object_unref()
 *         cuando ya no sea necesario.
 *         Retorna NULL si la creación del objeto falla.
 */
GappBrowser *gapp_browser_new(void)
{
    return g_object_new(GAPP_TYPE_BROWSER,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        NULL);
}

/**
 * Establece la carpeta actual para el navegador de archivos.
 *
 * Esta función actualiza el directorio que está siendo mostrado por el GappBrowser.
 * Convierte la ruta proporcionada en un GFile y lo establece como el nuevo
 * directorio para el modelo de lista de directorios del navegador.
 *
 * @param browser Puntero al GappBrowser cuyo directorio se va a cambiar.
 * @param path    Ruta del nuevo directorio a mostrar.
 */
void gapp_browser_set_folder(GappBrowser *browser, const gchar *path)
{
    g_return_if_fail(GAPP_IS_BROWSER(browser));
    g_return_if_fail(path != NULL && *path != '\0');

    GFile *file = g_file_new_for_path(path);
    g_return_if_fail(G_IS_FILE(file));

    g_return_if_fail(GTK_IS_DIRECTORY_LIST(browser->directory));
    gtk_directory_list_set_file(GTK_DIRECTORY_LIST(browser->directory), file);
    gapp_browser_set_monitored_directory(GTK_DIRECTORY_LIST(browser->directory));

    g_object_unref(file);
}

/**
 * Obtiene el directorio actual del navegador GappBrowser.
 *
 * Esta función recupera el objeto GFile que representa el directorio
 * actualmente mostrado en el navegador GappBrowser especificado.
 *
 * @param browser Puntero al objeto GappBrowser del cual obtener el directorio.
 * @return GFile* Puntero al objeto GFile del directorio actual, o NULL si hay un error.
 *         El llamador no asume la propiedad del GFile retornado.
 */
GFile *gapp_browser_get_folder(GappBrowser *browser)
{
    g_return_val_if_fail(GAPP_IS_BROWSER(browser), NULL);

    GtkDirectoryList *dir_list = browser->directory;
    g_return_val_if_fail(GTK_IS_DIRECTORY_LIST(dir_list), NULL);

    return gtk_directory_list_get_file(dir_list);
}
