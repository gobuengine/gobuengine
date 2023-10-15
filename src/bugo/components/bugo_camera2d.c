#include "bugo_camera2d.h"

static ecs_entity_t ecs_camera2d_t;

void bugo_ecs_init_camera2d(ecs_world_t *world)
{
    ecs_camera2d_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Camera2D"}),
        .type.size = ECS_SIZEOF(Camera2D),
        .type.alignment = ECS_ALIGNOF(Camera2D),
    });

    ecs_struct(world, {
        .entity = ecs_camera2d_t,
        .members = {
            { .name = "offset", .type = bugo_ecs_get_vector2_id() },
            { .name = "target", .type = bugo_ecs_get_vector2_id() },
            { .name = "rotation", .type = ecs_id(ecs_byte_t) },
            { .name = "zoom", .type = ecs_id(ecs_byte_t) },
        },
    });
}

void bugo_ecs_set_camera2d(ecs_entity_t entity, Camera2D *props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_camera2d_t, sizeof(Camera2D), props);
}

ecs_entity_t bugo_ecs_get_camera2d_id(void)
{
    return ecs_camera2d_t;
}
