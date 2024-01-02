#ifndef GB_ECS_RESOURCE_H
#define GB_ECS_RESOURCE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_resource.h"

#ifdef __cplusplus
extern "C" {
#endif

    // API::Bounding

    // module import
    void gb_bounding_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_RESOURCE_H
