#ifndef __GOBU_CUSTOM_H__
#define __GOBU_CUSTOM_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_color_t
    {
        ecs_byte_t r;
        ecs_byte_t g;
        ecs_byte_t b;
        ecs_byte_t a;
    }gb_color_t;

    typedef struct gb_rect_t {
        float x;
        float y;
        float width;
        float height;
    } gb_rect_t;

    typedef struct gb_vec2_t
    {
        float x;
        float y;
    }gb_vec2_t;

    typedef struct gb_vec3_t
    {
        float x;
        float y;
        float z;
    }gb_vec3_t;

    typedef struct gb_bounding_t
    {
        gb_vec3_t min;
        gb_vec3_t max;
    }gb_bounding_t;

    typedef struct gb_transform_t
    {
        gb_vec3_t position;
        gb_vec3_t scale;
        gb_vec3_t rotation;
        gb_vec3_t origin;
    }gb_transform_t;

    typedef struct gb_info_t
    {
        ecs_string_t name;
        ecs_u8_t type;              // 0: entity empty, 1: sprite, 2: text, 3: tiling sprite...
    }gb_info_t;

    typedef struct gb_animated_t
    {
        char* resource;
        char* animation;
        bool is_playing;
        // no-serialize
        float counter;
        int width;
        int height;
        int current;
        int count;
        int frames[100];
        int speed;
        int start_frame;
        int end_frame;
        bool loop;
    }gb_animated_t;

    typedef struct gb_text_t
    {
        char* resource;
        char* text;
        float size;
        float spacing;
        gb_color_t color;
        // no-serialize
        Font font;
    }gb_text_t;

    typedef struct gb_sprite_t
    {
        char* resource;
        gb_color_t tint;
        // no-serialize
        Texture2D texture;
        gb_rect_t src;
        gb_rect_t dst;
    }gb_sprite_t;

    typedef struct gb_shape_rect_t
    {
        float x;
        float y;
        float width;
        float height;
        gb_color_t color;
    }gb_shape_rect_t;

    typedef struct gb_gizmos_t
    {
        bool selected;
    }gb_gizmos_t;

    extern ECS_COMPONENT_DECLARE(gb_color_t);
    extern ECS_COMPONENT_DECLARE(gb_rect_t);
    extern ECS_COMPONENT_DECLARE(gb_vec2_t);
    extern ECS_COMPONENT_DECLARE(gb_vec3_t);
    extern ECS_COMPONENT_DECLARE(gb_transform_t);
    extern ECS_COMPONENT_DECLARE(gb_bounding_t);
    extern ECS_COMPONENT_DECLARE(gb_info_t);
    extern ECS_COMPONENT_DECLARE(gb_animated_t);
    extern ECS_COMPONENT_DECLARE(gb_text_t);
    extern ECS_COMPONENT_DECLARE(gb_sprite_t);
    extern ECS_COMPONENT_DECLARE(gb_shape_rect_t);
    extern ECS_COMPONENT_DECLARE(gb_gizmos_t);

    void GobuCustomImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
