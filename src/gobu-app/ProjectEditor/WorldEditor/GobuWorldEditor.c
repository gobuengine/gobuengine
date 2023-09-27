#include "GobuWorldEditor.h"
#include "GobuUiWorldEditor.h"
#include "GobuUiProjectEditor.h"

void GobuWorldEditorInit(GobuProjectEditor *ctx)
{
    GtkWidget *world_edit = GobuUiWorldEditor();
    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), world_edit, gtk_label_new("Untitled"));
}
