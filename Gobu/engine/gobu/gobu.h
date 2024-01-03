#ifndef GB_ENGINE_H
#define GB_ENGINE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "gb_type_camera.h"
#include "gb_type_vec2.h"
#include "gb_ecs_rendering.h"

#ifdef __cplusplus
extern "C"
{
#endif

    ecs_world_t *gb_init(gb_app_t *app);

    void gb_app_main(ecs_world_t *world);

    void gb_app_progress(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif
