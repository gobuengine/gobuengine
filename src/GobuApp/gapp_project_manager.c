#include "gapp_project_manager.h"
#include "binn/binn_json.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_editor.h"
#include "gapp.h"
#include "config.h"

#include "gapp_project_config.h"

struct _GobuProjectManager
{
    GtkBox parent_instance;

    GtkWidget *entry_name;
    GtkWidget *btn_file_chooser;
    GtkWidget *dialog_btn_create;
};

static void gobu_ui_dialog_new_project(GobuProjectManager *self);
static void gobu_project_manager_ui_setup(GobuProjectManager *self);
static void gobu_project_manager_dispose(GObject *object);

// MARK: --- BASE CLASS ---

G_DEFINE_TYPE(GobuProjectManager, gobu_project_manager, GTK_TYPE_BOX)

static void gobu_project_manager_class_init(GobuProjectManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gobu_project_manager_dispose;
}

static void gobu_project_manager_dispose(GObject *object)
{
    GobuProjectManager *self = GOBU_PROJECT_MANAGER(object);

    g_clear_pointer(&self, gtk_widget_unparent);

    G_OBJECT_CLASS(gobu_project_manager_parent_class)->dispose(object);
}

static void gobu_project_manager_init(GobuProjectManager *self)
{
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));
    gobu_project_manager_ui_setup(self);
}

// --- END CLASS ---

// MARK: PRIVATE FUNC

static binn *gobu_fn_if_project_exist_register(binn *list)
{
    g_return_val_if_fail(list != NULL, NULL);

    binn *list_new = binn_list();
    if (!list_new)
    {
        g_critical("Failed to create new binn list");
        return NULL;
    }

    binn value;
    binn_iter iter;
    binn_list_foreach(list, value)
    {
        const char *item_path = binn_object_str(&value, "path");
        if (item_path != NULL)
        {
            g_autofree gchar *project_file = pathJoin(item_path, GAPP_PROJECT_MANAGER_FILE, NULL);
            if (project_file != NULL && fsExist(project_file))
            {
                if (binn_list_add_object(list_new, &value) == FALSE)
                {
                    g_warning("Failed to add item to new list");
                }
            }
        }
        else
        {
            g_warning("Found list item with NULL path");
        }
    }

    gchar *config_file = pathJoin(g_get_user_config_dir(), "GOBU", GAPP_FILE_PROJECTS_NAME, NULL);
    binn_save_to_file(list_new, config_file);

    return list_new;
}

static binn *gobu_fn_register_load_projects(void)
{
    binn *list = NULL;
    gchar *config_file = pathJoin(g_get_user_config_dir(), "GOBU", GAPP_FILE_PROJECTS_NAME, NULL);

    if (fsExist(config_file))
    {
        list = binn_load_from_file(config_file);
        if (list == NULL)
        {
            g_warning("No se pudo cargar el archivo de proyectos: %s", config_file);
        }
    }
    else
    {
        list = binn_list();
        if (list == NULL)
        {
            g_warning("No se pudo crear una nueva lista binn");
        }
    }

    g_free(config_file);

    return gobu_fn_if_project_exist_register(list);
}

static gboolean gobu_fn_register_project(const gchar *path)
{
    g_return_val_if_fail(path != NULL && *path != '\0', FALSE);

    binn *list = binn_list();
    gboolean success = FALSE;
    gchar *config_dir = pathJoin(g_get_user_config_dir(), "GOBU", NULL);
    gchar *config_file = pathJoin(config_dir, GAPP_FILE_PROJECTS_NAME, NULL);

    // Asegurar que el directorio de configuración existe
    if (!pathCreateNew(config_dir))
    {
        g_warning("No se pudo crear el directorio de configuración: %s", config_dir);
        goto cleanup;
    }

    // Cargar proyectos existentes si el archivo existe
    if (fsExist(config_file))
    {
        list = binn_load_from_file(config_file);
        if (list == NULL)
        {
            g_warning("No se pudo cargar el archivo de proyectos: %s", config_file);
            goto cleanup;
        }
    }

    // Añadir el nuevo proyecto
    binn *obj = binn_object();
    binn_object_set_str(obj, "path", g_strdup(path));
    binn_list_add_object(list, obj);
    binn_free(obj);

    // Guardar la lista actualizada
    if (!binn_save_to_file(list, config_file))
    {
        g_warning("No se pudo guardar el archivo de proyectos: %s", config_file);
        goto cleanup;
    }

    success = TRUE;

cleanup:
    binn_free(list);
    g_free(config_dir);
    g_free(config_file);
    return success;
}

