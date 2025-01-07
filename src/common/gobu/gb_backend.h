#ifndef GOBU_BACKEND_H
#define GOBU_BACKEND_H
#include "gb_type.h"

gfx_backend_t *gfxb_viewport_create(void);
void gfxb_destroy(gfx_backend_t *gfx_backend);
void gfxb_viewport_begin(gfx_backend_t *gfx_backend);
void gfxb_viewport_end(gfx_backend_t *gfx_backend);
void gfxb_viewport_resize(gfx_backend_t *gfx_backend, int width, int height);
void gfxb_viewport_color(gfx_backend_t *gfx_backend, uint8_t r, uint8_t g, uint8_t b);
void gfxb_viewport_framebuffer(gfx_backend_t *gfx_backend, int framebuffer_id);
void gfxb_begin(int mode);
void gfxb_end(void);
void gfxb_vertex2f_t2f(float x, float y, float u, float v);
void gfxb_vertex2f(float x, float y);
void gfxb_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void gfxb_matrix_mode(int mode);
void gfxb_enable_texture(void);
void gfxb_disable_texture(void);
gb_texture_t gfxb_create_texture(unsigned char *data, int width, int height);
void gfxb_destroy_texture(gb_texture_t texture);
void gfxb_texture_apply(gb_texture_t texture);
void gfxb_load_identity(void);
void gfxb_viewport(int x, int y, int width, int height);
void gfxb_scissor(int x, int y, int width, int height);
void gfxb_ortho(float l, float r, float b, float t, float n, float f);
void gfxb_push_transform(void);
void gfxb_pop_transform(void);
void gfxb_translate(float x, float y);
void gfxb_scale(float x, float y);
void gfxb_rotate(float angle);
void gfxb_layer(int layer);

#endif // GOBU_BACKEND_H
