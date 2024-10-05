#include "pixio.h"
#include "pixio_type.h"
#include "pixio_render.h"

ecs_world_t *pixio_init(void)
{
    ecs_world_t *ecs = ecs_init();

    // registro de componentes
    ECS_IMPORT(ecs, pixio_rendering_module);

    ecs_insert(ecs, ecs_value(pixio_render_t, {0}));

    return ecs;
}

void pixio_process(ecs_world_t *ecs, float deltaTime)
{
    ecs_progress(ecs, deltaTime);
}

ecs_entity_t pixio_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name)
{
    ecs_entity_t entity = ecs_new_low_id(world);
    if (parent > 0)
        pixio_entity_set_parent(world, entity, parent);

    // asignamos un nombre
    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, parent, name, ".", ".", true);
    const char *name_id = (lookup == 0) ? name : g_strdup_printf("%s%ld", name, entity);
    ecs_set_name(world, entity, name_id);

    // component:transform
    ecs_set(world, entity, pixio_transform_t, {.position = {0, 0}, .scale = {1, 1}, .rotation = 0, .origin = {0.5, 0.5}});

    return entity;
}

void pixio_entity_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}