static gboolean gobu_fn_create_project(const gchar *name, const gchar *path)
{
    g_return_val_if_fail(name != NULL && *name != '\0', FALSE);
    g_return_val_if_fail(path != NULL && *path != '\0', FALSE);

    gboolean is_created = FALSE;
    g_autoptr(GError) error = NULL;

    g_autofree gchar *project_dir = pathJoin(path, name, NULL);

    if (!fsExist(project_dir))
    {
        g_autofree gchar *content_dir = pathJoin(project_dir, "Game", "Content", NULL);
        g_autofree gchar *project_file = pathJoin(project_dir, GAPP_PROJECT_MANAGER_FILE, NULL);

        // Crear directorios
        if (pathCreateNew(project_dir) &&
            pathCreateNew(content_dir))
        {
            // Crear archivo de configuración
            is_created = gapp_project_config_create_file_default(gapp_get_config_instance(), project_file, name);
        }
        else
        {
            g_warning("No se pudieron crear los directorios del proyecto");
        }
    }
    else
    {
        g_warning("El directorio del proyecto ya existe: %s", project_dir);
    }

    if (is_created)
    {
        // registramos el proyecto en el archivo de configuración del usuario
        gobu_fn_register_project(project_dir);
    }

    return is_created;
}

static gboolean gobu_fn_open_editor_main(const gchar *path, GobuProjectManager *self)
{
    g_return_val_if_fail(path != NULL, FALSE);
    g_return_val_if_fail(GOBU_IS_PROJECT_MANAGER(self), FALSE);

    if (!gapp_project_config_load(gapp_get_config_instance(), path))
    {
        g_warning("Failed to initialize project configuration for path: %s", path);
        return FALSE;
    }

    gapp_open_project(gapp_get_editor_instance(), path);

    g_debug("Editor main opened successfully for project: %s", path);
    return TRUE;
}

static gboolean gobu_fn_validate_project_name(const char *project_name)
{
    if (project_name == NULL || *project_name == '\0')
    {
        return FALSE;
    }

    const char *valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    return strspn(project_name, valid_chars) == strlen(project_name);
}

static void gobu_fn_response_file_chooser_folder(GtkFileDialog *source, GAsyncResult *res, GobuProjectManager *self)
{
    g_return_if_fail(GTK_IS_FILE_DIALOG(source));
    g_return_if_fail(G_IS_ASYNC_RESULT(res));
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    GError *error = NULL;
    GFile *file = gtk_file_dialog_select_folder_finish(source, res, &error);

    if (error == NULL && file != NULL)
    {
        char *path = g_file_get_path(file);
        if (path != NULL)
        {
            gtk_button_set_label(GTK_BUTTON(self->btn_file_chooser), path);
            g_free(path);
        }
        g_object_unref(file);
    }
    else if (error != NULL)
    {
        g_warning("Error al seleccionar la carpeta: %s", error->message);
        g_error_free(error);
    }
}

static void gobu_fn_open_dialog_response(GObject *source, GAsyncResult *result, GobuProjectManager *self)
{
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *file = gtk_file_dialog_open_finish(dialog, result, NULL);
    if (file)
    {
        g_autofree gchar *filename = g_file_get_path(file);
        if (gobu_fn_open_editor_main(filename, self))
        {
            gobu_fn_register_project(pathDirname(filename));
        }

        // g_free(filename);
        g_object_unref(file);
    }
}

