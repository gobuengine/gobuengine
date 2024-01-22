#ifndef GB_ECS_ANIMATE_SPRITE_H
#define GB_ECS_ANIMATE_SPRITE_H

#include <stdio.h>
#include <stdint.h>
#include "ecs/flecs.h"
#include "binn/binn_json.h"
#include "gfx/gb_gfx.h"

#include "ecs/component/gb_type_texture.h"
#include "gobu.h"

#ifdef __cplusplus
extern "C" {
#endif

    // API: animate_sprite
    void gb_animate_sprite_stop(gb_animate_sprite_t* animated);
    void gb_animate_sprite_pause(gb_animate_sprite_t* animated);
    void gb_animate_sprite_play(gb_animate_sprite_t* animated);
    bool gb_animate_sprite_is_playing(gb_animate_sprite_t* animated);
    bool gb_animate_sprite_is_looping(gb_animate_sprite_t* animated);
    void gb_animate_sprite_set(gb_animate_sprite_t* animated, const char* name);

    // module import
    void gb_animate_sprite_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_ANIMATE_SPRITE_H
