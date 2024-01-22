#ifndef GB_ECS_CAMERA_H
#define GB_ECS_CAMERA_H

#include <stdio.h>
#include <stdint.h>
#include "ecs/flecs.h"
#include "binn/binn_json.h"
#include "gfx/gb_gfx.h"

#include "gobu.h"

#ifdef __cplusplus
extern "C" {
#endif

    // API::Camera

    // module import
    void gb_camera_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_CAMERA_H
