#include "gobu-renderer-draw.h"
#include "gobu-gfx-internal.h"
#include "gobu-util.h"

// shape drawing functions

go_public void go_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, go_color_t color, int layer_index)
{
    go_gfx_layer(layer_index);
    go_gfx_begin(GFXB_TRIANGLES);
    {
        go_gfx_color4b(color.r, color.g, color.b, color.a);
        go_gfx_vertex2f(x0, y0);
        go_gfx_vertex2f(x1, y1);
        go_gfx_vertex2f(x2, y2);
    }
    go_gfx_end();
}

go_public void go_draw_rect(float x, float y, float w, float h, go_color_t fill_color, go_color_t outline_color, float outline_thickness, int layer_index)
{
    go_gfx_layer(layer_index);

    if (fill_color.a > 0)
    {
        go_gfx_begin(GFXB_QUADS);
        {
            go_gfx_color4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            go_gfx_vertex2f(x, y);
            go_gfx_vertex2f(x + w, y);
            go_gfx_vertex2f(x + w, y + h);
            go_gfx_vertex2f(x, y + h);
        }
        go_gfx_end();
    }

    if (outline_thickness > 0)
    {
        float spacing_clip = outline_thickness / 2.0f;
        go_draw_line(x - spacing_clip, y, x + w + spacing_clip, y, outline_thickness, outline_color, layer_index);
        go_draw_line(x + w, y, x + w, y + h, outline_thickness, outline_color, layer_index);
        go_draw_line(x + w + spacing_clip, y + h, x - spacing_clip, y + h, outline_thickness, outline_color, layer_index);
        go_draw_line(x, y + h, x, y, outline_thickness, outline_color, layer_index);
    }
}

go_public void go_draw_line(float x0, float y0, float x1, float y1, float thickness, go_color_t color, int layer_index)
{
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);

    float nx = -dy / length * (thickness / 2.0f);
    float ny = dx / length * (thickness / 2.0f);

    go_gfx_layer(layer_index);
    go_gfx_begin(GFXB_TRIANGLES);
    {
        go_gfx_color4b(color.r, color.g, color.b, color.a);
        go_gfx_vertex2f(x0 + nx, y0 + ny);
        go_gfx_vertex2f(x0 - nx, y0 - ny);
        go_gfx_vertex2f(x1 + nx, y1 + ny);

        go_gfx_vertex2f(x0 - nx, y0 - ny);
        go_gfx_vertex2f(x1 - nx, y1 - ny);
        go_gfx_vertex2f(x1 + nx, y1 + ny);
    }
    go_gfx_end();
}

go_public void go_draw_circle(float x, float y, float radius, go_color_t fill_color, go_color_t outline_color, float outline_thickness, int layer_index)
{
    const int segments = 64;
    go_gfx_layer(layer_index);

    if (outline_thickness <= 0)
    {
        go_gfx_begin(GFXB_QUADS);
        go_gfx_color4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);

        for (int i = 0; i < segments; i++)
        {
            float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
            float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

            go_gfx_vertex2f(x, y);
            go_gfx_vertex2f(x + cosf(a0) * radius, y + sinf(a0) * radius);
            go_gfx_vertex2f(x + cosf(a1) * radius, y + sinf(a1) * radius);
        }
        go_gfx_end();
        return;
    }

    go_gfx_begin(GFXB_QUADS);
    {
        go_gfx_color4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        for (int i = 0; i < segments; i++)
        {
            float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
            float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

            go_gfx_vertex2f(x, y);
            go_gfx_vertex2f(x + cosf(a0) * (radius - outline_thickness), y + sinf(a0) * (radius - outline_thickness));
            go_gfx_vertex2f(x + cosf(a1) * (radius - outline_thickness), y + sinf(a1) * (radius - outline_thickness));
        }
    }
    go_gfx_end();

    go_gfx_begin(GFXB_QUADS);
    {
        go_gfx_color4b(outline_color.r, outline_color.g, outline_color.b, outline_color.a);

        for (int i = 0; i < segments; i++)
        {
            float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
            float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

            // Outer circle points
            float x0_outer = x + cosf(a0) * radius;
            float y0_outer = y + sinf(a0) * radius;
            float x1_outer = x + cosf(a1) * radius;
            float y1_outer = y + sinf(a1) * radius;

            // Inner circle points
            float x0_inner = x + cosf(a0) * (radius - outline_thickness);
            float y0_inner = y + sinf(a0) * (radius - outline_thickness);
            float x1_inner = x + cosf(a1) * (radius - outline_thickness);
            float y1_inner = y + sinf(a1) * (radius - outline_thickness);

            // Draw outline triangles
            go_gfx_vertex2f(x0_outer, y0_outer);
            go_gfx_vertex2f(x0_inner, y0_inner);
            go_gfx_vertex2f(x1_outer, y1_outer);

            go_gfx_vertex2f(x1_outer, y1_outer);
            go_gfx_vertex2f(x0_inner, y0_inner);
            go_gfx_vertex2f(x1_inner, y1_inner);
        }
    }
    go_gfx_end();
}

