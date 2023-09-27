#include <gtk/gtk.h>
#include "config.h"
#include "GobuWidgets.h"
#include "ProjectManager/GobuProjectManager.h"
#include "ProjectEditor/GobuProjectEditor.h"
#include "gobu/gobu.h"

static void on_activate(GtkApplication *app)
{
    GobuWidgetThemeInit();
    GobuContext_init();

    GobuProjectManagerInit(app);
}

int main(int argc, char *argv[])
{
    GtkApplication *app = gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}