#ifndef __GOBU_SPRITE_H__
#define __GOBU_SPRITE_H__
#include <stdio.h>
#include <stdint.h>
#include <flecs/flecs.h>
#include "raygo/raylib.h"
#include "binn/binn.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GSprite
{
    char *resource;
    Rectangle src;
    Rectangle dst;
    Color tint;
    // no-serialize
    Texture2D texture;
}GSprite;

typedef struct GSpriteAnimated
{
    char *resource;
    char *animation;
    bool is_playing;
    // no-serialize
    float counter;
    int width;
    int height;
    int current;
    int count;
    int frames[100];
    bool loop;
    int speed;
    int start_frame;
    int end_frame;
}GSpriteAnimated;

extern ECS_COMPONENT_DECLARE(GSpriteAnimated);
extern ECS_COMPONENT_DECLARE(GSprite);

void GobuSpriteImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
