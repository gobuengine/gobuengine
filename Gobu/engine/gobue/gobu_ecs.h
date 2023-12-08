
#ifndef __GOBU_ECS_H__
#define __GOBU_ECS_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

ecs_entity_t gobu_ecs_new_entity(ecs_world_t* world, const char* name);

#ifdef __cplusplus
}
#endif

#endif
