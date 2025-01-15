#include "gobu-gfx.h"
#include "gobu-gfx-internal.h"

go_bgfx_context *go_gfx_context(void)
{
    return go_gfxptr.bctx;
}

go_font_t go_gfx_font_default(void)
{
    return go_gfxptr.font_default;
}

void *go_gfx_font_context(void)
{
    return go_bgfx_font_context();
}

go_public void go_gfx_init(go_gfx_context_t *ctx, int width, int height)
{
    *ctx = (go_gfx_context_t){0};
    ctx->bctx = go_bgfx_init();
    go__global_gfx_ptr = ctx;

    go_gfxptr.width = width;
    go_gfxptr.height = height;
    go_gfxptr.framebuffer_id = 0;

    go_gfxptr.font_default = go_font_load_from_file("Content/fonts/Silkscreen-Regular.ttf");
}

go_public void go_gfx_shutdown(void)
{
    go_bgfx_destroy();
    free(go__global_gfx_ptr);
}

go_public void go_gfx_set_viewport(int width, int height)
{
    go_gfx_viewport_set_resize(width, height);
    go_gfx_matrix_mode(GFXB_PROJECTION);
    go_gfx_load_identity();

    go_gfx_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

    go_gfx_matrix_mode(GFXB_MODELVIEW);
    go_gfx_load_identity();
}

go_public void go_gfx_viewport_set_resize(int width, int height)
{
    go_gfxptr.width = width;
    go_gfxptr.height = height;
}

go_public void go_gfx_viewport_begin(void)
{
    go_bgfx_viewport_begin();
}

go_public void go_gfx_viewport_end(void)
{
    go_bgfx_viewport_end();
}

go_public int go_gfx_viewport_width(void)
{
    return go_gfxptr.width;
}

go_public int go_gfx_viewport_height(void)
{
    return go_gfxptr.height;
}

go_public void go_gfx_viewport_set_framebuffer(int framebuffer_id)
{
    go_gfxptr.framebuffer_id = framebuffer_id;
}

go_public int go_gfx_viewport_framebuffer(void)
{
    return go_gfxptr.framebuffer_id;
}

go_public void go_gfx_viewport_color(uint8_t r, uint8_t g, uint8_t b)
{
    go_bgfx_viewport_color(r, g, b);
}

go_public void go_gfx_begin(int mode)
{
    go_bgfx_begin(mode);
}

go_public void go_gfx_end(void)
{
    go_bgfx_end();
}

go_public void go_gfx_vertex2f_t2f(float x, float y, float u, float v)
{
    go_bgfx_vertex2f_t2f(x, y, u, v);
}

go_public void go_gfx_vertex2f(float x, float y)
{
    go_bgfx_vertex2f(x, y);
}

go_public void go_gfx_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    go_bgfx_color4b(r, g, b, a);
}

go_public void go_gfx_viewport(int x, int y, int width, int height)
{
    go_bgfx_viewport(x, y, width, height);
}

go_public void go_gfx_scissor(int x, int y, int width, int height)
{
    go_bgfx_scissor(x, y, width, height);
}

go_public void go_gfx_ortho(float l, float r, float b, float t, float n, float f)
{
    go_bgfx_ortho(l, r, b, t, n, f);
}

go_public void go_gfx_matrix_mode(int mode)
{
    go_bgfx_matrix_mode(mode);
}

go_public void go_gfx_enable_texture(void)
{
    go_bgfx_enable_texture();
}

go_public void go_gfx_disable_texture(void)
{
    go_bgfx_disable_texture();
}

go_public go_texture_t go_gfx_create_texture(unsigned char *data, int width, int height)
{
    return go_bgfx_create_texture(data, width, height);
}

go_public void go_gfx_destroy_texture(go_texture_t texture)
{
    go_bgfx_destroy_texture(texture);
}

go_public void go_gfx_texture_apply(go_texture_t texture)
{
    go_bgfx_texture_apply(texture);
}

go_public void go_gfx_load_identity(void)
{
    go_bgfx_load_identity();
}

go_public void go_gfx_push_transform(void)
{
    go_bgfx_push_transform();
}

go_public void go_gfx_pop_transform(void)
{
    go_bgfx_pop_transform();
}

go_public void go_gfx_translate(float x, float y)
{
    go_bgfx_translate(x, y);
}

go_public void go_gfx_scale(float x, float y)
{
    go_bgfx_scale(x, y);
}

go_public void go_gfx_rotate(float angle)
{
    go_bgfx_rotate(angle);
}

go_public void go_gfx_layer(int layer)
{
    go_bgfx_layer(layer);
}
