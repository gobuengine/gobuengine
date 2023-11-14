#include "gobu_collision.h"
#include "gobu_transform.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GCollisionBox);
ECS_COMPONENT_DECLARE(GCollisionCircle);

static void GobuCollision_Update(ecs_iter_t* it);

void GobuCollisionImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuCollision);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GCollisionBox);
    ECS_COMPONENT_DEFINE(world, GCollisionCircle);

    ECS_SYSTEM(world, GobuCollision_Update, EcsOnUpdate, GPosition, GCollisionBox);
}

static void GobuCollision_Update(ecs_iter_t* it)
{
    GPosition* post = ecs_field(it, GPosition, 1);
    GCollisionBox* box = ecs_field(it, GCollisionBox, 2);
    // GCollisionCircle* circle = ecs_field(it, GCollisionCircle, 3);

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entitie = it->entities[i];

        ecs_filter_t* f = ecs_filter(it->world, { .terms = { { ecs_id(GPosition)}, { ecs_id(GCollisionBox)} } });
        ecs_iter_t fit = ecs_filter_iter(it->world, f);

        BoundingBox box1 = (BoundingBox){ 0 };

        while (ecs_filter_next(&fit)) {
            GPosition* fpost = ecs_field(&fit, GPosition, 1);
            GCollisionBox* fbox = ecs_field(&fit, GCollisionBox, 2);

            for (int n = 0; n < fit.count; n++) {
                if (entitie == fit.entities[n])continue;

                BoundingBox box2 = (BoundingBox){ 0 };

                if (CheckCollisionBoxes(box1, box2)) {
                    printf("Collision\n");
                    break;
                }
            }
        }
    }

}

