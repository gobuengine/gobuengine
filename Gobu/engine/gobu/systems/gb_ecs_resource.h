#ifndef GB_ECS_RESOURCE_H
#define GB_ECS_RESOURCE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "thirdparty/goburender/raylib.h"

#include "gobu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // API::Resource
    const char *gb_resource_key_normalize(const char *path);

    const char *gb_resource_set(ecs_world_t *world, const char *path);

    const gb_resource_t *gb_resource(ecs_world_t *world, const char *key);

    bool gb_resource_remove(ecs_world_t *world, const char *key);

    // module import
    void gb_resource_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_RESOURCE_H
