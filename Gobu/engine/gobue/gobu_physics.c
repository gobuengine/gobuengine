#define PHYSAC_IMPLEMENTATION
#define PHYSAC_DEBUG
#include "gobu_physics.h"
#include "gobu_transform.h"
#include "gobu_shapes.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GPhysics);

static void GobuPhysics_Set(ecs_iter_t* it);
static void GobuPhysics_Update(ecs_iter_t* it);

void GobuPhysicsImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuPhysics);
    ECS_IMPORT(world, GobuTransform);
    ECS_IMPORT(world, GobuShapes);

    ECS_COMPONENT_DEFINE(world, GPhysics);

    InitPhysics();

    ecs_observer(world, {
        .filter = {.terms = {
                {.id = ecs_id(GPhysics)},
                {.id = ecs_id(GPosition)},
                {.id = ecs_id(GShapeRec), .oper = EcsOptional},
        }},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = GobuPhysics_Set
    });

    ECS_SYSTEM(world, GobuPhysics_Update, EcsOnUpdate, GPhysics, GPosition, GRotation);
}

static void GobuPhysics_Set(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    GPhysics* physics = ecs_field(it, GPhysics, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GShapeRec* gshaperec = ecs_field(it, GShapeRec, 3);

    for (int i = 0; i < it->count; i++)
    {
    }
}

static void GobuPhysics_Update(ecs_iter_t* it)
{
    GPhysics* physics = ecs_field(it, GPhysics, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GRotation* rota = ecs_field(it, GRotation, 3);

    for (int i = 0; i < it->count; i++)
    {
    }
}

