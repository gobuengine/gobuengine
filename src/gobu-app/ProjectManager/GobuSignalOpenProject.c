#include "GobuSignalOpenProject.h"
#include "ProjectEditor/GobuProjectEditor.h"
#include "gobu/gobu.h"

static void GobuSignalRadyOpenProject(GObject *source, GAsyncResult *res, GobuProjectManager *ctx)
{
    GFile *file;
    GError *error = NULL;

    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, &error);
    if (!file)
    {
        g_error("%s\n", error->message);
        g_error_free(error);
        g_return_if_fail(file);
    }

    GobuContext *gobu = GobuContextGet();

    char *path_str = g_file_get_path(file);
    if (gobu->project.load(path_str))
    {
        GobuProjectEditorInit(gtk_window_get_application(ctx->window));
    }

    g_free(path_str);
    gtk_window_destroy(GTK_WINDOW(ctx->window));
}

void GobuSignalOpenProject(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkFileDialog *file_dialog;

    file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(GTK_FILE_DIALOG(file_dialog), GTK_WINDOW(ctx->window), NULL, GobuSignalRadyOpenProject, ctx);
}