static GtkStringList *gobu_fn_grid_view_list_project_model(void)
{
    GtkStringList *sl = gtk_string_list_new(NULL);
    binn *list = gobu_fn_register_load_projects();

    if (list != NULL)
    {
        binn value;
        binn_iter iter;
        binn_list_foreach(list, value)
        {
            const char *item_path = binn_object_str(&value, "path");
            if (item_path != NULL)
            {
                g_autofree gchar *content_path = pathJoin(item_path, "Content", NULL);
                // g_autofree gchar *project_file = pathJoin(item_path, GAPP_PROJECT_MANAGER_FILE, NULL);

                gtk_string_list_append(sl, content_path);
                // g_free(content_path);
            }
        }
        binn_free(list);
    }

    return sl;
}

// MARK:SIGNAL

static void gobu_s_item_factory_setup_item(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    g_return_if_fail(GTK_IS_LIST_ITEM_FACTORY(factory));
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkWidget *box, *imagen, *label;

    // Crear y configurar la caja contenedora
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_list_item_set_child(list_item, box);
    gtk_widget_set_size_request(box, 120, 120);

    // Crear y configurar la imagen
    imagen = gtk_image_new();
    gtk_image_set_icon_size(GTK_IMAGE(imagen), GTK_ICON_SIZE_LARGE);
    gtk_widget_set_vexpand(imagen, TRUE);
    gtk_box_append(GTK_BOX(box), imagen);

    // Crear y configurar la etiqueta
    label = gtk_label_new(NULL);
    gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_CHAR);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(box), label);
}

static void gobu_s_item_factory_bind_item(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkWidget *box, *image, *label;

    // Obtener los widgets del ítem
    box = gtk_list_item_get_child(list_item);
    g_return_if_fail(GTK_IS_BOX(box));

    image = gtk_widget_get_first_child(box);
    g_return_if_fail(GTK_IS_IMAGE(image));

    label = gtk_widget_get_next_sibling(image);
    g_return_if_fail(GTK_IS_LABEL(label));

    // Obtener la ruta del proyecto
    GtkStringObject *obj = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_STRING_OBJECT(obj));
    const char *path_project = g_path_get_dirname(gtk_string_object_get_string(obj));

    // Configurar la imagen
    gchar *thumbnail_path = pathJoin(path_project, GAPP_PROJECT_MANAGER_PREVIEW, NULL);
    if (fsExist(thumbnail_path))
    {
        gtk_image_set_from_file(GTK_IMAGE(image), thumbnail_path);
    }
    else
    {
        gtk_image_set_from_icon_name(GTK_IMAGE(image), GAPP_PROJECT_MANAGER_ICON);
    }
    g_free(thumbnail_path);

    // Configurar la etiqueta y el tooltip
    gtk_label_set_text(GTK_LABEL(label), pathBasename(path_project));
    gtk_widget_set_tooltip_text(box, path_project);
}

static void gobu_s_btn_file_chooser_clicked(GtkWidget *button, GobuProjectManager *self)
{
    g_return_if_fail(GTK_IS_WIDGET(button));
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_select_folder(GTK_FILE_DIALOG(dialog),
                                  GTK_WINDOW(self),
                                  NULL,
                                  gobu_fn_response_file_chooser_folder,
                                  self);
}

static void gobu_s_open_dialog_new_project_clicked(GtkWidget *button, GobuProjectManager *self)
{
    g_return_if_fail(GTK_IS_WIDGET(button));
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    g_debug("Abriendo diálogo para crear nuevo proyecto");
    gobu_ui_dialog_new_project(self);
}

static void gobu_s_create_project_clicked(GtkWidget *button, GobuProjectManager *self)
{
    g_return_if_fail(GTK_IS_WIDGET(button));
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    const gchar *name = gtk_editable_get_text(GTK_EDITABLE(self->entry_name));
    const gchar *path = gtk_button_get_label(GTK_BUTTON(self->btn_file_chooser));

    g_debug("Intentando crear proyecto '%s' en '%s'", name, path);

    gboolean is_created = gobu_fn_create_project(name, path);

    if (is_created)
    {
        g_debug("Proyecto creado exitosamente");
        gobu_fn_open_editor_main(pathJoin(path, name, GAPP_PROJECT_MANAGER_FILE, NULL), self);
    }
    else
    {
        g_warning("Falló la creación del proyecto");
        // TODO: Mostrar un mensaje de error al usuario
    }

    gtk_window_close(GTK_WINDOW(gtk_widget_get_ancestor(button, GTK_TYPE_WINDOW)));
}

