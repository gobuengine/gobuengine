#include "bugo_bcolor.h"

static ecs_entity_t ecs_color_t;

void bugo_ecs_init_color(ecs_world_t *world)
{
    ecs_color_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .type.size = ECS_SIZEOF(Color),
        .type.alignment = ECS_ALIGNOF(Color),
    });

    ecs_struct(world, {
        .entity = ecs_color_t,
        .members = {
            { .name = "r", .type = ecs_id(ecs_byte_t) },
            { .name = "g", .type = ecs_id(ecs_byte_t) },
            { .name = "b", .type = ecs_id(ecs_byte_t) },
            { .name = "a", .type = ecs_id(ecs_byte_t) },
        },
    });
}

void bugo_ecs_set_color(ecs_entity_t entity, Color *props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_color_t, sizeof(Color), props);
}

ecs_entity_t bugo_ecs_get_color_id(void)
{
    return ecs_color_t;
}
