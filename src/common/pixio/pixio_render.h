#ifndef PIXIO_RENDER_H
#define PIXIO_RENDER_H

#include <stdio.h>
#include <stdint.h>
#include "pixio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct pixio_render_phases_t
    {
        ecs_entity_t PreDraw;
        ecs_entity_t Background;
        ecs_entity_t Draw;
        ecs_entity_t PostDraw;
        ecs_entity_t ClearDraw;
    } pixio_render_phases_t;

    typedef struct pixio_render_t
    {
        int id;
    } pixio_render_t;

    // global component
    extern ECS_COMPONENT_DECLARE(pixio_render_phases_t);
    extern ECS_COMPONENT_DECLARE(pixio_render_t);

    // API::Resource

    // module import
    void pixio_rendering_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // PIXIO_RENDER_H
