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

    ECS_SYSTEM(world, GobuPhysics_Update, EcsOnUpdate, GPhysics, GPosition, ?GRotation);
}

static void GobuPhysics_Set(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    GPhysics* physics = ecs_field(it, GPhysics, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GShapeRec* gshaperec = ecs_field(it, GShapeRec, 3);

    for (int i = 0; i < it->count; i++)
    {
        // PHYSACDEF PhysicsBody CreatePhysicsBodyCircle(Vector2 pos, float radius, float density);                    // Creates a new circle physics body with generic parameters
        // PHYSACDEF PhysicsBody CreatePhysicsBodyRectangle(Vector2 pos, float width, float height, float density);    // Creates a new rectangle physics body with generic parameters
        // PHYSACDEF PhysicsBody CreatePhysicsBodyPolygon(Vector2 pos, float radius, int sides, float density);  
        if (event == EcsOnSet) {
            int width = 64, height = 64, radius = 20;
            float density = (physics[i].density == 0.0f) ? 10.0f : physics[i].density;

            // Shape Rectangle
            if (gshaperec) {
                width = gshaperec[i].width;
                height = gshaperec[i].height + 14;
            }

            physics[i].body = CreatePhysicsBodyRectangle((Vector2) { post[i].x, post[i].y }, width, height, density);

            physics[i].body->enabled = physics[i].enabled;
            physics[i].body->restitution = physics[i].restitution;
            physics[i].body->freezeOrient = physics[i].freezeOrient;
        }
        else if (event == EcsOnRemove)
            DestroyPhysicsBody(physics[i].body);
    }
}

static void GobuPhysics_Update(ecs_iter_t* it)
{
    GPhysics* physics = ecs_field(it, GPhysics, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GRotation* rota = ecs_field(it, GRotation, 3);

    for (int i = 0; i < it->count; i++)
    {
        post[i].x = physics[i].body->position.x;
        post[i].y = physics[i].body->position.y;

        if (rota)
            rota[i] = physics[i].body->orient;
    }
}

