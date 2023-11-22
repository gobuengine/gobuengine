#include "gobu_sprite.h"
#include "gobu_transform.h"
#include "gobu_rendering.h"
#include "gobu_resource.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GSpriteAnimated);
ECS_COMPONENT_DECLARE(GSprite);

static void GobuSprite_SetAnimated(ecs_iter_t* it);
static void GobuSprite_UpdateAnimated(ecs_iter_t* it);

static void GobuSprite_Set(ecs_iter_t* it);

void GobuSpriteImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuSprite);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GSpriteAnimated);
    ECS_COMPONENT_DEFINE(world, GSprite);

    ECS_OBSERVER(world, GobuSprite_Set, EcsOnSet, GSprite);
    ECS_OBSERVER(world, GobuSprite_SetAnimated, EcsOnSet, GSprite, GSpriteAnimated);
    ECS_SYSTEM(world, GobuSprite_UpdateAnimated, EcsOnUpdate, GSprite, GSpriteAnimated);

    ecs_struct(world, {
        .entity = ecs_id(GSprite),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(GSpriteAnimated),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "animation", .type = ecs_id(ecs_string_t) },
            {.name = "is_playing", .type = ecs_id(ecs_bool_t) },
        }
    });
}

// Sprite Animated
static void GobuSprite_SetAnimated(ecs_iter_t* it)
{
    GSprite* sprite = ecs_field(it, GSprite, 1);
    GSpriteAnimated* animated = ecs_field(it, GSpriteAnimated, 2);

    for (int i = 0; i < it->count; i++)
    {
        binn* resource = gobu_resource(it->world, animated[i].resource)->anim2d;

        char* anim_id = (animated[i].animation == NULL) ? binn_object_str(resource, "default") : animated[i].animation;
        animated[i].width = binn_object_int32(resource, "width");
        animated[i].height = binn_object_int32(resource, "height");

        sprite[i].src.width = animated[i].width;
        sprite[i].src.height = animated[i].height;

        sprite[i].dst.width = animated[i].width;
        sprite[i].dst.height = animated[i].height;

        binn* animation = binn_object_object(binn_object_object(resource, "animations"), anim_id);
        {
            animated[i].loop = binn_object_bool(animation, "loop");
            animated[i].speed = binn_object_int32(animation, "speed");

            binn* frames = binn_object_list(animation, "frames");
            animated[i].count = binn_count(frames);
            for (int n = 1; n <= animated[i].count; n++) {
                animated[i].frames[n - 1] = binn_list_int32(frames, n);
            }

            animated[i].start_frame = animated[i].frames[0];
            animated[i].end_frame = animated[i].frames[(animated[i].count - 1)];
            animated[i].current = 0;
        }
    }
}

static void GobuSprite_UpdateAnimated(ecs_iter_t* it)
{
    GSprite* sprite = ecs_field(it, GSprite, 1);
    GSpriteAnimated* animated = ecs_field(it, GSpriteAnimated, 2);

    for (int i = 0; i < it->count; i++)
    {
        Texture2D texture = sprite[i].texture;
        int frame_width = animated[i].width;
        int frame_height = animated[i].height;
        int start_frame = animated[i].start_frame;
        int end_frame = animated[i].end_frame;
        float speed = (float)(animated[i].speed * 0.01f);


        if (animated[i].is_playing)
        {
            animated[i].counter += GetFrameTime();
            if (animated[i].counter >= speed)
            {
                animated[i].counter = 0.0f;
                animated[i].current += 1;
                if (animated[i].current > (animated[i].count - 1))
                {
                    if (animated[i].loop)
                        animated[i].current = 0;
                    else {
                        animated[i].current = (animated[i].count - 1);
                        animated[i].is_playing = false;
                    }
                }
            }
        }

        int frame_now = animated[i].frames[animated[i].current];
        int frame_row = (texture.width / frame_width);

        sprite[i].src.x = (frame_now % frame_row) * frame_width;
        sprite[i].src.y = (frame_now / frame_row) * frame_height;
    }
}

// Sprite 
static void GobuSprite_Set(ecs_iter_t* it)
{
    GSprite* sprite = ecs_field(it, GSprite, 1);

    for (int i = 0; i < it->count; i++)
    {
        sprite[i].texture = gobu_resource(it->world, sprite[i].resource)->texture;
        sprite[i].src = (Rectangle){ 0.0f, 0.0f, sprite[i].texture.width, sprite[i].texture.height };
        sprite[i].dst = (Rectangle){ 0.0f, 0.0f, sprite[i].texture.width, sprite[i].texture.height };
        sprite[i].tint = (sprite[i].tint.a == 0) ? WHITE : sprite[i].tint;
    }
}
