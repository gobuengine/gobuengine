#ifndef __GOBU_TEXT_H__
#define __GOBU_TEXT_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GText
{
    char *resource;
    char *text;
    float size;
    float spacing;
    Color color;
    Font font;
}GText;

extern ECS_COMPONENT_DECLARE(GText);

void GobuTextImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
