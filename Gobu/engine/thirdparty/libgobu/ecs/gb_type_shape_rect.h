#ifndef GB_TYPE_SHAPE_RECT_H
#define GB_TYPE_SHAPE_RECT_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "gb_gfx.h"
#include "gb_type_color.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_shape_rect_t
    {
        float x;
        float y;
        float width;
        float height;
        gb_color_t color;
        int border_width;
        gb_color_t border_color;
        int border_radius_top_left;
        int border_radius_top_right;
        int border_radius_bottom_left;
        int border_radius_bottom_right;
    }gb_shape_rect_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_shape_rect_t);

    // module import
    void gb_type_shape_rect_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_SHAPE_RECT_H
