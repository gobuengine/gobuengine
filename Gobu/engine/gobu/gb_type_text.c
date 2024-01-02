#include "gb_type_text.h"

ECS_COMPONENT_DECLARE(gb_font_t);
ECS_COMPONENT_DECLARE(gb_text_t);

void gb_type_text_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_text_module);
    ECS_COMPONENT_DEFINE(world, gb_font_t);
    ECS_COMPONENT_DEFINE(world, gb_text_t);

    ECS_IMPORT(world, gb_type_color_module);

    ecs_struct(world, {
        .entity = ecs_id(gb_text_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "text", .type = ecs_id(ecs_string_t) },
            {.name = "size", .type = ecs_id(ecs_f32_t) },
            {.name = "spacing", .type = ecs_id(ecs_f32_t) },
            {.name = "color", .type = ecs_id(gb_color_t) },
        }
    });
}
