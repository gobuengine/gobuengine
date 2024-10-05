
#include "gapp_config.h"
#include "gapp_widget.h"

GKeyFile *gapp_config_load_from_file(const char *filename)
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

// config file properties
bool gapp_config_get_bool(GKeyFile *keyfile, const char *group, const char *key)
{
    return g_key_file_get_boolean(keyfile, group, key, NULL);
}

bool gapp_config_init()
{
    GKeyFile *keyfile = gapp_config_load_from_file("editor.config.ini");
    if (keyfile == NULL)
        return FALSE;

    // load config file properties
    bool darkmode = gapp_config_get_bool(keyfile, "Editor", "darkMode");
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
