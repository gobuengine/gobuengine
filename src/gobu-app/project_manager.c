#include "project_manager.h"
#include "gobu_project_editor.h"
#include "config.h"
#include "gobu/gobu-gobu.h"

typedef struct _GobuProjectManager
{
    GtkWidget *btn_create;
    GtkWidget *btn_file_chooser;
    GtkWidget *entry_name;
    GtkWidget *window;
} GobuProjectManager;

static GobuProjectManager project = {0};

/**
 * Función para crear un proyecto en Gobu.
 *
 * @param name  El nombre del proyecto a crear.
 * @param path  La ruta donde se creará el proyecto.
 *
 * @return TRUE si el proyecto se creó con éxito, FALSE en caso contrario.
 */
static gboolean project_manager_fn_create_project(const gchar *name, const gchar *path)
{
    gboolean is_created = FALSE;

    gchar *project = g_build_filename(path, name, NULL);

    /*NOTE  aqui solo es necesario saber si existe un folder con este nombre */
    if (!g_file_test(project, G_FILE_TEST_IS_DIR))
    {
        gchar *config_file = g_strdup_printf("{\"setting\":{\"name\":\"%s\", \"width\":1280, \"height\":720}}", name);

        g_mkdir(project, 0);
        g_mkdir(g_build_filename(project, "Content", NULL), 0); // Resource
        g_mkdir(g_build_filename(project, "Saved", NULL), 0);   // Saved
        g_file_set_contents(g_build_filename(project, "game.gobuproject", NULL), config_file, strlen(config_file), NULL);

        g_free(config_file);

        is_created = TRUE;
    }

    g_free(project);

    g_info("INFO: Proyecto Creado [%d]\n", is_created);
    return is_created;
}

/**
 * Maneja la señal de finalización de la apertura de un proyecto en Gobu.
 *
 * @param source  El objeto fuente que emitió la señal de finalización.
 * @param res     El resultado de la operación de apertura de proyecto.
 * @param ctx     El contexto del administrador de proyectos relacionado con la operación.
 */
static void project_manager_signal_file_dialog_open_project_ready(GObject *source, GAsyncResult *res, GobuProjectManager *ctx)
{
    GFile *file;
    GError *error = NULL;

    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, &error);
    g_return_if_fail(file);
    if (error != NULL)
        g_error_free(error);

    char *path_str = g_file_get_path(file);
    if (gobu_project_load(path_str))
    {
        gobu_project_editor_init(gtk_window_get_application(ctx->window));
    }

    g_free(path_str);
    gtk_window_destroy(GTK_WINDOW(ctx->window));
}

/**
 * Maneja la señal de apertura de un diálogo de selección de proyecto en Gobu.
 *
 * @param widget  El widget que desencadenó la señal de apertura de proyecto.
 * @param ctx     El contexto del administrador de proyectos relacionado con la operación.
 */
static void project_manager_signal_file_dialog_open_project(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkFileDialog *file_dialog;
    file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(GTK_FILE_DIALOG(file_dialog), GTK_WINDOW(ctx->window), NULL, project_manager_signal_file_dialog_open_project_ready, ctx);
}

/**
 * Maneja la señal de creación de proyecto en Gobu.
 *
 * @param widget  El widget que desencadenó la señal de creación de proyecto.
 * @param ctx     El contexto del administrador de proyectos relacionado con la creación del proyecto.
 */
static void project_manager_signal_create_project(GtkWidget *widget, GobuProjectManager *ctx)
{
    const gchar *name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
    const gchar *path = gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser));

    if (project_manager_fn_create_project(name, path))
    {
        gchar *path_str = g_build_filename(path, name, NULL);
        if (gobu_project_load(path_str))
        {
            gobu_project_editor_init(gtk_window_get_application(ctx->window));
        }
        gtk_window_destroy(GTK_WINDOW(ctx->window));
        g_free(path_str);
    }
}

/**
 * Maneja la señal de cambio en un campo de entrada para verificar el nombre del proyecto en Gobu.
 *
 * @param entry  El campo de entrada que desencadenó la señal de cambio.
 * @param ctx    El contexto del administrador de proyectos relacionado con la verificación del nombre del proyecto.
 */
