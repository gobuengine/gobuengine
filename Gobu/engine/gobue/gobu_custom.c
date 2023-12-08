#include "gobu_custom.h"

ECS_COMPONENT_DECLARE(gb_color_t);
ECS_COMPONENT_DECLARE(gb_rect_t);
ECS_COMPONENT_DECLARE(gb_vec2_t);
ECS_COMPONENT_DECLARE(gb_vec3_t);
ECS_COMPONENT_DECLARE(gb_bounding_t);
ECS_COMPONENT_DECLARE(gb_transform_t);
ECS_COMPONENT_DECLARE(gb_info_t);
ECS_COMPONENT_DECLARE(gb_animated_t);
ECS_COMPONENT_DECLARE(gb_text_t);
ECS_COMPONENT_DECLARE(gb_sprite_t);
ECS_COMPONENT_DECLARE(gb_shape_rect_t);
ECS_COMPONENT_DECLARE(gb_gizmos_t);

void GobuCustomImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuCustom);

    ECS_COMPONENT_DEFINE(world, gb_color_t);
    ECS_COMPONENT_DEFINE(world, gb_rect_t);
    ECS_COMPONENT_DEFINE(world, gb_vec2_t);
    ECS_COMPONENT_DEFINE(world, gb_vec3_t);
    ECS_COMPONENT_DEFINE(world, gb_bounding_t);
    ECS_COMPONENT_DEFINE(world, gb_transform_t);
    ECS_COMPONENT_DEFINE(world, gb_info_t);
    ECS_COMPONENT_DEFINE(world, gb_animated_t);
    ECS_COMPONENT_DEFINE(world, gb_text_t);
    ECS_COMPONENT_DEFINE(world, gb_sprite_t);
    ECS_COMPONENT_DEFINE(world, gb_shape_rect_t);
    ECS_COMPONENT_DEFINE(world, gb_gizmos_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_color_t),
        .members = {
            {.name = "r", .type = ecs_id(ecs_byte_t) },
            {.name = "g", .type = ecs_id(ecs_byte_t) },
            {.name = "b", .type = ecs_id(ecs_byte_t) },
            {.name = "a", .type = ecs_id(ecs_byte_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "width", .type = ecs_id(ecs_f32_t) },
            {.name = "height", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_vec2_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_vec3_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "z", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_transform_t),
        .members = {
            {.name = "position", .type = ecs_id(gb_vec3_t) },
            {.name = "scale", .type = ecs_id(gb_vec3_t) },
            {.name = "rotation", .type = ecs_id(gb_vec3_t) },
            {.name = "origin", .type = ecs_id(gb_vec3_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_info_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t) },
            {.name = "type", .type = ecs_id(ecs_u8_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animated_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "animation", .type = ecs_id(ecs_string_t) },
            {.name = "is_playing", .type = ecs_id(ecs_bool_t) },
        }
    });

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

    ecs_struct(world, {
        .entity = ecs_id(gb_sprite_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "tint", .type = ecs_id(gb_color_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "width", .type = ecs_id(ecs_f32_t) },
            {.name = "height", .type = ecs_id(ecs_f32_t) },
            {.name = "color", .type = ecs_id(gb_color_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_gizmos_t),
        .members = {
            {.name = "selected", .type = ecs_id(ecs_bool_t) },
        }
    });
}

