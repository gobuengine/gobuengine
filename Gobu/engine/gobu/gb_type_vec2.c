#include "gb_type_vec2.h"

ECS_COMPONENT_DECLARE(gb_vec2_t);

void gb_type_vec2_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_vec2_module);
    ECS_COMPONENT_DEFINE(world, gb_vec2_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_vec2_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
        }
    });
}
