#ifndef GB_ECS_TEXT_H
#define GB_ECS_TEXT_H

#include <stdio.h>
#include <stdint.h>
#include "ecs/flecs.h"
#include "binn/binn_json.h"
#include "gfx/gb_gfx.h"

#ifdef __cplusplus
extern "C" {
#endif

    // API

    // module import
    void gb_text_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_TEXT_H
