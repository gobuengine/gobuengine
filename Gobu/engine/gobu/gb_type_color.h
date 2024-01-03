#ifndef GB_TYPE_COLOR_H
#define GB_TYPE_COLOR_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef Color gb_color_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_color_t);

    // module import
    void gb_type_color_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_COLOR_H
