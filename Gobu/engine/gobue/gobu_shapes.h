#ifndef __GOBU_SHAPES_H__
#define __GOBU_SHAPES_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GShapeRec
{
    float x;
    float y;
    float width;
    float height;
    Color color;
}GShapeRec;

extern ECS_COMPONENT_DECLARE(GShapeRec);

void GobuShapesImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
