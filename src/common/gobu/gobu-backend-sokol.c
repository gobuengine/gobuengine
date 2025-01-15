#include "gobu-backend-sokol.h"

#define SOKOL_IMPL
#include "externs/sokol/sokol_gfx.h"
#include "externs/sokol/sokol_gl.h"
#include "externs/sokol/sokol_log.h"

#define FONTSTASH_IMPLEMENTATION
#include "gobu-text.h"
#include "externs/sokol/sokol_fontstash.h"

#include "gobu-gfx-internal.h"

struct go_bgfx_context
{
    sg_pass_action pass_action;
    sgl_pipeline pipeline;
    sgl_context context;
    FONScontext *fontctx;           // font context
    int layer;
};

go_public go_bgfx_context *go_bgfx_init(void)
{
    sg_setup(&(sg_desc){.logger.func = slog_func});
    sgl_setup(&(sgl_desc_t){.logger.func = slog_func});

    go_bgfx_context *context = malloc(sizeof(go_bgfx_context));

    context->pass_action = (sg_pass_action){
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {1.0f, 1.0f, 1.0f, 1.0f},
        },
    };

    context->pipeline = sgl_make_pipeline(&(sg_pipeline_desc){
        .depth.write_enabled = false,
        .colors[0].blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        },
    });

    context->context = sgl_make_context(&(sgl_context_desc_t){0});

    // font
    context->fontctx = sfons_create(&(sfons_desc_t){.width = 512, .height = 512});
    if (context->fontctx == NULL)
    {
        printf("Failed to create fontstash context\n");
        return NULL;
    }

    return context;
}

go_public void go_bgfx_destroy(void)
{
    sfons_destroy(go_gfxctx.fontctx);
    sgl_destroy_context(go_gfxctx.context);
    sgl_destroy_pipeline(go_gfxctx.pipeline);
    free(go_gfx_context());
}

go_public void go_bgfx_viewport_begin(void)
{
    sgl_set_context(go_gfxctx.context);
    sgl_defaults();
    sgl_load_pipeline(go_gfxctx.pipeline);
    sgl_ortho(0.0f, (float)go_gfx_viewport_width(), (float)go_gfx_viewport_height(), 0.0f, -1.0f, 1.0f);
}

go_public void go_bgfx_viewport_end(void)
{
    sfons_flush(go_fontctx);

    sg_begin_pass(&(sg_pass){
        .action = go_gfxctx.pass_action,
        .swapchain = {.width = go_gfx_viewport_width(), .height = go_gfx_viewport_height(), .gl.framebuffer = go_gfx_viewport_framebuffer()},
    });

    for (int i = 0; i < go_gfxctx.layer; i++)
        sgl_draw_layer(i);

    sgl_context_draw(go_gfxctx.context);
    sg_end_pass();
    sg_commit();

    // Reset layer
    go_gfxctx.layer = 1;
}

go_public void go_bgfx_viewport_color(uint8_t r, uint8_t g, uint8_t b)
{
    go_gfxctx.pass_action.colors[0].clear_value = (sg_color){r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
}

go_public void go_bgfx_begin(int mode)
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

go_public void go_bgfx_end(void)
{
    sgl_end();
}

go_public void go_bgfx_vertex2f_t2f(float x, float y, float u, float v)
{
    sgl_v2f_t2f(x, y, u, v);
}

go_public void go_bgfx_vertex2f(float x, float y)
{
    sgl_v2f(x, y);
}

go_public void go_bgfx_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    sgl_c4b(r, g, b, a);
}

go_public void go_bgfx_viewport(int x, int y, int width, int height)
{
    sgl_viewport(x, y, width, height, true);
}

go_public void go_bgfx_scissor(int x, int y, int width, int height)
{
    sgl_scissor_rect(x, y, width, height, true);
}

go_public void go_bgfx_ortho(float l, float r, float b, float t, float n, float f)
{
    sgl_ortho(l, r, b, t, n, f);
}

go_public void go_bgfx_matrix_mode(int mode)
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

go_public void go_bgfx_enable_texture(void)
{
    sgl_enable_texture();
}

go_public void go_bgfx_disable_texture(void)
{
    sgl_disable_texture();
}

go_public go_texture_t go_bgfx_create_texture(unsigned char *data, int width, int height)
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

    return (go_texture_t){.id = img_id.id, .width = width, .height = height, .sampler = sampler_id.id};
}

go_public void go_bgfx_destroy_texture(go_texture_t texture)
{
    sg_destroy_image((sg_image){texture.id});
    sg_destroy_sampler((sg_sampler){texture.sampler});
}

go_public void go_bgfx_texture_apply(go_texture_t texture)
{
    sgl_texture((sg_image){texture.id}, (sg_sampler){texture.sampler});
}

go_public void go_bgfx_load_identity(void)
{
    sgl_load_identity();
}

go_public void go_bgfx_push_transform(void)
{
    sgl_push_matrix();
}

go_public void go_bgfx_pop_transform(void)
{
    sgl_pop_matrix();
}

go_public void go_bgfx_translate(float x, float y)
{
    sgl_translate(x, y, 0.0f);
}

go_public void go_bgfx_scale(float x, float y)
{
    sgl_scale(x, y, 1.0f);
}

go_public void go_bgfx_rotate(float angle)
{
    sgl_rotate(sgl_rad(angle), 0.0f, 0.0f, 1.0f);
}

go_public void go_bgfx_layer(int layer)
{
    if (layer > go_gfxctx.layer)
        go_gfxctx.layer = layer;

    sgl_layer(layer);
}

go_public void *go_bgfx_font_context(void)
{
    return go_gfxctx.fontctx;
}

