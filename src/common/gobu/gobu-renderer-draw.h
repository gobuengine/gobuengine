#ifndef GOBU_RENDERER_DRAW_H
#define GOBU_RENDERER_DRAW_H

#include "gobu-type.h"

// shape drawing functions

go_public void go_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, go_color_t color, int layer_index);
go_public void go_draw_rect(float x, float y, float w, float h, go_color_t fill_color, go_color_t outline_color, float outline_thickness, int layer_index);
go_public void go_draw_line(float x0, float y0, float x1, float y1, float thickness, go_color_t color, int layer_index);
go_public void go_draw_circle(float x, float y, float radius, go_color_t fill_color, go_color_t outline_color, float outline_thickness, int layer_index);
go_public void go_draw_grid(int width, int height, int cell_size, go_color_t color, int layer_index);

// texture drawing functions

go_public void go_draw_texture_region(go_texture_t texture, go_rect_t src, go_rect_t dst, go_vec2_t scale, go_vec2_t origin, float angle, go_color_t tint, int layer_index);
go_public void go_draw_texture_ex(go_texture_t texture, go_rect_t src, go_vec2_t position, go_color_t tint, int layer_index);
go_public void go_draw_texture(go_texture_t texture, go_vec2_t position, go_color_t tint, int layer_index);

// text drawing functions

go_public void go_draw_text_ex(go_font_t font, const char *text, float font_size, go_vec2_t position, go_vec2_t scale, float rotation, go_vec2_t origin, go_color_t color, int layer_index);
go_public void go_draw_text(const char *text, float x, float y, float font_size, go_color_t color, int layer_index);

#endif // GOBU_RENDERER_DRAW_H