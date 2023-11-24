#include "project_manager_signal.h"
#include "project_manager_fn.h"
#include "gobu_project.h"
#include "gobu_utility.h"
#include "project_editor.h"

static void response_open_project_ready(GObject* source, GAsyncResult* res, GobuProjectManager* ctx)
{
    GFile* file;
    GError* error = NULL;

    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, &error);

    if (error != NULL)
        g_error_free(error);
    g_return_if_fail(file);

    char* path_str = g_file_get_path(file);
    if (gb_project_load(path_str)) {
        gapp_project_editor_window_new(gtk_window_get_application(ctx->window));
    }

    g_free(path_str);
    gtk_window_destroy(GTK_WINDOW(ctx->window));
}

void project_manager_signal_file_dialog_open_project(GtkWidget* widget, GobuProjectManager* ctx)
{
    GtkFileDialog* file_dialog;
    file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(GTK_FILE_DIALOG(file_dialog),
        GTK_WINDOW(ctx->window), NULL, response_open_project_ready, ctx);
}

static void response_file_chooser_folder(GtkDialog* dialog, gint response_id, GtkWidget* widget)
{
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        GFile* files = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        gtk_button_set_label(GTK_BUTTON(widget), g_file_get_path(files));
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void project_manager_signal_open_dialog_file_chooser_folder(GtkWidget* widget, GobuProjectManager* ctx)
{
    GtkWidget* self = gtk_file_chooser_dialog_new("Select folder",
                                                  GTK_WINDOW(ctx->window),
                                                  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                  "Cancel", GTK_RESPONSE_CANCEL,
                                                  "Select", GTK_RESPONSE_ACCEPT, NULL);

    g_signal_connect(self, "response",
        G_CALLBACK(response_file_chooser_folder), widget);

    gtk_widget_show(self);
    gtk_window_set_modal(GTK_WINDOW(self), TRUE);
}

void project_manager_signal_create_project(GtkWidget* widget, GobuProjectManager* ctx)
{
    const gchar* name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
    const gchar* path = gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser));

    if (project_manager_fn_create_project(name, path))
    {
        gchar* path_str = gb_path_join(path, name, "game.gobuproject", NULL);
        if (gb_project_load(path_str))
        {
            gapp_project_editor_window_new(gtk_window_get_application(ctx->window));
        }
        gtk_window_destroy(GTK_WINDOW(ctx->window));
        g_free(path_str);
    }
}

void project_manager_signal_change_entry_check_name_project(GtkWidget* entry, GobuProjectManager* ctx)
{
    gboolean test = FALSE;

    if (gtk_entry_get_text_length(entry) > 0)
    {
        const gchar* name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
        const gchar* path = gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser));

        gchar* path_full = gb_path_join(path, name, NULL);

        test = g_file_test(gb_path_join(path_full, FILE_GAME_PROJECT, NULL), G_FILE_TEST_EXISTS);
        g_free(path_full);
    }

    gtk_widget_remove_css_class(ctx->btn_create, "suggested-action");
    gtk_widget_set_sensitive(ctx->btn_create, !test);
}
