#ifndef __GOBU_CONTROLLER_H__
#define __GOBU_CONTROLLER_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GPlayerPlatform{
    int i;
}GPlayerPlatform;

extern ECS_COMPONENT_DECLARE(GPlayerPlatform);

void GobuPlayerControllerImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
