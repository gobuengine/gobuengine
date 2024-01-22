#include "gapp_config.h"

GKeyFile *gapp_config_load_from_file(const char *filename)
{
    gchar *path = g_build_filename(g_get_current_dir(), "Config", filename, NULL);
    GKeyFile *config = g_key_file_new();
    g_key_file_load_from_file(config, path, G_KEY_FILE_NONE, NULL);

    g_free(path);

    return config;
}

void gapp_config_close(GKeyFile *config)
{
    g_key_file_unref(config);
}

char *gapp_config_editor_theme_script(GKeyFile *config)
{
    return g_key_file_get_string(config, "editor", "themeScript", NULL);
}

bool gapp_config_editor_dark_mode(GKeyFile *config)
{
    return g_key_file_get_boolean(config, "editor", "darkMode", NULL);
}

