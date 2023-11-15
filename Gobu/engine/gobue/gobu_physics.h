#ifndef __GOBU_PHYSICS_H__
#define __GOBU_PHYSICS_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"
#include "physac/physac.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GPhysics{
    bool enabled;
    bool freezeOrient;
    float restitution;
    float density;
    // no-serialize
    PhysicsBody body;
}GPhysics;

extern ECS_COMPONENT_DECLARE(GPhysics);

void GobuPhysicsImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
