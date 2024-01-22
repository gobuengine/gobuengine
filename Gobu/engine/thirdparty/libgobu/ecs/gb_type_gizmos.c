#include "gb_type_gizmos.h"

ECS_COMPONENT_DECLARE(gb_gizmos_t);

void gb_type_gizmos_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_gizmos_module);
    ECS_COMPONENT_DEFINE(world, gb_gizmos_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_gizmos_t),
        .members = {
            {.name = "selected", .type = ecs_id(ecs_bool_t) },
        }
    });
}
