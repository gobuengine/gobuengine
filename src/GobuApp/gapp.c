#include "gapp.h"
#include "binn/binn_json.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_project_manager.h"
#include "gapp_browser.h"
#include "gapp_level_editor.h"
#include "gapp_project_setting.h"
#include "gapp_project_config.h"

static GappMain *gappMain;

// -----------------
// MARK:BASE CLASS
// -----------------

struct _GappMain
{
    GtkApplicationWindow parent_instance;
    GtkWidget *dnotebook, *vnotebook;
    GtkWidget *btn_s;
    GtkWidget *btn_p;
    GtkWidget *btn_b;
    GtkWidget *btn_set;
    GtkWidget *project_manager;
    GtkWidget *browser;
    GtkWidget *config;
    GtkWidget *window;
    GtkWidget *title_window;
    // ecs unique world to proyect: test fase 1
    ecs_world_t *world;
};

G_DEFINE_TYPE(GappMain, gapp_main, GTK_TYPE_APPLICATION)

static void gapp_main_activate(GappMain *app);
static void gapp_main_open(GappMain *app, GFile **files, gint n_files, const gchar *hint);
static void gapp_set_headerbar_button_sensitives(GappMain *self, gboolean sensitive);
static void gapp_signal_project_settings_open(GtkWidget *widget, GappMain *self);
static bool gapp_config_init(void);

static void gapp_main_class_init(GappMainClass *klass)
{
    G_APPLICATION_CLASS(klass)->activate = gapp_main_activate;
}

static void gapp_main_init(GappMain *self)
{
    self->config = gapp_project_config_new();
    self->world = pixio_world_init();
}

// -----------------
// MARK: UI
// -----------------

static void gapp_main_activate(GappMain *app)
{
    gapp_config_init();

    app->title_window = gtk_label_new(GOBU_VERSION_STR);

    app->window = gtk_window_new();
    gtk_window_set_application(GTK_WINDOW(app->window), GTK_APPLICATION(app));
    gtk_window_set_default_size(GTK_WINDOW(app->window), 1280, 800);
    {
        GtkWidget *header_bar = gtk_header_bar_new();
        gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), app->title_window);
        gtk_window_set_titlebar(GTK_WINDOW(app->window), header_bar);

        { // #toolbar headerbar
            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_header_bar_set_title_widget(header_bar, hbox);
            app->btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save all");
            gtk_box_append(hbox, app->btn_s);

            app->btn_p = gapp_widget_button_new_icon_with_label("applications-games-symbolic", "Preview");
            gtk_box_append(hbox, app->btn_p);

            app->btn_b = gapp_widget_button_new_icon_with_label("drive-optical-symbolic", "Build");
            gtk_box_append(hbox, app->btn_b);
        }

        { // #setting project, game
            app->btn_set = gapp_widget_button_new_icon_with_label("preferences-other-symbolic", "Project Settings");
            gtk_header_bar_pack_end(header_bar, app->btn_set);
            g_signal_connect(app->btn_set, "clicked", G_CALLBACK(gapp_signal_project_settings_open), app);
        }
    }

    GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(hpaned), 250);
    // gtk_paned_set_shrink_start_child(GTK_PANED(hpaned), FALSE);
    // gtk_paned_set_wide_handle(GTK_PANED(hpaned), TRUE);
    gtk_paned_set_resize_start_child(GTK_PANED(hpaned), FALSE);
    gtk_window_set_child(GTK_WINDOW(app->window), hpaned);
    {
        app->vnotebook = gtk_notebook_new();
        gtk_notebook_set_tab_pos(GTK_NOTEBOOK(app->vnotebook), GTK_POS_LEFT);
        gtk_widget_set_visible(GTK_NOTEBOOK(app->vnotebook), FALSE);
        gtk_paned_set_start_child(GTK_PANED(hpaned), app->vnotebook);
        {
            app->browser = gapp_browser_new();
            gapp_append_left_panel(app, GAPP_TOOLBAR_LEFT_ICON_BROWSER, app->browser);
            gapp_append_left_panel(app, GAPP_TOOLBAR_LEFT_ICON_ROOMS, gtk_label_new("Rooms"));
            gapp_append_left_panel(app, GAPP_TOOLBAR_LEFT_ICON_ACTOR, gtk_label_new("Actors"));
            gapp_append_left_panel(app, GAPP_TOOLBAR_LEFT_ICON_TILE, gtk_label_new("TileEditor"));
        }

        app->dnotebook = gtk_notebook_new();
        gtk_paned_set_end_child(GTK_PANED(hpaned), app->dnotebook);
        {
            app->project_manager = gobu_project_manager_new();
            gapp_append_right_panel(app, "Projects", app->project_manager, FALSE);
        }
    }

    gapp_set_headerbar_button_sensitives(app, FALSE);
    gtk_window_present(GTK_WINDOW(app->window));
}

