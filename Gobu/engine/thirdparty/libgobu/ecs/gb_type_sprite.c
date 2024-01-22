#include "gb_type_sprite.h"

ECS_COMPONENT_DECLARE(gb_sprite_t);

void gb_type_sprite_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_sprite_module);
    ECS_COMPONENT_DEFINE(world, gb_sprite_t);

    ECS_IMPORT(world, gb_type_rect_module);
    ECS_IMPORT(world, gb_type_color_module);

    ecs_struct(world, {
        .entity = ecs_id(gb_sprite_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "tint", .type = ecs_id(gb_color_t) },
            {.name = "src", .type = ecs_id(gb_rect_t) },
            {.name = "dst", .type = ecs_id(gb_rect_t) },
        }
    });
}

