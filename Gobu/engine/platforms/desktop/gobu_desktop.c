#include <stdio.h>
#include "gobu_rendering.h"
#include "gobu_shapes.h"
#include "gobu_transform.h"
#include "gobu_autorotate.h"
#include "gobu_text.h"
#include "gobu_sprite.h"
#include "gobu_resource.h"
#include "gobu_physics.h"
#include "gobu_controller.h"
#include "gobu_collision.h"

int main(int argc, char* argv[])
{
    ecs_world_t* world = ecs_init();

    ECS_IMPORT(world, GobuRendering);
    ECS_IMPORT(world, GobuTransform);
    ECS_IMPORT(world, GobuAutoRotate);
    ECS_IMPORT(world, GobuShapes);
    ECS_IMPORT(world, GobuText);
    ECS_IMPORT(world, GobuSprite);
    ECS_IMPORT(world, GobuResource);
    ECS_IMPORT(world, GobuPhysics);
    ECS_IMPORT(world, GobuPlayerController);
    ECS_IMPORT(world, GobuCollision);

    gobu_rendering_init(world, 1280, 720, "Gobu - Example2", false);

    gobu_resource_set(world, "good_sprite", "Content/goo_weapon.png");
    gobu_resource_set(world, "good_anim", "Content/goo_weapon.anim");

    gobu_resource_set(world, "roberto_sprite", "Content/RobertoSheet.png");
    gobu_resource_set(world, "roberto_anim", "Content/RobertoSheet.anim");

    ecs_entity_t e = ecs_new_id(world);
    ecs_set(world, e, GPosition, { 100, 720 - 120 });
    ecs_set(world, e, GShapeRec, { .width = (1280 - 200), .height = 50, .color = DARKBROWN });
    ecs_set(world, e, GPhysics, {.enabled = false});
    // ecs_set(world, e, GCollisionBox, {0});

    ecs_entity_t good = ecs_new_id(world);
    ecs_set(world, good, GSprite, { .resource = "good_sprite" });
    ecs_set(world, good, GSpriteAnimated, { .resource = "good_anim", .is_playing = true });
    ecs_set(world, good, GPosition, { 300, 300 });
    ecs_set(world, good, GPhysics, {.enabled = true});
    ecs_set(world, good, GCollisionBox, {.size = {64, 64}});
    ecs_set(world, good, GPlayerPlatform, {0});

    ecs_entity_t roberto = ecs_new_id(world);
    ecs_set(world, roberto, GSprite, { .resource = "roberto_sprite" });
    ecs_set(world, roberto, GSpriteAnimated, { .resource = "roberto_anim", .is_playing = true });
    ecs_set(world, roberto, GPosition, { 400, 486 });
    ecs_set(world, roberto, GCollisionBox, {.size = {64, 64}});

    gobu_rendering_main(world);

    return 0;
}

