#include "gapp.h"
#include "gobu/gobu.h"
#include "gapp_widget.h"

#include "gapp_project_manager.h"
#include "gapp_project_setting.h"
#include "gapp_project_config.h"

#include "gapp_wviewport.h"
#include "gapp_scene_viewport.h"
#include "gapp_scene_hierarchy.h"
#include "gapp_prefab.h"
#include "gapp_inspector.h"

static GappMain *gappMain;

// -----------------
// MARK:BASE CLASS
// -----------------

struct _GappMain
{
    GtkApplicationWindow parent_instance;
    GtkWidget *headerbar_btn_setting;
    GtkWidget *project_manager;
    GtkWidget *config;
    GtkWidget *window;
    GtkWidget *title_window;
    // modulo main
    GtkWidget *stack;
    // modulos
    GtkWidget *hierarchy;
    GtkWidget *prefab;
    GtkWidget *viewport;
    GtkWidget *inspector;
    // ecs unique world to proyect
    ecs_world_t *world;
    //
    gchar *path_project;
    // resource default icons
    GtkWidget *ricons[GAPP_RESOURCE_ICON_COUNT];
};

G_DEFINE_TYPE(GappMain, gapp_main, GTK_TYPE_APPLICATION)

static void gapp_main_activate(GappMain *app);
static void gapp_headerbar_set_button_visible(GappMain *self, gboolean sensitive);
static void gapp_signal_project_settings_open(GtkWidget *widget, GappMain *self);
static void gapp_signal_project_save(GtkWidget *widget, GappMain *self);
static void gapp_signal_project_preview(GtkWidget *widget, GappMain *self);
static void gapp_signal_open_popover_create_entity(GtkWidget *widget, GappMain *self);
static void gapp_signal_observer_scene_open(ecs_iter_t *it);

static bool gapp_config_init(void);

static void gapp_main_class_init(GappMainClass *klass)
{
    G_APPLICATION_CLASS(klass)->activate = gapp_main_activate;
}

static void gapp_main_init(GappMain *self)
{
    self->config = gapp_project_config_new();
    self->world = gobu_ecs_init();
}

// -----------------
// MARK: PRIVATE API
// -----------------

static void gapp_headerbar_set_button_visible(GappMain *self, gboolean sensitive)
{
    gtk_widget_set_visible(self->headerbar_btn_setting, sensitive);
}

static GappMain *gapp_new(void)
{
    return g_object_new(GAPP_TYPE_MAIN, "application-id", APPLICATION_ID, "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}

static GKeyFile *gapp_config_load_from_file(const char *filename)
{
    GKeyFile *keyfile = g_key_file_new();
    GError *error = NULL;

    gchar *fileconfig = gobu_util_path_build(g_get_current_dir(), "Config", filename);

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

    GtkIconTheme *theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    gtk_icon_theme_add_search_path(theme, "Content/icons/svg/");

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

static void gapp_signal_project_save(GtkWidget *widget, GappMain *self)
{
    g_return_if_fail(self != NULL);

    g_autofree gchar *path_world = gobu_util_path_build(gapp_get_project_path(), "resources", "world.json");
    gobu_ecs_save_to_file(self->world, path_world);
}

static void gapp_signal_project_preview(GtkWidget *widget, GappMain *self)
{
    g_return_if_fail(self != NULL);
    printf("Preview load && save project\n");
    gapp_signal_project_save(widget, self);
}

static void gapp_signal_open_popover_create_entity(GtkWidget *widget, GappMain *self)
{
    GtkWidget *popover = gtk_popover_new();
    gtk_widget_set_parent(popover, widget);
    gtk_popover_set_cascade_popdown(GTK_POPOVER(popover), FALSE);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_popover_set_child(GTK_POPOVER(popover), vbox);
    {
        GtkWidget *btn_item;

        btn_item = gapp_widget_button_new_icon_with_label("background-app-ghost-symbolic", "Sprite");
        gtk_box_append(GTK_BOX(vbox), btn_item);

        btn_item = gapp_widget_button_new_icon_with_label("image-symbolic", "Tiled Sprite");
        gtk_box_append(GTK_BOX(vbox), btn_item);

        gtk_box_append(vbox, gapp_widget_separator_h());

        btn_item = gapp_widget_button_new_icon_with_label("square-outline-thick-symbolic", "Shape Square");
        gtk_box_append(vbox, btn_item);

        btn_item = gapp_widget_button_new_icon_with_label("circle-outline-thick-symbolic", "Shape Circle");
        gtk_box_append(vbox, btn_item);

        btn_item = gapp_widget_button_new_icon_with_label("draw-text-symbolic", "Text");
        gtk_box_append(vbox, btn_item);
    }

    gtk_popover_popup(GTK_POPOVER(popover));
}

static void gapp_signal_observer_scene_open(ecs_iter_t *it)
{
    GappMain *self = it->ctx;
    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];
        inspectorSetEntity(self->inspector, it->world, gobu_scene_get_open(it->world));
    }
}

// -----------------
// MARK: UI
// -----------------

static void gapp_window_set_headerbar(GappMain *app)
{
    app->title_window = gtk_label_new(GAPP_VERSION_STR);

    GtkWidget *header_bar = gtk_header_bar_new();
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), app->title_window);
    gtk_window_set_titlebar(GTK_WINDOW(app->window), header_bar);

    { // #toolbar headerbar
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_header_bar_set_title_widget(header_bar, hbox);
        {
            // botones de la barra de herramientas
        }
    }

    { // #setting project, game
        app->headerbar_btn_setting = gapp_widget_button_new_icon_with_label("open-menu-symbolic", NULL);
        gtk_header_bar_pack_end(header_bar, app->headerbar_btn_setting);
        g_signal_connect(app->headerbar_btn_setting, "clicked", G_CALLBACK(gapp_signal_project_settings_open), app);
    }
}

