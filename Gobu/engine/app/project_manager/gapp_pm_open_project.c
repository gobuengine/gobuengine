#include "gapp_pm_open_project.h"
#include "gapp_pm_register_project.h"
#include "editor/project-editor.h"

#include "utility/gb_log.h"
#include "gb_project.h"

static void ready_callback(GObject *source, GAsyncResult *res, GobuProjectManager *ctx);

bool gapp_pm_open_project(const char *path_str, GtkApplication *app)
{
    if (gb_project_load_from_file_from_file(path_str))
    {
        gapp_project_editor_window_new(app);
        gapp_pm_register_project_set(path_str);
        return TRUE;
    }

    return FALSE;
}

void gapp_pm_signal_dialog_open_project(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkFileFilter *filter;
    GtkFileDialog *file_dialog;

    file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(file_dialog, "Select Project");
    gtk_file_dialog_set_modal(file_dialog, TRUE);

    filter = gtk_file_filter_new();
    gtk_file_filter_add_suffix(filter, "gobuproject");

    GListStore *list_store = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(list_store, filter);

    gtk_file_dialog_set_filters(file_dialog, G_LIST_MODEL(list_store));

    gtk_file_dialog_open(file_dialog, GTK_WINDOW(ctx->window), NULL, ready_callback, ctx);
}

static void ready_callback(GObject *source, GAsyncResult *res, GobuProjectManager *ctx)
{
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, NULL);
    gb_return_if_fail(file != NULL);

    char *path_str = g_file_get_path(file);
    if (gapp_pm_open_project(path_str, gtk_window_get_application(ctx->window)))
    {
        gtk_window_destroy(GTK_WINDOW(ctx->window));
    }
    g_free(path_str);
}

