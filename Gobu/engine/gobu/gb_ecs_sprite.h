#ifndef GB_ECS_SPRITE_H
#define GB_ECS_SPRITE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

    // API::SPRITE
    gb_sprite_t gb_sprite_deserialize(binn* fsprite);
    binn* gb_sprite_serialize(gb_sprite_t sprite);
    gb_sprite_t gb_sprite_init(void);

    // module import
    void gb_sprite_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_SPRITE_H
