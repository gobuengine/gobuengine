#ifndef __GOBU_COLLISION_H__
#define __GOBU_COLLISION_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GCollisionBox{
    bool is_trigger;
    Vector2 offset;
    Vector2 size;
}GCollisionBox;

typedef struct GCollisionCircle{
    bool is_trigger;
    Vector2 offset;
    float radius;
}GCollisionCircle;

extern ECS_COMPONENT_DECLARE(GCollisionBox);
extern ECS_COMPONENT_DECLARE(GCollisionCircle);

void GobuCollisionImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
