#include "gobu.h"
#include "gobu-project.h"

static GobuContext *ctx;

GobuContext *GobuContextGet()
{
    return ctx;
}

void GobuContext_init(void)
{
    ctx = g_new0(GobuContext, 1);

    GobuAPI_projectInit();
    GobuAPI_jsonInit();
}

void GobuContext_free(void)
{
    GobuAPI_projectFree();
    GobuAPI_jsonFree();
    
    g_free(ctx);
}