static GtkWidget *gapp_module_editor(GappMain *app)
{
    GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(hpaned), 230);
    gtk_paned_set_resize_start_child(GTK_PANED(hpaned), FALSE);
    {
        // left modulo, center modulo y right modulo
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_paned_set_start_child(hpaned, box);
        gtk_paned_set_shrink_start_child(GTK_PANED(hpaned), FALSE);

        GtkWidget *stack = adw_view_stack_new();
        {
            GtkWidget *switcher = adw_view_switcher_new();
            adw_view_switcher_set_policy(switcher, ADW_VIEW_SWITCHER_POLICY_WIDE);
            gtk_widget_set_size_request(switcher, 225, -1);
            gapp_widget_set_margin(switcher, 10);
            adw_view_switcher_set_stack(switcher, stack);
            gtk_box_append(box, switcher);
            {
                app->hierarchy = gapp_scene_hierarchy_new();
                adw_view_stack_add_titled_with_icon((stack), app->hierarchy, "hierarchy", "Hierarchy", "edit-select-all-symbolic");

                app->prefab = gapp_prefab_new();
                adw_view_stack_add_titled_with_icon((stack), app->prefab, "prefab", "Prefabs", "application-x-addon-symbolic");
            }
        }
        gtk_box_append(box, stack);

        // center modulo
        GtkWidget *cpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_paned_set_resize_end_child(GTK_PANED(cpaned), FALSE);
        gtk_paned_set_shrink_end_child(GTK_PANED(cpaned), FALSE);
        gtk_paned_set_end_child(hpaned, cpaned);
        {
            GtkWidget *overlay = gtk_overlay_new();
            gtk_paned_set_start_child(cpaned, overlay);
            {
                GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_widget_add_css_class(toolbar, "toolbar_scene");
                // gtk_widget_set_can_target(toolbar, FALSE);
                gtk_overlay_add_overlay(GTK_OVERLAY(overlay), toolbar);
                gtk_widget_set_halign(toolbar, GTK_ALIGN_CENTER);
                gtk_widget_set_valign(toolbar, GTK_ALIGN_START);
                gtk_widget_set_margin_top(toolbar, 10);
                {
                    GtkWidget *btn_item;

                    btn_item = gapp_widget_button_new_icon_with_label("list-add-symbolic", NULL);
                    gtk_box_append(toolbar, btn_item);
                    g_signal_connect(btn_item, "clicked", G_CALLBACK(gapp_signal_open_popover_create_entity), app);

                    gtk_box_append(toolbar, gapp_widget_separator_h());

                    btn_item = gapp_widget_button_new_icon_with_label("square-outline-thick-symbolic", NULL);
                    gtk_box_append(toolbar, btn_item);
                    // g_signal_connect(btn_item, "clicked", G_CALLBACK(gapp_signal_project_preview), app);

                    btn_item = gapp_widget_button_new_icon_with_label("circle-outline-thick-symbolic", NULL);
                    gtk_box_append(toolbar, btn_item);
                    // g_signal_connect(btn_item, "clicked", G_CALLBACK(gapp_signal_project_preview), app);

                    btn_item = gapp_widget_button_new_icon_with_label("draw-text-symbolic", NULL);
                    gtk_box_append(toolbar, btn_item);
                    // g_signal_connect(btn_item, "clicked", G_CALLBACK(gapp_signal_project_preview), app);

                    gtk_box_append(toolbar, gapp_widget_separator_h());

                    btn_item = gapp_widget_button_new_icon_with_label("media-playback-start-symbolic", NULL);
                    g_signal_connect(btn_item, "clicked", G_CALLBACK(gapp_signal_project_preview), app);
                    gtk_box_append(toolbar, btn_item);
                }

                app->viewport = gapp_scene_viewport_new();
                gtk_overlay_set_child(GTK_OVERLAY(overlay), app->viewport);
            }
        }

        // right modulo
        app->inspector = gapp_inspector_new();
        inspectorSetEmpty(app->inspector, "Select an entity to inspect");
        gtk_widget_set_size_request(app->inspector, 300, -1);
        gtk_paned_set_end_child(cpaned, app->inspector);
    }

    gobu_ecs_observer(app->world, gbOnSceneOpen, gapp_signal_observer_scene_open, app);

    return hpaned;
}

