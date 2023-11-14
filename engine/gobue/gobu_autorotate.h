#ifndef __GOBU_AUTOROTATE_H__
#define __GOBU_AUTOROTATE_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GAutoRotate
{
    float speed;
}GAutoRotate;

extern ECS_COMPONENT_DECLARE(GAutoRotate);

void GobuAutoRotateImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
