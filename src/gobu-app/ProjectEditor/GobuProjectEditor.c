#include "GobuProjectEditor.h"
#include "GobuUiProjectEditor.h"
#include "gobu/gobu-gobu.h"

static GobuProjectEditor *ctx;

void GobuProjectEditorInit(GtkApplication *app)
{
    g_info("GOBU Project Editor Init\n");
    ctx = g_new0(GobuProjectEditor, 1);

    GobuProjectEditorResourceInit();
    
    // de-serialize el game.gcomponent
    gchar *gcomponent = g_build_filename(GobuProjectGetPath(), "Scripts", "game.gcomponent", NULL);
    ctx->components = GobuJsonLoadFromFile(gcomponent);
    g_free(gcomponent);

    GobuUiProjectEditor(app, ctx);
}

void GobuProjectEditorFree(void)
{
    GobuProjectEditorResourceFree();
    g_free(ctx);
}