go_public void go_draw_grid(int width, int height, int cell_size, go_color_t color, int layer_index)
{
    go_gfx_layer(layer_index);
    go_gfx_begin(GFXB_LINES);
    {
        go_gfx_color4b(color.r, color.g, color.b, color.a);

        for (int x = 0; x <= width; x += cell_size)
        {
            go_gfx_vertex2f((float)x, 0.0f);
            go_gfx_vertex2f((float)x, (float)height);
        }

        for (int y = 0; y <= height; y += cell_size)
        {
            go_gfx_vertex2f(0.0f, (float)y);
            go_gfx_vertex2f((float)width, (float)y);
        }
    }
    go_gfx_end();
}

// texture drawing functions

go_public void go_draw_texture_region(go_texture_t texture, go_rect_t src, go_rect_t dst, go_vec2_t scale, go_vec2_t origin, float angle, go_color_t tint, int layer_index)
{
    src.w = (src.w == 0) ? texture.width : src.w;
    src.h = (src.h == 0) ? texture.height : src.h;

    dst.w = (dst.w == 0) ? src.w : dst.w;
    dst.h = (dst.h == 0) ? src.h : dst.h;

    go_gfx_layer(layer_index);

    go_gfx_enable_texture();
    go_gfx_texture_apply(texture);

    float tex_left = src.x / (float)texture.width;
    float tex_top = src.y / (float)texture.height;
    float tex_right = (src.x + src.w) / (float)texture.width;
    float tex_bottom = (src.y + src.h) / (float)texture.height;

    go_gfx_push_transform();
    go_gfx_translate(dst.x, dst.y);
    go_gfx_scale(scale.x, scale.y);
    go_gfx_rotate(angle);
    go_gfx_translate(-origin.x, -origin.y);
    go_gfx_begin(GFXB_QUADS);
    go_gfx_color4b(tint.r, tint.g, tint.b, tint.a);
    {
        go_gfx_vertex2f_t2f(0.0f, 0.0f, tex_left, tex_top);
        go_gfx_vertex2f_t2f(0.0f, dst.h, tex_left, tex_bottom);
        go_gfx_vertex2f_t2f(dst.w, dst.h, tex_right, tex_bottom);
        go_gfx_vertex2f_t2f(dst.w, 0.0f, tex_right, tex_top);
    }
    go_gfx_end();
    go_gfx_pop_transform();
    go_gfx_disable_texture();
}

go_public void go_draw_texture_ex(go_texture_t texture, go_rect_t src, go_vec2_t position, go_color_t tint, int layer_index)
{
    go_rect_t dst = {position.x, position.y, fabsf(src.w), fabsf(src.h)};
    go_vec2_t scale = {1.0f, 1.0f};
    go_vec2_t origin = {0.0f, 0.0f};
    go_draw_texture_region(texture, src, dst, scale, origin, 0.0f, tint, layer_index);
}

go_public void go_draw_texture(go_texture_t texture, go_vec2_t position, go_color_t tint, int layer_index)
{
    go_rect_t src = {0.0f, 0.0f, texture.width, texture.height};
    go_draw_texture_ex(texture, src, position, tint, layer_index);
}

// text drawing functions

go_public void go_draw_text_ex(go_font_t font, const char *text, float font_size, go_vec2_t position, go_vec2_t scale, float rotation, go_vec2_t origin, go_color_t color, int layer_index)
{
    go_gfx_layer(layer_index);
    go_font_draw_begin(font, font_size, color);
    {
        go_gfx_push_transform();
        {
            go_gfx_translate(position.x, position.y);
            go_gfx_scale(scale.x, scale.y);
            go_gfx_rotate(rotation);
            go_gfx_translate(-(go_font_text_width(text) * origin.x), -origin.y);
            {
                go_font_draw_end(text);
            }
        }
        go_gfx_pop_transform();
    }
}

go_public void go_draw_text(const char *text, float x, float y, float font_size, go_color_t color, int layer_index)
{
    go_draw_text_ex(go_font_get_default(), text, font_size, (go_vec2_t){x, y}, (go_vec2_t){1.0f, 1.0f}, 0.0f, (go_vec2_t){0.0f, 0.0f}, color, layer_index);
}
