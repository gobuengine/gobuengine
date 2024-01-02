#include "gb_type_rect.h"

ECS_COMPONENT_DECLARE(gb_rect_t);

void gb_type_rect_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_rect_module);
    ECS_COMPONENT_DEFINE(world, gb_rect_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "width", .type = ecs_id(ecs_f32_t) },
            {.name = "height", .type = ecs_id(ecs_f32_t) },
        }
    });
}
