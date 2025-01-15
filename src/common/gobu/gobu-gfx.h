#ifndef GOBU_GFX_H
#define GOBU_GFX_H

#include "gobu-type.h"
#include "gobu-text.h"

#if !defined(SOKOL_BACKEND)
#include "gobu-backend-sokol.h"
#endif

typedef struct go_gfx_context_t
{
    int width, height;
    int framebuffer_id;
    go_font_t font_default;
    go_bgfx_context *bctx;          // backend context
}go_gfx_context_t;

go_bgfx_context *go_gfx_context(void);
go_font_t go_gfx_font_default(void);
void *go_gfx_font_context(void);

go_public void go_gfx_init(go_gfx_context_t *ctx, int width, int height);
go_public void go_gfx_shutdown(void);

go_public int go_gfx_viewport_width(void);
go_public int go_gfx_viewport_height(void);
go_public void go_gfx_viewport_set_resize(int width, int height);
go_public void go_gfx_set_viewport(int width, int height);

go_public void go_gfx_viewport_begin(void);
go_public void go_gfx_viewport_end(void);
go_public void go_gfx_viewport_color(uint8_t r, uint8_t g, uint8_t b);

go_public void go_gfx_viewport_set_framebuffer(int framebuffer_id);
go_public int go_gfx_viewport_framebuffer(void);

go_public void go_gfx_begin(int mode);
go_public void go_gfx_end(void);
go_public void go_gfx_vertex2f_t2f(float x, float y, float u, float v);
go_public void go_gfx_vertex2f(float x, float y);
go_public void go_gfx_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

go_public void go_gfx_viewport(int x, int y, int width, int height);
go_public void go_gfx_scissor(int x, int y, int width, int height);
go_public void go_gfx_ortho(float l, float r, float b, float t, float n, float f);

go_public void go_gfx_matrix_mode(int mode);
go_public void go_gfx_load_identity(void);

go_public void go_gfx_enable_texture(void);
go_public void go_gfx_disable_texture(void);
go_public go_texture_t go_gfx_create_texture(unsigned char *data, int width, int height);
go_public void go_gfx_destroy_texture(go_texture_t texture);
go_public void go_gfx_texture_apply(go_texture_t texture);

go_public void go_gfx_push_transform(void);
go_public void go_gfx_pop_transform(void);
go_public void go_gfx_translate(float x, float y);
go_public void go_gfx_scale(float x, float y);
go_public void go_gfx_rotate(float angle);

go_public void go_gfx_layer(int layer);

#endif // GOBU_GFX_H

