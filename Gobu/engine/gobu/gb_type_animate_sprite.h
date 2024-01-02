#ifndef GB_TYPE_ANIMATE_SPRITE_H
#define GB_TYPE_ANIMATE_SPRITE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_animate_frame_t
    {
        int index_;
        int duration;
        gb_sprite_t sprite;
    }gb_animate_frame_t;

    typedef struct gb_animate_animation_t
    {
        ecs_string_t name;
        ecs_i16_t fps;
        ecs_bool_t loop;
        ecs_vec_t frames;
    }gb_animate_animation_t;

    typedef struct gb_animate_sprite_t
    {
        int current;
        int current_frame;
        float counter;
        ecs_bool_t is_playing;
        ecs_string_t resource;
        ecs_string_t animation;
        ecs_vec_t animations;
    }gb_animate_sprite_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_animate_frame_t);
    extern ECS_COMPONENT_DECLARE(gb_animate_animation_t);
    extern ECS_COMPONENT_DECLARE(gb_animate_sprite_t);

    // module import
    void gb_type_animate_sprite_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_ANIMATE_SPRITE_H
