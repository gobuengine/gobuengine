#include "bugo_stage.h"

static ecs_entity_t ecs_bugo_stage_t;

void bugo_ecs_init_stage(ecs_world_t* world)
{
    ecs_bugo_stage_t = ecs_component_init(world, &(ecs_component_desc_t) {
        .entity = ecs_entity(world, { .name = "ComponentStage" }),
            .type.size = ECS_SIZEOF(ComponentStage),
            .type.alignment = ECS_ALIGNOF(ComponentStage),
    });

    ecs_struct(world, {
        .entity = ecs_bugo_stage_t,
        .members = {
            {.name = "background", .type = bugo_ecs_get_color_id() },
            {.name = "bshowgrid", .type = ecs_id(ecs_bool_t) },
        },
    });

    ecs_set_id(bugo_ecs_world(), ecs_bugo_stage_t, ecs_bugo_stage_t, sizeof(ComponentStage),
    &(ComponentStage){.bshowgrid = true, .background = (Color){ 28,28,28,255 } });
}

ecs_entity_t bugo_ecs_get_stage_id(void)
{
    return ecs_bugo_stage_t;
}

