#include "gb_type_transform.h"

ECS_COMPONENT_DECLARE(gb_transform_t);

void gb_type_transform_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_transform_module);
    ECS_COMPONENT_DEFINE(world, gb_transform_t);

    ECS_IMPORT(world, gb_type_vec2_module);

    ecs_struct(world, {
        .entity = ecs_id(gb_transform_t),
        .members = {
            {.name = "position", .type = ecs_id(gb_vec2_t) },
            {.name = "scale", .type = ecs_id(gb_vec2_t) },
            {.name = "rotation", .type = ecs_id(ecs_f32_t) },
            {.name = "origin", .type = ecs_id(gb_vec2_t) },
        }
    });
}
