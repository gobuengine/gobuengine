#include "gb_ecs_animate_sprite.h"

static void set_observe_animate_sprite(ecs_iter_t* it);
static void post_update_animate_sprite(ecs_iter_t* it);

void gb_animate_sprite_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_animate_sprite_module);
    ECS_IMPORT(world, gb_type_animate_sprite_module);

    ecs_observer(world, {
         .filter = {.terms = { {.id = ecs_id(gb_animate_sprite_t)}, {.id = ecs_id(gb_sprite_t)} }},
         .events = { EcsOnSet },
         .callback = set_observe_animate_sprite
     });

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsPostUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_animate_sprite_t)}, {.id = ecs_id(gb_sprite_t)} },
        .callback = post_update_animate_sprite
    });
}


// --
// gb_animate_sprite_t: EVENTS
// --
static void set_observe_animate_sprite(ecs_iter_t* it)
{
    gb_animate_sprite_t* animate = ecs_field(it, gb_animate_sprite_t, 1);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 2);

    ecs_entity_t event = it->event;

    for (int i = 0; i < it->count; i++)
    {
        if (event == EcsOnSet) {
            ecs_vec_init(NULL, &animate->animations, ecs_id(gb_animate_animation_t), 200);
        }
        else if (event == EcsOnRemove) {
            ecs_vec_fini(NULL, &animate->animations, ecs_id(gb_animate_animation_t));
        }
    }
}

static void post_update_animate_sprite(ecs_iter_t* it)
{
    gb_animate_sprite_t* animated = ecs_field(it, gb_animate_sprite_t, 1);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 2);

    for (int i = 0; i < it->count; i++)
    {
        if (ecs_vec_count(&animated[i].animations) > 0)
        {
            gb_animate_animation_t* animation = ecs_vec_get_t(&animated[i].animations, gb_animate_animation_t, animated[i].current);
            int count_frames = ecs_vec_count(&animation->frames);

            if (count_frames > 0)
            {
                gb_animate_frame_t* frame = ecs_vec_get_t(&animation->frames, gb_animate_frame_t, animated[i].current_frame);

                if (animated[i].is_playing)
                {
                    animated[i].counter += GetFrameTime();
                    if ((animated[i].counter) >= (float)(60 / animation->fps) * (float)(frame->duration * 0.02f))
                    {
                        animated[i].counter = 0.0f;
                        animated[i].current_frame++;
                        if (animated[i].current_frame >= count_frames)
                        {
                            animated[i].current_frame = 0;
                            if (!animation->loop)
                            {
                                animated[i].is_playing = false;
                            }
                        }
                    }
                }

                // clip sprite
                float x = frame->sprite.src.x;
                float y = frame->sprite.src.y;
                float w = frame->sprite.dst.width;
                float h = frame->sprite.dst.height;

                float clip_width = (w == 0) ? frame->sprite.texture.width : w;
                float clip_height = (h == 0) ? frame->sprite.texture.height : h;

                sprite[i].texture = frame->sprite.texture;
                sprite[i].src = (gb_rect_t){ x, y, clip_width, clip_height };
                sprite[i].dst = (gb_rect_t){ 0.0f, 0.0f, clip_width, clip_height };
                sprite[i].tint = (frame->sprite.tint.a == 0) ? (gb_color_t) { 255, 255, 255, 255 } : frame->sprite.tint;
            }
            else {
                animated[i].is_playing = false;
                sprite[i].texture.id = 0;
            }
        }
        else {
            animated[i].is_playing = false;
            sprite[i].texture.id = 0;
        }
    }
}

// --
// gb_animate_sprite_t: API
// --
void gb_animate_sprite_stop(gb_animate_sprite_t* animated)
{
    animated->is_playing = false;
    animated->current_frame = 0;
    animated->counter = 0.0f;
}

void gb_animate_sprite_pause(gb_animate_sprite_t* animated)
{
    animated->is_playing = false;
}

void gb_animate_sprite_play(gb_animate_sprite_t* animated)
{
    animated->is_playing = true;
}

bool gb_animate_sprite_is_playing(gb_animate_sprite_t* animated)
{
    return animated->is_playing;
}

bool gb_animate_sprite_is_looping(gb_animate_sprite_t* animated)
{
    gb_animate_animation_t* animation = ecs_vec_get_t(&animated->animations, gb_animate_animation_t, animated->current);
    return animation->loop;
}

void gb_animate_sprite_set(gb_animate_sprite_t* animated, const char* name)
{
    for (int i = 0; i < ecs_vec_count(&animated->animations); i++)
    {
        gb_animate_animation_t* animation = ecs_vec_get_t(&animated->animations, gb_animate_animation_t, i);

        if (strcmp(animation->name, name) == 0) {
            animated->current = i;
            animated->current_frame = 0;
            animated->counter = 0.0f;
            animated->is_playing = true;
            return;
        }
    }

    gb_animate_sprite_stop(animated);
}

