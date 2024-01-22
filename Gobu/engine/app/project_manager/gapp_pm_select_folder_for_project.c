#include "gapp_pm_select_folder_for_project.h"

static void response_file_chooser_folder(GtkFileDialog *source, GAsyncResult *res, GtkWidget *widget)
{
    GError *error = NULL;

    GFile *file = gtk_file_dialog_select_folder_finish(source, res, &error);

    if (error == NULL)
    {
        gtk_button_set_label(GTK_BUTTON(widget), g_file_get_path(file));
    }
}

void gapp_pm_signal_select_folder_for_project(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_select_folder(GTK_FILE_DIALOG(dialog), GTK_WINDOW(ctx->window), NULL, response_file_chooser_folder, widget);
}

