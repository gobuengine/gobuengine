#ifndef __GOBU_CAMERA_H__
#define __GOBU_CAMERA_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CAMERA_NONE = 0,
    CAMERA_EDITOR,
    CAMERA_FOLLOWING,
}enumCameraMode;

typedef struct GCamera
{
    Camera2D camera;
    enumCameraMode mode;
}GCamera;

extern ECS_COMPONENT_DECLARE(GCamera);
extern ECS_COMPONENT_DECLARE(enumCameraMode);
extern ECS_COMPONENT_DECLARE(Camera2D);

void GobuCameraImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
