#ifndef GB_TYPE_SPRITE_H
#define GB_TYPE_SPRITE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "gb_gfx.h"

#include "gb_type_texture.h"
#include "gb_type_rect.h"
#include "gb_type_color.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_sprite_t
    {
        char* resource;
        gb_color_t tint;
        gb_texture_t texture;
        gb_rect_t src;
        gb_rect_t dst;
    }gb_sprite_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_sprite_t);

    // module import
    void gb_type_sprite_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_SPRITE_H
