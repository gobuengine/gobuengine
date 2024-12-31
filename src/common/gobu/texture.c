#include "gobu.h"

#define STB_IMAGE_IMPLEMENTATION
#include "externs/stb_image.h"

gobu_texture_t gobu_load_texture(const char *filename)
{
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        printf("Failed to load texture: %s", filename);
        return (gobu_texture_t){0};
    }

    gobu_texture_t texture = gfxb_create_texture(data, width, height);

    stbi_image_free(data);

    return texture;
}

bool gobu_texture_is_valid(gobu_texture_t texture)
{
    return texture.id != 0;
}

void gobu_free_texture(gobu_texture_t texture)
{
    gfxb_destroy_texture(texture);
}

void gobu_draw_texture_pro(gobu_texture_t texture, gobu_rect_t src, gobu_rect_t dst, gobu_vec2_t scale, gobu_vec2_t origin, float angle, gobu_color_t tint, int layer_index)
{
    src.w = (src.w == 0) ? texture.width : src.w;
    src.h = (src.h == 0) ? texture.height : src.h;

    dst.w = (dst.w == 0) ? src.w : dst.w;
    dst.h = (dst.h == 0) ? src.h : dst.h;

    gfxb_layer(layer_index);

    gfxb_enable_texture();
    gfxb_texture_apply(texture);

    float tex_left = src.x / (float)texture.width;
    float tex_top = src.y / (float)texture.height;
    float tex_right = (src.x + src.w) / (float)texture.width;
    float tex_bottom = (src.y + src.h) / (float)texture.height;

    gfxb_push_transform();
    gfxb_translate(dst.x, dst.y);
    gfxb_scale(scale.x, scale.y);
    gfxb_rotate(angle);
    gfxb_translate(-origin.x, -origin.y);
    gfxb_begin(GFXB_QUADS);
    gfxb_color4b(tint.r, tint.g, tint.b, tint.a);
    {
        gfxb_vertex2f_t2f(0.0f, 0.0f, tex_left, tex_top);
        gfxb_vertex2f_t2f(0.0f, dst.h, tex_left, tex_bottom);
        gfxb_vertex2f_t2f(dst.w, dst.h, tex_right, tex_bottom);
        gfxb_vertex2f_t2f(dst.w, 0.0f, tex_right, tex_top);
    }
    gfxb_end();
    gfxb_pop_transform();
    gfxb_disable_texture();
}

void gobu_draw_texture_rect(gobu_texture_t texture, gobu_rect_t src, gobu_vec2_t position, gobu_color_t tint, int layer_index)
{
    gobu_rect_t dst = {position.x, position.y, fabsf(src.w), fabsf(src.h)};
    gobu_vec2_t scale = {1.0f, 1.0f};
    gobu_vec2_t origin = {0.0f, 0.0f};
    gobu_draw_texture_pro(texture, src, dst, scale, origin, 0.0f, tint, layer_index);
}

void gobu_draw_texture(gobu_texture_t texture, gobu_vec2_t position, gobu_color_t tint, int layer_index)
{
    gobu_rect_t src = {0.0f, 0.0f, texture.width, texture.height};
    gobu_draw_texture_rect(texture, src, position, tint, layer_index);
}

