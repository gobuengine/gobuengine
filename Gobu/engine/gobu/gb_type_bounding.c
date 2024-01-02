#include "gb_type_bounding.h"

ECS_COMPONENT_DECLARE(gb_bounding_t);

void gb_type_bounding_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_bounding_module);
    ECS_COMPONENT_DEFINE(world, gb_bounding_t);

    ECS_IMPORT(world, gb_type_vec2_module);
}
