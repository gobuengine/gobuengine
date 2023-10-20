#include "bugo_brectangle.h"

static ecs_entity_t ecs_rectangle_t;

void bugo_ecs_init_rectangle(ecs_world_t *world)
{
    ecs_rectangle_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Rectangle"}),
        .type.size = ECS_SIZEOF(Rectangle),
        .type.alignment = ECS_ALIGNOF(Rectangle),
    });

    ecs_struct(world, {
        .entity = ecs_rectangle_t,
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) },
            { .name = "width", .type = ecs_id(ecs_f32_t) },
            { .name = "height", .type = ecs_id(ecs_f32_t) },
        },
    });
}

void bugo_ecs_set_rectangle(ecs_entity_t entity, Rectangle *props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_rectangle_t, sizeof(Rectangle), props);
}

ecs_entity_t bugo_ecs_get_rectangle_id(void)
{
    return ecs_rectangle_t;
}

