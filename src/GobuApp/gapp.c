#include "gapp.h"
#include "binn/binn_json.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_project_manager.h"
#include "gapp_browser.h"
#include "gapp_level_editor.h"
#include "gapp_project_setting.h"
#include "gapp_project_config.h"
#include "gapp_scene.h"

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
    //
    gchar *path_project;
    // resource
    GtkWidget *ricons[GAPP_RESOURCE_ICON_COUNT];
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

    app->ricons[GAPP_RESOURCE_ICON_SCENE] = gtk_image_new_from_file("D:/software/Gobu/gobu/bin/Content/icons/scene.png");
    app->ricons[GAPP_RESOURCE_ICON_PREFAB] = gtk_image_new_from_file("D:/software/Gobu/gobu/bin/Content/icons/prefab.png");
    app->ricons[GAPP_RESOURCE_ICON_TILEMAP] = gtk_image_new_from_file("D:/software/Gobu/gobu/bin/Content/icons/scene.png");
    app->ricons[GAPP_RESOURCE_ICON_SCRIPT] = gtk_image_new_from_file("D:/software/Gobu/gobu/bin/Content/icons/script.png");
    app->ricons[GAPP_RESOURCE_ICON_ANIM2D] = gtk_image_new_from_file("D:/software/Gobu/gobu/bin/Content/icons/anim2d.png");

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
            gapp_append_left_panel(GAPP_TOOLBAR_LEFT_ICON_BROWSER, app->browser);
            gapp_append_left_panel(GAPP_TOOLBAR_LEFT_ICON_TILE, gtk_label_new("TileEditor"));
        }

        app->dnotebook = gtk_notebook_new();
        gtk_paned_set_end_child(GTK_PANED(hpaned), app->dnotebook);
        {
            app->project_manager = gobu_project_manager_new();
            gapp_append_right_panel(GAPP_RESOURCE_ICON_NONE, "Projects", app->project_manager, FALSE);
        }
    }

    gapp_set_headerbar_button_sensitives(app, FALSE);
    gtk_window_present(GTK_WINDOW(app->window));
}

// -----------------
// MARK:SIGNAL
// -----------------

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

static void gapp_set_headerbar_button_sensitives(GappMain *self, gboolean sensitive)
{
    gtk_widget_set_visible(self->btn_s, sensitive);
    gtk_widget_set_visible(self->btn_p, sensitive);
    gtk_widget_set_visible(self->btn_b, sensitive);
    gtk_widget_set_visible(self->btn_set, sensitive);
}

static GappMain *gapp_new(void)
{
    return g_object_new(GAPP_TYPE_MAIN, "application-id", APPLICATION_ID, "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}

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

GObject *gapp_get_editor_instance(void)
{
    return gappMain;
}

GObject *gapp_get_config_instance(void)
{
    return gappMain->config;
}

ecs_world_t *gapp_get_world_instance(void)
{
    return gappMain->world;
}

GdkPaintable *gapp_get_resource_icon(GappResourceIcon icon)
{
    g_return_val_if_fail(gappMain->ricons[icon] != NULL, NULL);
    return gtk_image_get_paintable(gappMain->ricons[icon]);
}

void gapp_open_project(GappMain *self, const gchar *path)
{
    g_return_if_fail(GAPP_IS_MAIN(self));
    g_return_if_fail(path != NULL && *path != '\0');

    gapp_set_project_path(pathDirname(path));

    gapp_append_right_panel(GAPP_RESOURCE_ICON_SCENE, "Untitle~", gapp_scene_new(), TRUE);
    gtk_notebook_remove_page(GTK_NOTEBOOK(self->dnotebook), 0);
    gtk_widget_set_visible(GTK_WIDGET(self->vnotebook), TRUE);
    gapp_set_headerbar_button_sensitives(self, TRUE);
    browserSetFolderContent(GAPP_BROWSER(self->browser));

    g_autofree gchar *title = g_strdup_printf("%s - %s", GOBU_VERSION_STR, gapp_project_config_get_name(gappMain->config));
    gtk_label_set_text(GTK_LABEL(self->title_window), title);
}

void gapp_append_right_panel(GappResourceIcon icon, const gchar *title, GtkWidget *module, gboolean is_button_close)
{
    g_return_if_fail(GAPP_IS_MAIN(gappMain));
    g_return_if_fail(title != NULL);
    g_return_if_fail(GTK_IS_WIDGET(module));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    if (icon != GAPP_RESOURCE_ICON_NONE)
        gtk_box_append(GTK_BOX(box), gtk_image_new_from_paintable(gapp_get_resource_icon(icon)));
        
    gtk_box_append(GTK_BOX(box), gtk_label_new(title));

    gapp_widget_notebook_append_page(GTK_NOTEBOOK(gappMain->dnotebook),
                                     box,
                                     module,
                                     is_button_close);
}

void gapp_append_left_panel(const gchar *icon_name, GtkWidget *module)
{
    g_return_if_fail(gappMain != NULL && icon_name != NULL && module != NULL);

    GtkWidget *icon = gtk_image_new_from_icon_name(icon_name);
    g_return_if_fail(icon != NULL);

    gtk_widget_set_margin_top(icon, 10);
    gtk_widget_set_margin_bottom(icon, 10);

    gint page_num = gtk_notebook_append_page(GTK_NOTEBOOK(gappMain->vnotebook), module, icon);

    if (page_num == -1)
    {
        g_warning("Failed to append page to notebook");
    }
}

const gchar *gapp_get_project_path(void)
{
    return g_strdup(gappMain->path_project);
}

void gapp_set_project_path(const gchar *path)
{
    g_return_if_fail(path != NULL && *path != '\0');
    gappMain->path_project = g_strdup(path);
}

// -----------------
// MARK: MAIN-C
// -----------------
int main(int argc, char *argv[])
{
    gappMain = gapp_new();
    return g_application_run(G_APPLICATION(gappMain), argc, argv);
}
