#ifndef __GOBU_GIZMOS_H__
#define __GOBU_GIZMOS_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_gizmos_t
{
    bool selected;
}ecs_gizmos_t;

extern ECS_COMPONENT_DECLARE(ecs_gizmos_t);

void GobuGizmosImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
