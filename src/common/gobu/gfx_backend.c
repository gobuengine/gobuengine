#include "gobu.h"

#define SOKOL_IMPL
#include "externs/sokol/sokol_gfx.h"
#include "externs/sokol/sokol_gl.h"

#define SOKOL_LOG_IMPL
#include "externs/sokol/sokol_log.h"

static int gfxb__layer = 1;

typedef struct gfx_backend_t
{
    sg_pass_action pass_action;
    sgl_pipeline pipeline;
    sgl_context context;
    int width, height;
    int framebuffer_id;
} gfx_backend_t;

gfx_backend_t *gfxb_viewport_create(void)
{
    gfx_backend_t *gfx_backend = (gfx_backend_t *)malloc(sizeof(gfx_backend_t));
    if (!gfx_backend)
    {
        return NULL;
    }

    sg_setup(&(sg_desc){.logger.func = slog_func});
    sgl_setup(&(sgl_desc_t){.logger.func = slog_func});

    gfx_backend->pass_action = (sg_pass_action){
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {1.0f, 1.0f, 1.0f, 1.0f},
        },
    };

    gfx_backend->pipeline = sgl_make_pipeline(&(sg_pipeline_desc){
        .depth.write_enabled = false,
        .colors[0].blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        },
    });

    // gfx_backend->context = sgl_make_context(&(sgl_context_desc_t){0});

    gfx_backend->framebuffer_id = 0;

    return gfx_backend;
}

void gfxb_destroy(gfx_backend_t *gfx_backend)
{
    if (gfx_backend)
    {
        // sgl_destroy_context(gfx_backend->context);
        sgl_destroy_pipeline(gfx_backend->pipeline);
        free(gfx_backend);
    }
}

void gfxb_viewport_begin(gfx_backend_t *gfx_backend)
{
    // sgl_set_context(gfx_backend->context);
    sgl_defaults();
    sgl_load_pipeline(gfx_backend->pipeline);
    sgl_ortho(0.0f, (float)gfx_backend->width, (float)gfx_backend->height, 0.0f, -1.0f, 1.0f);
}

void gfxb_viewport_end(gfx_backend_t *gfx_backend)
{
    sg_begin_pass(&(sg_pass){
        .action = gfx_backend->pass_action,
        .swapchain = {.width = gfx_backend->width, .height = gfx_backend->height, .gl.framebuffer = gfx_backend->framebuffer_id},
    });

    for (int i = 0; i < gfxb__layer; i++)
        sgl_draw_layer(i);
    // sgl_context_draw(gfx_backend->context);

    sg_end_pass();
    sg_commit();

    // Reset layer
    gfxb__layer = 1;
}

void gfxb_viewport_resize(gfx_backend_t *gfx_backend, int width, int height)
{
    gfx_backend->width = width;
    gfx_backend->height = height;
}

void gfxb_viewport_framebuffer(gfx_backend_t *gfx_backend, int framebuffer_id)
{
    gfx_backend->framebuffer_id = framebuffer_id;
}

void gfxb_viewport_color(gfx_backend_t *gfx_backend, uint8_t r, uint8_t g, uint8_t b)
{
    gfx_backend->pass_action.colors[0].clear_value = (sg_color){r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
}

void gfxb_begin(int mode)
{
    switch (mode)
    {
    case GFXB_POINTS:
        sgl_begin_points();
        break;
    case GFXB_LINES:
        sgl_begin_lines();
        break;
    case GFXB_TRIANGLES:
        sgl_begin_triangles();
        break;
    case GFXB_QUADS:
        sgl_begin_quads();
        break;
    default:
        break;
    }
}

void gfxb_end(void)
{
    sgl_end();
}

void gfxb_vertex2f_t2f(float x, float y, float u, float v)
{
    sgl_v2f_t2f(x, y, u, v);
}

void gfxb_vertex2f(float x, float y)
{
    sgl_v2f(x, y);
}

void gfxb_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    sgl_c4b(r, g, b, a);
}

void gfxb_viewport(int x, int y, int width, int height)
{
    sgl_viewport(x, y, width, height, true);
}

void gfxb_scissor(int x, int y, int width, int height)
{
    sgl_scissor_rect(x, y, width, height, true);
}

void gfxb_ortho(float l, float r, float b, float t, float n, float f)
{
    sgl_ortho(l, r, b, t, n, f);
}

void gfxb_matrix_mode(int mode)
{
    switch (mode)
    {
    case GFXB_MODELVIEW:
        sgl_matrix_mode_modelview();
        break;
    case GFXB_PROJECTION:
        sgl_matrix_mode_projection();
        break;
    case GFXB_TEXTURE:
        sgl_enable_texture();
        break;
    default:
        break;
    }
}

void gfxb_enable_texture(void)
{
    sgl_enable_texture();
}

void gfxb_disable_texture(void)
{
    sgl_disable_texture();
}

gb_texture_t gfxb_create_texture(unsigned char *data, int width, int height)
{
    sg_image img_id = sg_make_image(&(sg_image_desc){
        .width = width,
        .height = height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data.subimage[0][0] = {
            .ptr = data,
            .size = (size_t)(width * height * 4),
        },
    });

    sg_sampler sampler_id = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_REPEAT,
        .wrap_v = SG_WRAP_REPEAT,
    });

    return (gb_texture_t){.id = img_id.id, .width = width, .height = height, .sampler = sampler_id.id};
}

void gfxb_destroy_texture(gb_texture_t texture)
{
    sg_destroy_image((sg_image){texture.id});
    sg_destroy_sampler((sg_sampler){texture.sampler});
}

void gfxb_texture_apply(gb_texture_t texture)
{
    sgl_texture((sg_image){texture.id}, (sg_sampler){texture.sampler});
}

void gfxb_load_identity(void)
{
    sgl_load_identity();
}

void gfxb_push_transform(void)
{
    sgl_push_matrix();
}

void gfxb_pop_transform(void)
{
    sgl_pop_matrix();
}

void gfxb_translate(float x, float y)
{
    sgl_translate(x, y, 0.0f);
}

void gfxb_scale(float x, float y)
{
    sgl_scale(x, y, 1.0f);
}

void gfxb_rotate(float angle)
{
    sgl_rotate(sgl_rad(angle), 0.0f, 0.0f, 1.0f);
}

void gfxb_layer(int layer)
{
    // La idea es que la variable se incremente si layer es mayor que gfxb__layer.
    if (layer > gfxb__layer)
        gfxb__layer = layer;
    sgl_layer(layer);
}
