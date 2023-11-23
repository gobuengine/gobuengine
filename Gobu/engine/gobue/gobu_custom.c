#include "gobu_custom.h"

ECS_COMPONENT_DECLARE(ecs_vec2_t);
ECS_COMPONENT_DECLARE(ecs_vec3_t);

void GobuCustomImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuCustom);

    ECS_COMPONENT_DEFINE(world, ecs_vec2_t);
    ECS_COMPONENT_DEFINE(world, ecs_vec3_t);

    ecs_struct(world, {
        .entity = ecs_id(ecs_vec2_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(ecs_vec3_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "z", .type = ecs_id(ecs_f32_t) },
        }
    });
}

