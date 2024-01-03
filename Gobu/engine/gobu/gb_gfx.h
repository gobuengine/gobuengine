#ifndef GB_GFX_H
#define GB_GFX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "gb_type_color.h"
#include "gb_type_transform.h"
#include "gb_type_bounding.h"
#include "gb_type_shape_rect.h"
#include "gb_type_shape_circle.h"
#include "gb_type_sprite.h"
#include "gb_type_text.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void gb_gfx_begin_lines(void);

    void gb_gfx_end();

    void gb_gfx_color4ub(gb_color_t color);

    void gb_gfx_vertex2f(float x, float y);

    void gb_gfx_vertex3f(float x, float y, float z);

    void gb_gfx_translate(float x, float y, float z);

    void gb_gfx_rotate(float angle, float x, float y, float z);

    void gb_gfx_scale(float x, float y, float z);

    void gb_gfx_push_matrix(void);

    void gb_gfx_pop_matrix(void);

    void gb_gfx_init();

    void gb_gfx_close();

    void gb_gfx_draw_grid_2d(int slices, float spacing, gb_color_t color);

    void gb_gfx_draw_gismos(gb_transform_t transform, gb_bounding_t bonding_box);

    void gb_gfx_draw_rect(gb_shape_rect_t rect);

    void gb_gfx_draw_circle(gb_shape_circle_t circle);

    void gb_gfx_draw_sprite(gb_sprite_t sprite);

    void gb_gfx_draw_text(gb_text_t text);

#ifdef __cplusplus
}
#endif

#endif // GB_GFX_H
