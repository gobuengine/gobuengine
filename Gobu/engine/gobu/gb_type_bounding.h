#ifndef GB_TYPE_BOUNDING_H
#define GB_TYPE_BOUNDING_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_bounding_t
    {
        gb_vec2_t min;
        gb_vec2_t max;
        gb_vec2_t size;
        gb_vec2_t center;
        gb_vec2_t extents;
    }gb_bounding_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_bounding_t);

    // module import
    void gb_type_bounding_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_BOUNDING_H
