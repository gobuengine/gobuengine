#include <gtk/gtk.h>
#include "config.h"
#include "gobu-app.h"
#include "gobu_widget.h"
#include "project_manager.h"

GobuApp *APP; // public global

static void gobu_app_setting_init(void)
{
    g_info("GOBU: Setting Editor load\n");

    gchar *full_path = g_build_filename(g_get_current_dir(), "config", "editor.config", NULL);
    APP->setting.data = bugo_json_load_from_file(full_path);

    gboolean dark_theme = binn_object_bool(APP->setting.data, "app-theme-dark");
    const gchar *style_scheme = dark_theme == TRUE ? "Adwaita-dark" : "Adwaita";
    const gchar *language = "c";

    // THEME
    gobu_widget_theme_init(dark_theme);

    // SCRIPT EDITOR
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    APP->setting.scripts.style_scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, style_scheme);

    GtkSourceLanguageManager *managerLanguage = gtk_source_language_manager_get_default();
    APP->setting.scripts.source_language = gtk_source_language_manager_get_language(managerLanguage, language);
}

static void gobu_app_free(GtkApplication *self, GtkWindow *window, gpointer data)
{
    if (gtk_application_get_active_window(self) == NULL)
    {
        g_print("GOBU: APP free\n");
        binn_free(APP->setting.data);
        g_object_unref(APP->setting.scripts.style_scheme);
        g_object_unref(APP->setting.scripts.source_language);
        g_free(APP);
    }
}

static void on_activate(GtkApplication *app)
{
    g_print("GOBU: APP Init\n");
    APP = g_new0(GobuApp, 1);

    gobu_app_setting_init();
    gobu_project_manager_window_new(app);
}

int main(int argc, char *argv[])
{
    GtkApplication *app = gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "window-removed", G_CALLBACK(gobu_app_free), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}