static GtkWidget *gapp_module_project_manager(GappMain *app)
{
    app->project_manager = gobu_project_manager_new();
    return app->project_manager;
}

static void gapp_main_activate(GappMain *app)
{
    gapp_config_init();

    app->world = gobu_ecs_init();

    app->window = gtk_window_new();
    gtk_window_set_application(GTK_WINDOW(app->window), GTK_APPLICATION(app));
    gtk_window_set_default_size(GTK_WINDOW(app->window), 1280, 800);
    gtk_widget_set_size_request(app->window, 1280, 800);
    gapp_window_set_headerbar(app);

    app->stack = gtk_stack_new();
    // gtk_stack_set_transition_type(GTK_STACK(app->stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_window_set_child(GTK_WINDOW(app->window), app->stack);

    gtk_stack_add_named(GTK_STACK(app->stack), gapp_module_project_manager(app), "project_manager");
    gtk_stack_add_named(GTK_STACK(app->stack), gapp_module_editor(app), "editor");

    gapp_headerbar_set_button_visible(app, FALSE);
    gtk_window_present(GTK_WINDOW(app->window));
}

// -----------------
// MARK: PUBLIC API
// -----------------

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

GtkWindow *gapp_get_window_instance(void)
{
    return GTK_WINDOW(gappMain->window);
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

    gapp_set_project_path(gobu_util_path_dirname(path));

    gtk_stack_set_visible_child_name(GTK_STACK(self->stack), "editor");
    gapp_headerbar_set_button_visible(self, TRUE);

    g_autofree gchar *title = gobu_util_string_format("%s - %s", GAPP_VERSION_STR, gapp_project_config_get_name(gappMain->config));
    gtk_label_set_text(GTK_LABEL(self->title_window), title);

    // cargamos el world del proyecto
    if (!gobu_ecs_load_from_file(self->world, gobu_util_path_build(gapp_get_project_path(), "resources", "world.json")))
    {
        g_warning("Failed to load project world");
    }

    gobu_scene_reload(self->world);
}

const gchar *gapp_get_project_path(void)
{
    return gobu_util_string(gappMain->path_project);
}

void gapp_set_project_path(const gchar *path)
{
    g_return_if_fail(path != NULL && *path != '\0');
    gappMain->path_project = gobu_util_string(path);
}

// -----------------
// MARK: MAIN-C
// -----------------
int main(int argc, char *argv[])
{
    gappMain = gapp_new();

    return g_application_run(G_APPLICATION(gappMain), argc, argv);
}
