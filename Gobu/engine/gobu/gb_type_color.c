#include "gb_type_color.h"

ECS_COMPONENT_DECLARE(gb_color_t);

void gb_type_color_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_color_module);
    ECS_COMPONENT_DEFINE(world, gb_color_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_color_t),
        .members = {
            {.name = "r", .type = ecs_id(ecs_byte_t) },
            {.name = "g", .type = ecs_id(ecs_byte_t) },
            {.name = "b", .type = ecs_id(ecs_byte_t) },
            {.name = "a", .type = ecs_id(ecs_byte_t) },
        }
    });
}
