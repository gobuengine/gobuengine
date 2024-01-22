#ifndef GB_TYPE_TRANSFORM_H
#define GB_TYPE_TRANSFORM_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "gb_gfx.h"
#include "gb_type_vec2.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct gb_transform_t
    {
        gb_vec2_t position;
        gb_vec2_t scale;
        float rotation;
        gb_vec2_t origin;
    } gb_transform_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_transform_t);

    // API::Transform
#define gb_ecs_transform(x, y)                                                                                       \
    (gb_transform_t)                                                                                                 \
    {                                                                                                                \
        .position = (gb_vec2_t){x, y}, .scale = (gb_vec2_t){1.0f, 1.0f}, .rotation = 0.0f, .origin = (gb_vec2_t) { 0.5f, 0.5f } \
    }

    // module import
    void gb_type_transform_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_TRANSFORM_H
