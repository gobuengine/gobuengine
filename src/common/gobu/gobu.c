#include "gobu.h"

void gobu_init(gbContext *ctx, int width, int height)
{
    ctx->backend = gfxb_create();
    ctx->width = width;
    ctx->height = height;
}

void gobu_destroy(gbContext *ctx)
{
    gfxb_destroy(ctx->backend);
}

void gobu_present(gbContext *ctx)
{
    gfxb_present(ctx->backend, ctx->width, ctx->height);
}
