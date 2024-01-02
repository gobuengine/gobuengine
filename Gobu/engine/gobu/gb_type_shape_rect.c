#include "gb_type_shape_rect.h"

ECS_COMPONENT_DECLARE(gb_shape_rect_t);

void gb_type_shape_rect_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_shape_rect_module);
    ECS_COMPONENT_DEFINE(world, gb_shape_rect_t);

    ECS_IMPORT(world, gb_type_color_module);

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "width", .type = ecs_id(ecs_f32_t) },
            {.name = "height", .type = ecs_id(ecs_f32_t) },
            // CSS Style
            {.name = "background_color", .type = ecs_id(gb_color_t) },
            {.name = "border_width", .type = ecs_id(ecs_i32_t) },
            {.name = "border_color", .type = ecs_id(gb_color_t) },
            {.name = "border_radius_top_left", .type = ecs_id(ecs_i32_t) },
            {.name = "border_radius_top_right", .type = ecs_id(ecs_i32_t) },
            {.name = "border_radius_bottom_left", .type = ecs_id(ecs_i32_t) },
            {.name = "border_radius_bottom_right", .type = ecs_id(ecs_i32_t) },
        }
    });
}
