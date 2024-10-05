#include "gapp.h"
#include "gapp_widget.h"
#include "gapp_editor.h"

static GobuEditorMain *gappMain;

static void activate(GtkApplication *app, gpointer user_data)
{
    gapp_config_init();

    gappMain = gobu_editor_main_new(app);
    gobu_editor_main_show(gappMain);
}

static void window_removed(GtkApplication *app, gpointer user_data)
{
    g_application_quit(G_APPLICATION(app));
}

GObject *gapp_get_editor_instance(void)
{
    return gappMain;
}

GObject *gapp_get_config_instance(void)
{
    return gapp_editor_get_config(gappMain);
}

int main(int argc, char *argv[])
{
    g_autoptr(GtkApplication) app = adw_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_signal_connect(app, "window-removed", G_CALLBACK(window_removed), NULL);

    return g_application_run(G_APPLICATION(app), argc, argv);
}
