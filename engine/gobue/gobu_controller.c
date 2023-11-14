#include "gobu_controller.h"
#include "gobu_physics.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GPlayerPlatform);

static void GobuPlayerController_Set(ecs_iter_t* it);
static void GobuPlayerController_Update(ecs_iter_t* it);

void GobuPlayerControllerImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuPlayerController);
    ECS_IMPORT(world, GobuPhysics);

    ECS_COMPONENT_DEFINE(world, GPlayerPlatform);

    ecs_observer(world, {
        .filter = {.terms = {
                {.id = ecs_id(GPlayerPlatform)},
                {.id = ecs_id(GPhysics)},
        }},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = GobuPlayerController_Set
    });

    ECS_SYSTEM(world, GobuPlayerController_Update, EcsOnUpdate, GPlayerPlatform, GPhysics);
}

static void GobuPlayerController_Set(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    GPlayerPlatform* controller = ecs_field(it, GPlayerPlatform, 1);
    GPhysics* gphysics = ecs_field(it, GPhysics, 2);

    for (int i = 0; i < it->count; i++)
    {

    }
}

static void GobuPlayerController_Update(ecs_iter_t* it)
{
    GPlayerPlatform* controller = ecs_field(it, GPlayerPlatform, 1);
    GPhysics* gphysics = ecs_field(it, GPhysics, 2);

    for (int i = 0; i < it->count; i++)
    {
        if (IsKeyDown(KEY_LEFT)) gphysics[i].body->velocity.x -= 0.002f;
        if (IsKeyDown(KEY_RIGHT)) gphysics[i].body->velocity.x += 0.002f;
        if (IsKeyDown(KEY_SPACE) && gphysics[i].body->isGrounded)
        {
            PhysicsAddForce(gphysics[i].body, (Vector2){0.0f, -5000.0f});
        }
    }
}

