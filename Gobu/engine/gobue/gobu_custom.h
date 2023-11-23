#ifndef __GOBU_CUSTOM_H__
#define __GOBU_CUSTOM_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_vec2_t
{
    float x;
    float y;
}ecs_vec2_t;

typedef struct ecs_vec3_t
{
    float x;
    float y;
    float z;
}ecs_vec3_t;

extern ECS_COMPONENT_DECLARE(ecs_vec2_t);
extern ECS_COMPONENT_DECLARE(ecs_vec3_t);

void GobuCustomImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
