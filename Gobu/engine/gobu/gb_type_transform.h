#ifndef GB_TYPE_TRANSFORM_H
#define GB_TYPE_TRANSFORM_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_transform_t
    {
        gb_vec2_t position;
        gb_vec2_t scale;
        float rotation;
        gb_vec2_t origin;
    }gb_transform_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_transform_t);

    // module import
    void gb_type_transform_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_TRANSFORM_H