static void project_manager_signal_change_entry_check_name_project(GtkWidget *entry, GobuProjectManager *ctx)
{
    if (gtk_entry_get_text_length(entry) > 0)
    {
        const gchar *name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
        gchar *path_full = g_build_filename(gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser)), name, NULL);

        gboolean test = g_file_test(g_build_filename(path_full, FILE_GAME_PROJECT, NULL), G_FILE_TEST_EXISTS);
        if (test == FALSE)
        {
            gtk_widget_add_css_class(ctx->btn_create, "suggested-action");
            gtk_widget_set_sensitive(ctx->btn_create, TRUE);
            g_free(path_full);
            return;
        }

        g_free(path_full);
    }

    gtk_widget_remove_css_class(ctx->btn_create, "suggested-action");
    gtk_widget_set_sensitive(ctx->btn_create, FALSE);
}

/**
 * Maneja la señal de respuesta del diálogo de selección de carpetas en Gobu.
 *
 * @param dialog       El diálogo de selección de carpetas que generó la señal.
 * @param response_id  El identificador de respuesta de la señal.
 * @param widget       El widget relacionado con la señal.
 */
static void project_manager_signal_response_file_chooser_folder(GtkDialog *dialog, gint response_id, GtkWidget *widget)
{
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        GFile *files = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        gtk_button_set_label(GTK_BUTTON(widget), g_file_get_path(files));
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * Abre un diálogo de selección de carpetas en el administrador de proyectos de Gobu.
 *
 * @param widget  El widget que desencadenó la apertura del diálogo de selección de carpetas.
 * @param ctx     El contexto del administrador de proyectos relacionado con la operación.
 */
static void project_manager_signal_open_dialog_file_chooser_folder(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkWidget *self = gtk_file_chooser_dialog_new("Select folder",
                                                  GTK_WINDOW(ctx->window),
                                                  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                  "Cancel", GTK_RESPONSE_CANCEL,
                                                  "Select", GTK_RESPONSE_ACCEPT, NULL);

    g_signal_connect(self, "response", G_CALLBACK(project_manager_signal_response_file_chooser_folder), widget);

    gtk_widget_show(self);
    gtk_window_set_modal(GTK_WINDOW(self), TRUE);
}

/**
 * Crea una nueva ventana para el administrador de proyectos en Gobu.
 *
 * @param app   La aplicación GtkApplication en la que se crea la ventana.
 * @param ctx   El contexto del administrador de proyectos relacionado con la ventana.
 */
void gobu_project_manager_window_new(GtkApplication *app)
{
    GtkWidget *vbox, *hbox, *btn_other_chooser, *label;

    project.window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(project.window), "GobuEngine - ProjectManager");
    gtk_window_set_default_size(GTK_WINDOW(project.window), 500, 200);
    gtk_window_set_resizable(project.window, FALSE);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(project.window), vbox);
    gobu_widget_set_margin(vbox, 15);

    label = gtk_label_new("Project Name:");
    gtk_label_set_xalign(label, 0.0);
    gtk_box_append(GTK_BOX(vbox), label);

    project.entry_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(project.entry_name), "New Project");
    gtk_box_append(GTK_BOX(vbox), project.entry_name);
    g_signal_connect(project.entry_name, "changed", G_CALLBACK(project_manager_signal_change_entry_check_name_project), &project);

    label = gtk_label_new("Project Path:");
    gtk_label_set_xalign(label, 0.0);
    gtk_box_append(GTK_BOX(vbox), label);

    project.btn_file_chooser = gtk_button_new_with_label(g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS));
    gtk_box_append(GTK_BOX(vbox), project.btn_file_chooser);
    g_signal_connect(project.btn_file_chooser, "clicked", G_CALLBACK(project_manager_signal_open_dialog_file_chooser_folder), &project);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_end(hbox, 15);
    gtk_widget_set_margin_top(hbox, 15);
    gtk_widget_set_margin_bottom(hbox, 15);
    gtk_box_append(GTK_BOX(vbox), hbox);

    project.btn_create = gtk_button_new_with_label("Create");
    gtk_widget_set_sensitive(GTK_BUTTON(project.btn_create), FALSE);
    gtk_box_append(GTK_BOX(hbox), project.btn_create);
    g_signal_connect(project.btn_create, "clicked", G_CALLBACK(project_manager_signal_create_project), &project);

    btn_other_chooser = gtk_button_new_with_label("Open other");
    gtk_box_append(GTK_BOX(hbox), btn_other_chooser);
    g_signal_connect(btn_other_chooser, "clicked", G_CALLBACK(project_manager_signal_file_dialog_open_project), &project);

    gtk_window_present(GTK_WINDOW(project.window));
}
