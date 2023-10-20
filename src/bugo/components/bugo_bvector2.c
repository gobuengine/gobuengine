#include "bugo_bvector2.h"

static ecs_entity_t ecs_vector2_t;

void bugo_ecs_init_vector2(ecs_world_t *world)
{
    ecs_vector2_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Vector2"}),
        .type.size = ECS_SIZEOF(Vector2),
        .type.alignment = ECS_ALIGNOF(Vector2),
    });

    ecs_struct(world, {
        .entity = ecs_vector2_t,
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) },
        },
    });
}

void bugo_ecs_set_vector2(ecs_entity_t entity, Vector2 *props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_vector2_t, sizeof(Vector2), props);
}

ecs_entity_t bugo_ecs_get_vector2_id(void)
{
    return ecs_vector2_t;
}
