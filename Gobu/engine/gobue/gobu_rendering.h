#ifndef __GOBU_RENDERING_H__
#define __GOBU_RENDERING_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RenderPhases
{
    ecs_entity_t PreDraw;
    ecs_entity_t Background;
    ecs_entity_t Draw;
    ecs_entity_t PostDraw;
}RenderPhases;

typedef struct GWindow
{
    char *title;
    uint32_t width;
    uint32_t height;
    bool viewport;
    // RenderPhases phases;
}GWindow;

extern ECS_COMPONENT_DECLARE(GWindow);
extern ECS_COMPONENT_DECLARE(RenderPhases);

void GobuRenderingImport(ecs_world_t* world);
void gobu_rendering_init(ecs_world_t* world, int width, int height, const char *title, bool viewport);
void gobu_rendering_main(ecs_world_t *world);
void gobu_rendering_progress(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
