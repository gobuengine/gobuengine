#include "gb_type_animate_sprite.h"

ECS_COMPONENT_DECLARE(gb_animate_frame_t);
ECS_COMPONENT_DECLARE(gb_animate_animation_t);
ECS_COMPONENT_DECLARE(gb_animate_sprite_t);

void gb_type_animate_sprite_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_animate_sprite_module);

    ECS_COMPONENT_DEFINE(world, gb_animate_frame_t);
    ECS_COMPONENT_DEFINE(world, gb_animate_animation_t);
    ECS_COMPONENT_DEFINE(world, gb_animate_sprite_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_animate_frame_t),
        .members = {
            {.name = "duration", .type = ecs_id(ecs_i32_t) },
            {.name = "sprite", .type = ecs_id(gb_sprite_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animate_animation_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t) },
            {.name = "fps", .type = ecs_id(ecs_i16_t) },
            {.name = "loop", .type = ecs_id(ecs_bool_t) },
            {.name = "frames", .type = ecs_vector(world, {.entity = ecs_entity(world, {.name = "frames" }),.type = ecs_id(gb_animate_frame_t)})},
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animate_sprite_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "animation", .type = ecs_id(ecs_string_t) },
            {.name = "animations", .type = ecs_vector(world, {.entity = ecs_entity(world, {.name = "animations" }),.type = ecs_id(gb_animate_animation_t)})},
        }
    });
}
