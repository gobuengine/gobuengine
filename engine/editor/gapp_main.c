#include "gapp_main.h"

GAPP *EditorCore;

static void editor_setting_init(void)
{
    g_info("GOBU-APP: Setting load\n");

    gchar* full_path = g_build_filename(g_get_current_dir(), "config", "editor.config", NULL);
    EditorCore->setting.data = binn_serialize_from_file(full_path);

    gboolean dark_theme = binn_object_bool(EditorCore->setting.data, GAPP_THEME_DEFAULT);
    const gchar* style_scheme = dark_theme == TRUE ? GAPP_THEME_DARK_SCRIPT : GAPP_THEME_LIGHT_SCRIPT;

    // THEME
    gapp_widget_theme_init(dark_theme);

    // SCRIPT EDITOR
    GtkSourceStyleSchemeManager* scheme_manager = gtk_source_style_scheme_manager_get_default();
    EditorCore->setting.scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, style_scheme);

    GtkSourceLanguageManager* managerLanguage = gtk_source_language_manager_get_default();
    EditorCore->setting.language = gtk_source_language_manager_get_language(managerLanguage, GAPP_SCRIPT_LANGUAGE_DEFAULT);
}

static void editor_free(GtkApplication* self, GtkWindow* window, gpointer data)
{
    if (gtk_application_get_active_window(self) == NULL)
    {
        g_print("GOBU-APP: free\n");
        binn_free(EditorCore->setting.data);
        g_object_unref(EditorCore->setting.scheme);
        g_object_unref(EditorCore->setting.language);
        g_free(EditorCore);
    }
}

static void editor_init(GtkApplication* app)
{
    EditorCore = g_new0(GAPP, 1);

    g_print("GOBU-EDITOR: Initialize\n");

    editor_setting_init();

    gapp_project_manager_window_new(app);
}

int main(int argc, char* argv[])
{
    GtkApplication* app = gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(editor_init), NULL);
    g_signal_connect(app, "window-removed", G_CALLBACK(editor_free), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}

