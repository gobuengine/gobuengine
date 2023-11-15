#include "gobu_transform.h"

ECS_COMPONENT_DECLARE(GPosition);
ECS_COMPONENT_DECLARE(GScale);
ECS_COMPONENT_DECLARE(GRotation);

void GobuTransformImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GPosition);
    ECS_COMPONENT_DEFINE(world, GScale);
    ECS_COMPONENT_DEFINE(world, GRotation);

    ecs_struct(world, {
        .entity = ecs_id(GPosition),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(GScale),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_primitive(world, {
        .entity = ecs_id(GRotation),
        .kind = ecs_id(ecs_f32_t)
    });
}

