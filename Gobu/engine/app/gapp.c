#include <stdio.h>
#include <gtk/gtk.h>
#include "config.h"
#include "gapp_config.h"
#include "project_manager/gapp_project_manager.h"
#include "utility/gb_fs.h"
#include "utility/gb_path.h"
#include "utility/gb_log.h"

void gapp_theme_set_dark_mode(gboolean dark_mode)
{
    if (dark_mode)
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
}

void gapp_theme_load_default(void)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "Content/theme/default.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void gapp_load_editor_config(void)
{
    GKeyFile *config = gapp_config_load_from_file("editor.config");

    // THEME
    gapp_theme_set_dark_mode(gapp_config_editor_dark_mode(config));
    gapp_theme_load_default();

    gapp_config_close(config);
}

static void editor_free(GtkApplication *self, GtkWindow *window, gpointer data)
{
    if (gtk_application_get_active_window(self) == NULL)
    {
        gb_log_info("GOBU-APP: free\n");
    }
}

static void editor_init(GtkApplication *app)
{
    gb_log_info("GOBU-EDITOR: Initialize\n");

    gapp_load_editor_config();
    gapp_project_manager_window_new(app);
}

int main(int argc, char *argv[])
{
    GtkApplication *app = gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(editor_init), NULL);
    g_signal_connect(app, "window-removed", G_CALLBACK(editor_free), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
