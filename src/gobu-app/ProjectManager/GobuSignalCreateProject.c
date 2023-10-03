#include "GobuSignalCreateProject.h"
#include "GobuFnCreateProject.h"
#include "ProjectEditor/GobuProjectEditor.h"
#include "gobu/gobu-gobu.h"

void GobuSignalCreateProject(GtkWidget *widget, GobuProjectManager *ctx)
{
    const gchar *name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
    const gchar *path = gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser));

    if (GobuFnCreateProject(name, path))
    {
        gchar *path_str = g_build_filename(path, name, NULL);
        if (gobu_project_load(path_str))
        {
            GobuProjectEditorInit(gtk_window_get_application(ctx->window));
        }
        gtk_window_destroy(GTK_WINDOW(ctx->window));
        g_free(path_str);
    }
}