#include "GobuUiProjectEditor.h"
#include "WorldEditor/GobuWorldEditor.h"
#include "ComponentSystem/GobuComponentSystem.h"
#include "gobu/gobu-gobu.h"

void GobuUiProjectEditor(GtkApplication *app, GobuProjectEditor *ctx)
{
    ctx->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(ctx->window), g_strdup_printf("GobuEngine - %s", gobu_project_get_name()));
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), 1280, 720);

    ctx->notebook = gtk_notebook_new();
    gtk_window_set_child(GTK_WINDOW(ctx->window), ctx->notebook);

    GobuWorldEditorInit(ctx);
    GobuComponentSystemInit(ctx);

    gtk_window_present(GTK_WINDOW(ctx->window));
}
