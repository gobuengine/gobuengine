#include "gb_type_resource.h"

ECS_COMPONENT_DECLARE(gb_resource_t);

void gb_type_resource_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_resource_module);
    ECS_COMPONENT_DEFINE(world, gb_resource_t);

    ECS_IMPORT(world, gb_type_texture_module);
    ECS_IMPORT(world, gb_type_text_module);
}

