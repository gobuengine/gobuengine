#include "gobu_text.h"
#include "gobu_transform.h"
#include "gobu_rendering.h"
#include "gobu_resource.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GText);

static void GobuText_Set(ecs_iter_t* it);

void GobuTextImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuText);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GText);

    ECS_OBSERVER(world, GobuText_Set, EcsOnSet, GText);
}

static void GobuText_Set(ecs_iter_t* it)
{
    GText* text = ecs_field(it, GText, 1);

    for (int i = 0; i < it->count; i++)
    {
        text[i].font = gobu_resource(it->world, text[i].resource)->font;
    }
}

