#include "GobuWorldEditor.h"
#include "GobuUiWorldEditor.h"
#include "GobuUiProjectEditor.h"
#include "gobu/gobu-gobu.h"

void GobuWorldEditorInit(GobuProjectEditor *ctx)
{
    gobu_ecs_world_init();

    GtkWidget *world_edit = GobuUiWorldEditor();
    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), world_edit, gtk_label_new("Untitled"));
}
