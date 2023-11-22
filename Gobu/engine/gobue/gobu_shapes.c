#include "gobu_shapes.h"
#include "gobu_transform.h"
#include "gobu_rendering.h"
#include "raygo/rlgl.h"

ECS_COMPONENT_DECLARE(GShapeRec);

void GobuShapesImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuShapes);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GShapeRec);
}
