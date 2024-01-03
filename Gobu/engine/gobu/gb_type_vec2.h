#ifndef GB_TYPE_VEC2_H
#define GB_TYPE_VEC2_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"
#include "thirdparty/goburender/raymath.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef Vector2 gb_vec2_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_vec2_t);

    // module import
    void gb_type_vec2_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_VEC2_H
