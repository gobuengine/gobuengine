#include "gapp_pm_check_project_name_availability.h"
#include "utility/gb_path.h"
#include "utility/gb_fs.h"
#include "config.h"

void gapp_pm_check_project_name_availability(GtkWidget *entry, GobuProjectManager *ctx)
{
    bool test = TRUE;

    if (gtk_entry_get_text_length(GTK_ENTRY(entry)) > 0)
    {
        const char *name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
        const char *path = gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser));

        char *path_full = gb_path_join(path, name, NULL);

        test = gb_fs_file_exist(gb_path_join(path_full, FILE_GAME_PROJECT, NULL));
        g_free(path_full);
    }

    gtk_widget_set_sensitive(ctx->btn_create, !test);
}
