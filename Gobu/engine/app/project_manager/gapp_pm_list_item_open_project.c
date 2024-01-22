#include "gapp_pm_list_item_open_project.h"
#include "gapp_pm_open_project.h"
#include "utility/gb_log.h"
#include "utility/gb_path.h"

void gapp_pm_list_item_open_project(GtkGridView *self, guint position, GobuProjectManager *ctx)
{
    GtkStringObject *obj = g_list_model_get_item(G_LIST_MODEL(gtk_grid_view_get_model(self)), position);
    gb_return_if_fail(position != -1);

    const char *path_project = gtk_string_object_get_string(obj);
    if (gapp_pm_open_project(path_project, gtk_window_get_application(ctx->window)))
    {
        gtk_window_destroy(GTK_WINDOW(ctx->window));
    }
}
