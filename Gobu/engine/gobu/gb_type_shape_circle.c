#include "gb_type_shape_circle.h"

ECS_COMPONENT_DECLARE(gb_shape_circle_t);

void gb_type_shape_circle_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_shape_circle_module);
    ECS_COMPONENT_DEFINE(world, gb_shape_circle_t);

    ECS_IMPORT(world, gb_type_color_module);

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_circle_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "radius", .type = ecs_id(ecs_f32_t) },
            {.name = "background_color", .type = ecs_id(gb_color_t) },
        }
    });
}
