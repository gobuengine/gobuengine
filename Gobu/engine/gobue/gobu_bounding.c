#include "gobu_bounding.h"

ECS_COMPONENT_DECLARE(GBoundingBox);

void GobuBoundingBoxImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuBoundingBox);
    ECS_IMPORT(world, GobuCustom);
    
    ECS_COMPONENT_DEFINE(world, GBoundingBox);
}
