#ifndef __GOBU_BOUNDING_BOX_H__
#define __GOBU_BOUNDING_BOX_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "gobu_custom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GBoundingBox
{
    gb_vec2_t min;
    gb_vec2_t max;
}GBoundingBox;

extern ECS_COMPONENT_DECLARE(GBoundingBox);

void GobuBoundingBoxImport(ecs_world_t* world);
Rectangle GBoundingBox_ToRectangle(GBoundingBox box);

#ifdef __cplusplus
}
#endif

#endif
