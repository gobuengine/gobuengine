#ifndef GB_ECS_RENDERING_H
#define GB_ECS_RENDERING_H

#include <stdio.h>
#include <stdint.h>
#include "ecs/flecs.h"
#include "binn/binn_json.h"
#include "gfx/gb_gfx.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct gb_render_phases_t
    {
        ecs_entity_t PreDraw;
        ecs_entity_t Background;
        ecs_entity_t Draw;
        ecs_entity_t PostDraw;
        ecs_entity_t ClearDraw;
    } gb_render_phases_t;

    typedef struct gb_app_t
    {
        char *title;
        uint32_t width;
        uint32_t height;
        bool no_window;
        // RenderPhases phases;
        int fps;
        bool show_fps;
        bool show_grid;
    } gb_app_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_render_phases_t);
    extern ECS_COMPONENT_DECLARE(gb_app_t);

    // API::Resource

    // module import
    void gb_rendering_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_RENDERING_H
