#include "GobuProjectManager.h"
#include "GobuUiProjectManager.h"

static GobuProjectManager *ctx;

void GobuProjectManagerInit(GtkApplication *app)
{
    ctx = g_new0(GobuProjectManager, 1);

    GobuUiProjectManager(app, ctx);
}

void GobuProjectManagerFree(void)
{
    g_free(ctx);
}
