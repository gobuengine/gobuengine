#ifndef GB_ECS_ANIMATE_SPRITE_H
#define GB_ECS_ANIMATE_SPRITE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_animate_sprite.h"

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
