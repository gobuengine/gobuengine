#include "gobu_ecs.h"
#include "gobu_gobu.h"

ecs_entity_t gobu_ecs_new_entity(ecs_world_t* world, const char* name)
{
    ecs_entity_t entity = ecs_new_entity(world, name);
    ecs_add(world, entity, gb_entity_t, { .name = gb_strdup(name) });

    // ecs_entity_t entity = ecs_new_id(world);
    ecs_set_name(world, entity, gb_strdups("%s%d", name, entity));
    ecs_add_pair(world, entity, EcsChildOf, ecs_lookup(world, "World"));
    ecs_set(world, entity, GPosition, { mouseWorld.x, mouseWorld.y });
    ecs_set(world, entity, GScale, { 1.0f, 1.0f });
    ecs_set(world, entity, GRotation, { 0.0f, 0.0f });
    ecs_set(world, entity, GBoundingBox, { 0.0f, 0.0f, 0.0f, 0.0f });
    // Solo para el editor de nivel: experimental
    ecs_set(world, entity, ecs_gizmos_t, { 0 });

    return entity;
}