static void gobu_s_list_project_activated(GtkGridView *grid_view, guint position, GobuProjectManager *self)
{
    GtkSelectionModel *select_model = gtk_grid_view_get_model(grid_view);
    g_return_if_fail(GTK_IS_SELECTION_MODEL(select_model));

    GtkStringObject *obj = g_list_model_get_item(select_model, position);

    g_return_if_fail(GTK_IS_STRING_OBJECT(obj));
    const char *path_project = g_path_get_dirname(gtk_string_object_get_string(obj));

    gobu_fn_open_editor_main(pathJoin(path_project, GAPP_PROJECT_MANAGER_FILE, NULL), self);
}

static void gobu_s_open_other_project_clicked(GtkWidget *button, GobuProjectManager *self)
{
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    g_debug("Abriendo diálogo para seleccionar proyecto: %p", (void *)self);

    GtkFileDialog *dialog = gtk_file_dialog_new();
    GtkRoot *root = gtk_widget_get_root(button);

    gtk_file_dialog_open(dialog,
                         GTK_WINDOW(root),
                         NULL,
                         gobu_fn_open_dialog_response,
                         self);

    g_debug("Diálogo de selección de proyecto abierto");
}

static void gobu_s_entry_name_changed(GtkWidget *entry, GobuProjectManager *self)
{
    g_return_if_fail(GTK_IS_ENTRY(entry));
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    const char *name = gtk_editable_get_text(GTK_EDITABLE(self->entry_name));
    const char *path = gtk_button_get_label(GTK_BUTTON(self->btn_file_chooser));
    g_autofree char *project_dir = pathJoin(path, name, NULL);

    gboolean name_is_valid = gobu_fn_validate_project_name(name);
    gboolean dir_not_exists = !fsExist(project_dir);
    gboolean is_valid = name_is_valid && dir_not_exists;

    gtk_widget_set_sensitive(self->dialog_btn_create, is_valid);

    // Proporcionar retroalimentación visual al usuario
    if (is_valid)
    {
        gtk_widget_remove_css_class(entry, "error");
        gtk_widget_add_css_class(entry, "success");
    }
    else
    {
        gtk_widget_remove_css_class(entry, "success");
        gtk_widget_add_css_class(entry, "error");

        // Depuración: identificar la razón específica del error
        if (!name_is_valid)
        {
            g_debug("Nombre de proyecto no válido: %s", name);
        }
        else if (!dir_not_exists)
        {
            g_debug("Ya existe un proyecto con el nombre: %s", name);
        }
    }

    g_debug("Nombre de proyecto: %s, Válido: %s", name, is_valid ? "Sí" : "No");
}

// MARK: UI

static void gobu_ui_dialog_new_project(GobuProjectManager *self)
{
    GtkWidget *win, *vbox, *hbox, *label, *btn_create;

    g_autofree gchar *pathProjects = pathJoin(g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS), "Gobu Projects", NULL);

    GtkWidget *parent = gtk_widget_get_root(self);

    // Crear la ventana del diálogo
    win = gtk_window_new();
    gtk_window_set_modal(GTK_WINDOW(win), TRUE);
    gtk_window_set_title(GTK_WINDOW(win), "Create new project");
    gtk_window_set_default_size(GTK_WINDOW(win), 500, 200);
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(parent));

    // Crear el contenedor principal
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_start(vbox, 15);
    gtk_widget_set_margin_end(vbox, 15);
    gtk_widget_set_margin_top(vbox, 15);
    gtk_widget_set_margin_bottom(vbox, 15);
    gtk_window_set_child(GTK_WINDOW(win), GTK_WIDGET(vbox));

    // Añadir campo para el nombre del proyecto
    label = gtk_label_new("Project Name:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(label));

    self->entry_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(self->entry_name), "New Project");
    gtk_box_append(GTK_BOX(vbox), self->entry_name);
    g_signal_connect(self->entry_name, "changed", G_CALLBACK(gobu_s_entry_name_changed), self);

    // Añadir campo para la ruta del proyecto
    label = gtk_label_new("Project Path:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(label));

    self->btn_file_chooser = gtk_button_new_with_label(pathProjects);
    gtk_box_append(GTK_BOX(vbox), self->btn_file_chooser);
    g_signal_connect(self->btn_file_chooser, "clicked", G_CALLBACK(gobu_s_btn_file_chooser_clicked), self);

    // Añadir botón de creación
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(hbox, 15);
    gtk_widget_set_margin_bottom(hbox, 15);
    gtk_box_append(GTK_BOX(vbox), hbox);

    self->dialog_btn_create = gtk_button_new_with_label("Create");
    gtk_widget_set_sensitive(GTK_WIDGET(self->dialog_btn_create), FALSE);
    gtk_box_append(GTK_BOX(hbox), self->dialog_btn_create);
    gtk_widget_add_css_class(self->dialog_btn_create, "suggested-action");
    g_signal_connect(self->dialog_btn_create, "clicked", G_CALLBACK(gobu_s_create_project_clicked), self);

    gtk_window_present(GTK_WINDOW(win));
}

