#include "gobu.h"
#include <math.h>

void gobu_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, gb_color_t color, int layer_index)
{
    gfxb_layer(layer_index);
    gfxb_begin(GFXB_TRIANGLES);
    {
        gfxb_color4b(color.r, color.g, color.b, color.a);
        gfxb_vertex2f(x0, y0);
        gfxb_vertex2f(x1, y1);
        gfxb_vertex2f(x2, y2);
    }
    gfxb_end();
}


void gobu_draw_rect(float x, float y, float w, float h, gb_color_t fill_color, gb_color_t outline_color, float outline_thickness, int layer_index)
{
    gfxb_layer(layer_index);

    if (fill_color.a > 0)
    {
        gfxb_begin(GFXB_QUADS);
        {
            gfxb_color4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            gfxb_vertex2f(x, y);
            gfxb_vertex2f(x + w, y);
            gfxb_vertex2f(x + w, y + h);
            gfxb_vertex2f(x, y + h);
        }
        gfxb_end();
    }

    if (outline_thickness > 0)
    {
        float spacing_clip = outline_thickness / 2.0f;
        gobu_draw_line(x - spacing_clip, y, x + w + spacing_clip, y, outline_thickness, outline_color, layer_index);
        gobu_draw_line(x + w, y, x + w, y + h, outline_thickness, outline_color, layer_index);
        gobu_draw_line(x + w + spacing_clip, y + h, x - spacing_clip, y + h, outline_thickness, outline_color, layer_index);
        gobu_draw_line(x, y + h, x, y, outline_thickness, outline_color, layer_index);
    }
}

void gobu_draw_line(float x0, float y0, float x1, float y1, float thickness, gb_color_t color, int layer_index)
{
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);

    float nx = -dy / length * (thickness / 2.0f);
    float ny = dx / length * (thickness / 2.0f);

    gfxb_layer(layer_index);
    gfxb_begin(GFXB_TRIANGLES);
    {
        gfxb_color4b(color.r, color.g, color.b, color.a);
        gfxb_vertex2f(x0 + nx, y0 + ny);
        gfxb_vertex2f(x0 - nx, y0 - ny);
        gfxb_vertex2f(x1 + nx, y1 + ny);

        gfxb_vertex2f(x0 - nx, y0 - ny);
        gfxb_vertex2f(x1 - nx, y1 - ny);
        gfxb_vertex2f(x1 + nx, y1 + ny);
    }
    gfxb_end();
}

void bdr_draw_circle(float x, float y, float radius, gb_color_t fill_color, gb_color_t outline_color, float outline_thickness, int layer_index)
{
    const int segments = 64;
    gfxb_layer(layer_index);

    if (outline_thickness <= 0)
    {
        gfxb_begin(GFXB_QUADS);
        gfxb_color4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);

        for (int i = 0; i < segments; i++)
        {
            float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
            float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

            gfxb_vertex2f(x, y);
            gfxb_vertex2f(x + cosf(a0) * radius, y + sinf(a0) * radius);
            gfxb_vertex2f(x + cosf(a1) * radius, y + sinf(a1) * radius);
        }
        gfxb_end();
        return;
    }

    gfxb_begin(GFXB_QUADS);
    {
        gfxb_color4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        for (int i = 0; i < segments; i++)
        {
            float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
            float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

            gfxb_vertex2f(x, y);
            gfxb_vertex2f(x + cosf(a0) * (radius - outline_thickness), y + sinf(a0) * (radius - outline_thickness));
            gfxb_vertex2f(x + cosf(a1) * (radius - outline_thickness), y + sinf(a1) * (radius - outline_thickness));
        }
    }
    gfxb_end();

    gfxb_begin(GFXB_QUADS);
    {
        gfxb_color4b(outline_color.r, outline_color.g, outline_color.b, outline_color.a);

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
            gfxb_vertex2f(x0_outer, y0_outer);
            gfxb_vertex2f(x0_inner, y0_inner);
            gfxb_vertex2f(x1_outer, y1_outer);

            gfxb_vertex2f(x1_outer, y1_outer);
            gfxb_vertex2f(x0_inner, y0_inner);
            gfxb_vertex2f(x1_inner, y1_inner);
        }
    }
    gfxb_end();
}

void gobu_draw_grid(int width, int height, int cell_size, gb_color_t color, int layer_index)
{
    gfxb_layer(layer_index);
    gfxb_begin(GFXB_LINES);
    {
        gfxb_color4b(color.r, color.g, color.b, color.a);

        for (int x = 0; x <= width; x += cell_size)
        {
            gfxb_vertex2f((float)x, 0.0f);
            gfxb_vertex2f((float)x, (float)height);
        }

        for (int y = 0; y <= height; y += cell_size)
        {
            gfxb_vertex2f(0.0f, (float)y);
            gfxb_vertex2f((float)width, (float)y);
        }
    }
    gfxb_end();
}
