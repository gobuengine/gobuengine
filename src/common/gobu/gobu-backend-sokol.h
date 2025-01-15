#ifndef GOBU_BACKEND_SOKOL_H
#define GOBU_BACKEND_SOKOL_H
#include "gobu-type.h"

typedef struct go_bgfx_context go_bgfx_context;

go_public go_bgfx_context *go_bgfx_init(void);
go_public void go_bgfx_destroy(void);
go_public void go_bgfx_viewport_begin(void);
go_public void go_bgfx_viewport_end(void);
go_public void go_bgfx_viewport_color(uint8_t r, uint8_t g, uint8_t b);

go_public void go_bgfx_begin(int mode);
go_public void go_bgfx_end(void);
go_public void go_bgfx_vertex2f_t2f(float x, float y, float u, float v);
go_public void go_bgfx_vertex2f(float x, float y);
go_public void go_bgfx_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

go_public void go_bgfx_viewport(int x, int y, int width, int height);
go_public void go_bgfx_scissor(int x, int y, int width, int height);
go_public void go_bgfx_ortho(float l, float r, float b, float t, float n, float f);
go_public void go_bgfx_matrix_mode(int mode);

go_public void go_bgfx_enable_texture(void);
go_public void go_bgfx_disable_texture(void);
go_public go_texture_t go_bgfx_create_texture(unsigned char *data, int width, int height);
go_public void go_bgfx_destroy_texture(go_texture_t texture);
go_public void go_bgfx_texture_apply(go_texture_t texture);

go_public void go_bgfx_load_identity(void);

go_public void go_bgfx_push_transform(void);
go_public void go_bgfx_pop_transform(void);
go_public void go_bgfx_translate(float x, float y);
go_public void go_bgfx_scale(float x, float y);
go_public void go_bgfx_rotate(float angle);

go_public void go_bgfx_layer(int layer);

go_public void *go_bgfx_font_context(void);

#endif // GOBU_BACKEND_SOKOL_H