static void gobu_project_manager_ui_setup(GobuProjectManager *self)
{
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(hbox, 5);
    gtk_widget_set_margin_bottom(hbox, 5);
    gtk_widget_set_margin_end(hbox, 5);
    gtk_widget_set_halign(hbox, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(self), hbox);
    {
        GtkWidget *btn_new = gapp_widget_button_new_icon_with_label("window-new-symbolic", "New");
        gtk_button_set_has_frame(GTK_BUTTON(btn_new), TRUE);
        gtk_box_append(GTK_BOX(hbox), btn_new);
        g_signal_connect(btn_new, "clicked", G_CALLBACK(gobu_s_open_dialog_new_project_clicked), self);

        GtkWidget *btn_open = gapp_widget_button_new_icon_with_label("document-open-symbolic", "Open other");
        gtk_button_set_has_frame(GTK_BUTTON(btn_open), TRUE);
        gtk_box_append(GTK_BOX(hbox), btn_open);
        g_signal_connect(btn_open, "clicked", G_CALLBACK(gobu_s_open_other_project_clicked), self);
    }

    GtkWidget *search = gtk_search_entry_new();
    gtk_widget_set_margin_start(search, 5);
    gtk_widget_set_margin_end(search, 5);
    gtk_widget_set_margin_top(search, 5);
    gtk_widget_set_margin_bottom(search, 5);
    gtk_box_append(GTK_BOX(self), search);
    { // GRID LIST ITEM PROJECTS
        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_widget_set_margin_start(scroll, 5);
        gtk_widget_set_margin_end(scroll, 5);
        gtk_widget_set_margin_top(scroll, 5);
        gtk_widget_set_margin_bottom(scroll, 5);
        gtk_box_append(GTK_BOX(self), scroll);
        {
            GtkStringList *sl = gobu_fn_grid_view_list_project_model();

            GtkFilter *filter = GTK_FILTER(gtk_string_filter_new(gtk_property_expression_new(GTK_TYPE_STRING_OBJECT, NULL, "string")));
            GtkFilterListModel *filter_model = gtk_filter_list_model_new(G_LIST_MODEL(sl), filter);
            gtk_filter_list_model_set_incremental(filter_model, TRUE);
            g_object_bind_property(search, "text", filter, "search", 0);

            GtkSingleSelection *selection = gtk_single_selection_new(G_LIST_MODEL(filter_model));

            GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(gobu_s_item_factory_setup_item), NULL);
            g_signal_connect(factory, "bind", G_CALLBACK(gobu_s_item_factory_bind_item), NULL);

            GtkWidget *grid = gtk_grid_view_new(GTK_SELECTION_MODEL(selection), factory);
            gtk_widget_set_vexpand(grid, TRUE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
            g_signal_connect(grid, "activate", G_CALLBACK(gobu_s_list_project_activated), self);
        }
    }
}

// --- MARK:BEGIN API ---

GobuProjectManager *gobu_project_manager_new(void)
{
    return g_object_new(GOBU_TYPE_PROJECT_MANAGER, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}

void gobu_project_manager_show(GobuProjectManager *self)
{
    g_return_if_fail(GOBU_IS_PROJECT_MANAGER(self));
    // gtk_window_present(GTK_WINDOW(self));
}

// --- END API ---
