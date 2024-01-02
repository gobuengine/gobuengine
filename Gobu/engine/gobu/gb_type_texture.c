#include "gb_type_texture.h"

ECS_COMPONENT_DECLARE(gb_texture_t);

void gb_type_texture_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_type_texture_module);
    ECS_COMPONENT_DEFINE(world, gb_texture_t);
}
