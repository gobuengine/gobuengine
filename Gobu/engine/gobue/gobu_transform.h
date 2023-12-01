#ifndef __GOBU_TRANSFORM_H__
#define __GOBU_TRANSFORM_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GPosition
{
    float x;
    float y;
    float _x;
    float _y;
}GPosition;

typedef struct GScale
{
    float x;
    float y;
}GScale;

typedef struct GRotation
{
    float x;
    float y;
}GRotation;

extern ECS_COMPONENT_DECLARE(GPosition);
extern ECS_COMPONENT_DECLARE(GScale);
extern ECS_COMPONENT_DECLARE(GRotation);

void GobuTransformImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