// -----------------
// MARK:SIGNAL
// -----------------

/**
 * Manejador de señal para abrir la configuración del proyecto.
 *
 * Esta función se activa cuando se solicita abrir la configuración del proyecto.
 * Crea una nueva ventana de configuración y la muestra.
 *
 * @param widget El widget que emitió la señal (no utilizado en esta función).
 * @param self Puntero a la estructura principal de la aplicación.
 */
static void gapp_signal_project_settings_open(GtkWidget *widget, GappMain *self)
{
    g_return_if_fail(self != NULL);

    GtkWidget *setting = gobu_project_setting_new();
    if (setting == NULL)
    {
        g_warning("Failed to create project settings window");
        return;
    }

    gobu_project_setting_show(setting, self->window);
}

// -----------------
// MARK: API
// -----------------

/**
 * Establece la sensibilidad de los botones de la barra de encabezado.
 *
 * Esta función cambia el estado de sensibilidad (activado/desactivado) de
 * varios botones en la barra de encabezado de la aplicación.
 *
 * @param self Puntero a la estructura principal de la aplicación.
 * @param sensitive Valor booleano que indica si los botones deben estar
 *                  sensibles (TRUE) o insensibles (FALSE).
 */
static void gapp_set_headerbar_button_sensitives(GappMain *self, gboolean sensitive)
{
    gtk_widget_set_visible(self->btn_s, sensitive);
    gtk_widget_set_visible(self->btn_p, sensitive);
    gtk_widget_set_visible(self->btn_b, sensitive);
    gtk_widget_set_visible(self->btn_set, sensitive);
}

/**
 * Crea una nueva instancia de GappMain.
 *
 * Esta función crea y devuelve un nuevo objeto GappMain, que es la clase
 * principal de la aplicación. Configura el ID de la aplicación y las banderas.
 *
 * @return Un puntero a la nueva instancia de GappMain, o NULL si la creación falla.
 */
