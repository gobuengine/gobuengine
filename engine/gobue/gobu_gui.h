#ifndef __GOBU_CONTROLLER_H__
#define __GOBU_CONTROLLER_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"
#include "raygo/raymath.h"
// #include "raygo/raygui.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GUIWindow{
    bool movable;
    Vector2 position;
    void (*handle)(ecs_world_t* world, struct GUIWindow *window, float delta_time);
}GUIWindow;

extern ECS_COMPONENT_DECLARE(GUIWindow);

void GobuGuiImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
