#include "gobu_autorotate.h"
#include "gobu_transform.h"
#include "raygo/rlgl.h"

ECS_COMPONENT_DECLARE(GAutoRotate);

static void GobuAutoRotate_Update(ecs_iter_t* it);

void GobuAutoRotateImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuAutoRotate);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GAutoRotate);

    ECS_SYSTEM(world, GobuAutoRotate_Update, EcsOnUpdate, GAutoRotate, GRotation);
}

static void GobuAutoRotate_Update(ecs_iter_t* it)
{
    GAutoRotate* autoRotate = ecs_field(it, GAutoRotate, 1);
    GRotation* rotation = ecs_field(it, GRotation, 2);

    for (int i = 0; i < it->count; i++)
    {
        rotation[i].x += autoRotate[i].speed * it->delta_time;
    }
}

