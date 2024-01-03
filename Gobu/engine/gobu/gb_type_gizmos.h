#ifndef GB_TYPE_GIZMOS_H
#define GB_TYPE_GIZMOS_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"
#include "thirdparty/goburender/raymath.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_gizmos_t
    {
        bool selected;
    }gb_gizmos_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_gizmos_t);

    // module import
    void gb_type_gizmos_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_GIZMOS_H