static GappMain *gapp_new(void)
{
    return g_object_new(GAPP_TYPE_MAIN, "application-id", APPLICATION_ID, "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}

/**
 * Carga un archivo de configuración en un objeto GKeyFile.
 *
 * @param filename El nombre del archivo de configuración a cargar.
 * @return Un puntero al objeto GKeyFile cargado, o NULL si ocurrió un error.
 */
static GKeyFile *gapp_config_load_from_file(const char *filename)
{
    GKeyFile *keyfile = g_key_file_new();
    GError *error = NULL;

    gchar *fileconfig = g_build_filename(g_get_current_dir(), "Config", filename, NULL);

    if (!g_key_file_load_from_file(keyfile, fileconfig, G_KEY_FILE_NONE, &error))
    {
        g_error("Error loading key file: %s\n", error->message);
        g_error_free(error);
        g_key_file_free(keyfile);
        return NULL;
    }

    g_free(fileconfig);

    return keyfile;
}

/**
 * Inicializa la configuración de la aplicación.
 *
 * Esta función carga el archivo de configuración, establece el tema
 * (oscuro o claro) y aplica los estilos CSS.
 *
 * @return TRUE si la inicialización fue exitosa, FALSE en caso contrario.
 */
static bool gapp_config_init(void)
{
    GKeyFile *keyfile = gapp_config_load_from_file("editor.config.ini");
    if (keyfile == NULL)
        return FALSE;

    // load config file properties
    bool darkmode = g_key_file_get_boolean(keyfile, "Editor", "darkMode", NULL);
    // g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", darkmode, NULL);

    AdwStyleManager *style_manager = adw_style_manager_get_default();
    adw_style_manager_set_color_scheme(style_manager, darkmode ? ADW_COLOR_SCHEME_PREFER_DARK : ADW_COLOR_SCHEME_FORCE_LIGHT);

    // css provider
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "Content/theme/default.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_key_file_free(keyfile);

    return TRUE;
}

/**
 * Obtiene la instancia del editor principal.
 *
 * Esta función devuelve un puntero a la instancia global del editor principal
 * de la aplicación. La instancia se devuelve como un GObject genérico.
 *
 * @return Un puntero GObject a la instancia del editor principal, o NULL si no está inicializada.
 */
GObject *gapp_get_editor_instance(void)
{
    return gappMain;
}

/**
 * Obtiene la instancia de configuración de la aplicación.
 *
 * Esta función devuelve un puntero a la instancia de configuración
 * asociada con la instancia principal de la aplicación (gappMain).
 * La instancia se devuelve como un GObject genérico.
 *
 * @return Un puntero GObject a la instancia de configuración, o NULL si no está disponible.
 */
GObject *gapp_get_config_instance(void)
{
    return gappMain->config;
}

ecs_world_t *gapp_get_world_instance(void)
{
    return gappMain->world;
}

/**
 * Inicializa un proyecto en el editor.
 *
 * @param self Puntero al objeto GappMain.
 * @param path Ruta del proyecto a inicializar.
 */
void gapp_open_project(GappMain *self, const gchar *path)
{
    g_return_if_fail(GAPP_IS_MAIN(self));
    g_return_if_fail(path != NULL && *path != '\0');

    // Crear una escena inicial
    gapp_append_right_panel(self, "Viewport", gobu_level_editor_new(), FALSE);

    // Cerrar la página de proyectos
    gtk_notebook_remove_page(GTK_NOTEBOOK(self->dnotebook), 0);

    // Mostrar el notebook del panel izquierdo
    gtk_widget_set_visible(GTK_WIDGET(self->vnotebook), TRUE);

    // Habilitar los botones del título-headerbar
    gapp_set_headerbar_button_sensitives(self, TRUE);

    // Cambiar el path del navegador
    g_autofree gchar *data_path = gobu_path_join(gobu_path_dirname(path), "Data", NULL);
    gapp_browser_set_folder(self->browser, data_path);

    // actualizamos el titulo
    g_autofree gchar *title = g_strdup_printf("%s - %s", GOBU_VERSION_STR, gapp_project_config_get_name(gappMain->config));
    gtk_label_set_text(GTK_LABEL(self->title_window), title);
}

/**
 * Añade un nuevo panel al lado derecho del editor.
 *
 * @param self Puntero al objeto GappMain.
 * @param title Título del nuevo panel.
 * @param module Widget a añadir como contenido del panel.
 * @param is_button_close Indica si se debe mostrar un botón de cierre en la pestaña.
 */
void gapp_append_right_panel(GappMain *self, const gchar *title, GtkWidget *module, gboolean is_button_close)
{
    g_return_if_fail(GAPP_IS_MAIN(self));
    g_return_if_fail(title != NULL);
    g_return_if_fail(GTK_IS_WIDGET(module));

    GtkWidget *label = gtk_label_new(title);

    gapp_widget_notebook_append_page(GTK_NOTEBOOK(self->dnotebook),
                                     label,
                                     module,
                                     is_button_close);
}

/**
 * Añade un nuevo módulo al panel izquierdo de la aplicación.
 *
 * @param self Puntero a la estructura principal de la aplicación.
 * @param icon_name Nombre del icono a mostrar en la pestaña.
 * @param module Widget del módulo a añadir.
 */
void gapp_append_left_panel(GappMain *self, const gchar *icon_name, GtkWidget *module)
{
    g_return_if_fail(self != NULL && icon_name != NULL && module != NULL);

    GtkWidget *icon = gtk_image_new_from_icon_name(icon_name);
    g_return_if_fail(icon != NULL);

    gtk_widget_set_margin_top(icon, 10);
    gtk_widget_set_margin_bottom(icon, 10);

    gint page_num = gtk_notebook_append_page(GTK_NOTEBOOK(self->vnotebook), module, icon);

    if (page_num == -1)
    {
        g_warning("Failed to append page to notebook");
    }
}

// -----------------
// MARK: MAIN-C
// -----------------
int main(int argc, char *argv[])
{
    gappMain = gapp_new();
    return g_application_run(G_APPLICATION(gappMain), argc, argv);
}
