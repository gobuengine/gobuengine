#ifndef __GOBU_RESOURCE_H__
#define __GOBU_RESOURCE_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"
#include "binn/binn_json.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gb_resource{
    char *path;
    // no serialize
    Texture2D texture;
    Font font;
    binn *anim2d;
}gb_resource;

extern ECS_COMPONENT_DECLARE(gb_resource);

void GobuResourceImport(ecs_world_t* world);

bool gobu_resource_set(ecs_world_t* world, const char* key, const char* path);
const gb_resource* gobu_resource(ecs_world_t* world, const char* key);

#ifdef __cplusplus
}
#endif

#endif
