#include "GobuProjectEditor.h"
#include "GobuUiProjectEditor.h"
#include "gobu/gobu.h"

static GobuProjectEditor *ctx;

void GobuProjectEditorInit(GtkApplication *app)
{
    g_info("GOBU Project Editor Init\n");
    ctx = g_new0(GobuProjectEditor, 1);

    GobuProjectEditorResourceInit();
    
    // de-serialize el game.gcomponent
    ctx->components = GobuContextGet()->JSON.parse_from_file(GobuContextGet()->project.component);

    GobuUiProjectEditor(app, ctx);
}

void GobuProjectEditorFree(void)
{
    GobuProjectEditorResourceFree();
    g_free(ctx);
}
