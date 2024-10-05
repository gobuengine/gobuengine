#include "gapp_editor.h"
#include "binn/binn_json.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_project_manager.h"
#include "gapp_browser.h"
#include "gapp_level_editor.h"
#include "gapp_project_setting.h"
#include "gapp.h"

#include "gapp_project_config.h"

// -----------------
// MARK:BASE CLASS
// -----------------

struct _GobuEditorMain
{
    GtkApplicationWindow parent_instance;
    GtkWidget *dnotebook, *vnotebook;
    GtkWidget *btn_s;
    GtkWidget *btn_p;
    GtkWidget *btn_b;
    GtkWidget *btn_set;
    GtkWidget *project_manager;
    GtkWidget *browser;
    GtkWidget *toast_overlay;

    // config project
    GtkWidget *config;
};

static void gobu_editor_ui_setup(GobuEditorMain *self);
static void gobu_editor_main_dispose(GObject *object);

G_DEFINE_TYPE(GobuEditorMain, gobu_editor_main, GTK_TYPE_APPLICATION_WINDOW)

static void gobu_editor_main_dispose(GObject *object)
{
    GobuEditorMain *self = GOBU_EDITOR_MAIN(object);

    g_clear_pointer(&self, gtk_widget_unparent);

    // gobu_project_config_free(TRUE);

    G_OBJECT_CLASS(gobu_editor_main_parent_class)->dispose(object);
}

static void gobu_editor_main_class_init(GobuEditorMainClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gobu_editor_main_dispose;
}

static void gobu_editor_main_init(GobuEditorMain *self)
{
    self->config = gapp_project_config_new();
    gobu_editor_ui_setup(self);
}

// -----------------
// MARK:FUNCTION
// -----------------

static void gobu_editor_fn_left_panel_append(GobuEditorMain *self, const gchar *icon, GtkWidget *module)
{
    GtkWidget *iconpga = gtk_image_new_from_icon_name(icon);
    gtk_image_set_icon_size(GTK_IMAGE(iconpga), GTK_ICON_SIZE_NORMAL);
    gtk_widget_set_margin_top(iconpga, 10);
    gtk_widget_set_margin_bottom(iconpga, 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(self->vnotebook), module, iconpga);
}

static void gobu_editor_fn_set_sensitives(GobuEditorMain *self, gboolean sensitive)
{
    gtk_widget_set_sensitive(self->btn_s, sensitive);
    gtk_widget_set_sensitive(self->btn_p, sensitive);
    gtk_widget_set_sensitive(self->btn_b, sensitive);
    gtk_widget_set_sensitive(self->btn_set, sensitive);
}

// -----------------
// MARK:SIGNAL
// -----------------

static void gapp_editor_s_project_settings_open(GtkWidget *widget, GobuEditorMain *self)
{
    GtkWidget *setting = gobu_project_setting_new();
    gobu_project_setting_show(setting, self);
}

// -----------------
// MARK:UI
// -----------------

static void gobu_editor_ui_setup(GobuEditorMain *self)
{
    g_return_if_fail(GOBU_IS_EDITOR_MAIN(self));

    gtk_window_set_title(GTK_WINDOW(self), "GobuEditor");
    gtk_window_set_default_size(GTK_WINDOW(self), 1280, 800);
    // gtk_window_set_resizable(GTK_WINDOW(self), FALSE);

    {
        GtkWidget *header_bar = gtk_header_bar_new();
        gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar),
                                  gtk_label_new(GOBU_VERSION_STR));
        gtk_window_set_titlebar(GTK_WINDOW(self), header_bar);

        { // #toolbar headerbar
            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_header_bar_set_title_widget(header_bar, hbox);
            self->btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save all");
            gtk_box_append(hbox, self->btn_s);

            self->btn_p = gapp_widget_button_new_icon_with_label("applications-games-symbolic", "Preview");
            gtk_box_append(hbox, self->btn_p);

            self->btn_b = gapp_widget_button_new_icon_with_label("drive-optical-symbolic", "Build");
            gtk_box_append(hbox, self->btn_b);
        }

        { // #setting project, game
            self->btn_set = gapp_widget_button_new_icon_with_label("preferences-other-symbolic", "Project Settings");
            gtk_header_bar_pack_end(header_bar, self->btn_set);
            g_signal_connect(self->btn_set, "clicked", G_CALLBACK(gapp_editor_s_project_settings_open), self);
        }
    }

    GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(hpaned), 250);
    // gtk_paned_set_shrink_start_child(GTK_PANED(hpaned), FALSE);
    // gtk_paned_set_wide_handle(GTK_PANED(hpaned), TRUE);
    gtk_paned_set_resize_start_child(GTK_PANED(hpaned), FALSE);
    gtk_window_set_child(GTK_WINDOW(self), hpaned);
    {
        self->vnotebook = gtk_notebook_new();
        gtk_notebook_set_tab_pos(GTK_NOTEBOOK(self->vnotebook), GTK_POS_LEFT);
        gtk_widget_set_visible(GTK_NOTEBOOK(self->vnotebook), FALSE);
        gtk_paned_set_start_child(GTK_PANED(hpaned), self->vnotebook);
        {
            self->browser = gapp_browser_new();
            gobu_editor_fn_left_panel_append(self, "edit-copy", self->browser);
        }

        self->dnotebook = gtk_notebook_new();
        gtk_paned_set_end_child(GTK_PANED(hpaned), self->dnotebook);
        {
            self->project_manager = gobu_project_manager_new();
            gapp_editor_append_right_panel(self, "Projects", self->project_manager, FALSE);
        }
    }

    gobu_editor_fn_set_sensitives(self, FALSE);
}

