#include "gobu_gobu.h"

void gobu_import_all(ecs_world_t* world) {
    ECS_IMPORT(world, GobuCustom);
    
    ECS_IMPORT(world, GobuTransform);
    ECS_IMPORT(world, GobuRendering);
    ECS_IMPORT(world, GobuResource);
    ECS_IMPORT(world, GobuCamera);
    
    ECS_IMPORT(world, GobuAutoRotate);
    ECS_IMPORT(world, GobuPhysics);
    ECS_IMPORT(world, GobuPlayerController);
    ECS_IMPORT(world, GobuCollision);
    ECS_IMPORT(world, GobuInputSystem);

    ECS_IMPORT(world, GobuShapes);
    ECS_IMPORT(world, GobuText);
    ECS_IMPORT(world, GobuSprite);

    ECS_IMPORT(world, GobuBoundingBox);
    ECS_IMPORT(world, GobuGizmos);
}
