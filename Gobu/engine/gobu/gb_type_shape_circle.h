#ifndef GB_TYPE_SHAPE_CIRCLE_H
#define GB_TYPE_SHAPE_CIRCLE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_color.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_shape_circle_t
    {
        float x;
        float y;
        float radius;
        gb_color_t background_color;
    }gb_shape_circle_t;


    // global component
    extern ECS_COMPONENT_DECLARE(gb_shape_circle_t);

    // module import
    void gb_type_shape_circle_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_SHAPE_CIRCLE_H
