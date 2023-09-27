#include "gobu-project.h"
#include "gobu.h"

gboolean GobuAPI_projectLoad(const gchar *path)
{
    gboolean test = g_file_test(path, G_FILE_TEST_EXISTS);
    if (test == TRUE)
    {
        GobuContext *ctx = GobuContextGet();
        ctx->project.path = g_strdup(g_dirname(path));
        ctx->project.name = g_basename(ctx->project.path);
        ctx->project.component = g_build_filename(ctx->project.path, "Scripts", "game.gcomponent", NULL);
    }
    return test;
}

void GobuAPI_projectInit(void)
{
    GobuContext *ctx = GobuContextGet();

    ctx->project.load = GobuAPI_projectLoad;
}

void GobuAPI_projectFree(void)
{
}