// -----------------
// MARK:API
// -----------------

/**
 * gobu_editor_main_new:
 * @app: La instancia de GtkApplication a la que se asociará el GobuEditorMain.
 *
 * Crea una nueva instancia de GobuEditorMain.
 *
 * Returns: (transfer full): Una nueva instancia de #GobuEditorMain.
 *   Usa g_object_unref() cuando ya no la necesites.
 */
GobuEditorMain *gobu_editor_main_new(GtkApplication *app)
{
    return g_object_new(GOBU_TYPE_EDITOR_MAIN, "application", app, NULL);
}

/**
 * gobu_editor_main_show:
 * @self: La instancia de GobuEditorMain a mostrar.
 *
 * Muestra la ventana del GobuEditorMain, trayéndola al frente
 * si ya estaba visible.
 *
 * Esta función es parte de la API pública de GobuEditorMain.
 */
void gobu_editor_main_show(GobuEditorMain *self)
{
    g_return_if_fail(GOBU_IS_EDITOR_MAIN(self));
    gtk_window_present(GTK_WINDOW(self));
}

/**
 * Inicializa un proyecto en el editor.
 *
 * @param self Puntero al objeto GobuEditorMain.
 * @param path Ruta del proyecto a inicializar.
 */
void gobu_editor_main_init_project(GobuEditorMain *self, const gchar *path)
{
    g_return_if_fail(GOBU_IS_EDITOR_MAIN(self));
    g_return_if_fail(path != NULL && *path != '\0');

    // Crear una escena inicial
    gapp_editor_append_right_panel(self, "main~", gobu_level_editor_new(), TRUE);

    // Cerrar la página de proyectos
    gtk_notebook_remove_page(GTK_NOTEBOOK(self->dnotebook), 0);

    // Mostrar el notebook del panel izquierdo
    gtk_widget_set_visible(GTK_WIDGET(self->vnotebook), TRUE);

    // Habilitar los botones del título-headerbar
    gobu_editor_fn_set_sensitives(self, TRUE);

    // Cambiar el path del navegador
    g_autofree gchar *data_path = gobu_path_join(gobu_path_dirname(path), "Data", NULL);
    gapp_browser_set_folder(self->browser, data_path);
}

/**
 * Añade un nuevo panel al lado derecho del editor.
 *
 * @param self Puntero al objeto GobuEditorMain.
 * @param title Título del nuevo panel.
 * @param module Widget a añadir como contenido del panel.
 * @param is_button_close Indica si se debe mostrar un botón de cierre en la pestaña.
 */
void gapp_editor_append_right_panel(GobuEditorMain *self, const gchar *title, GtkWidget *module, gboolean is_button_close)
{
    g_return_if_fail(GOBU_IS_EDITOR_MAIN(self));
    g_return_if_fail(title != NULL);
    g_return_if_fail(GTK_IS_WIDGET(module));

    GtkWidget *label = gtk_label_new(title);

    gapp_widget_notebook_append_page(GTK_NOTEBOOK(self->dnotebook),
                                     label,
                                     module,
                                     is_button_close);
}

/**
 * Obtiene el widget de configuración del editor.
 *
 * Esta función devuelve el widget de configuración asociado con el editor principal.
 *
 * @param self El objeto GobuEditorMain del cual se obtendrá la configuración.
 * @return El widget de configuración (GtkWidget*), o NULL si ocurre un error.
 */
GtkWidget *gapp_editor_get_config(GobuEditorMain *self)
{
    g_return_val_if_fail(GOBU_IS_EDITOR_MAIN(self), NULL);
    return self->config;
}

