#include "pixio.h"
#include "pixio_type.h"
#include "pixio_render.h"

ecs_world_t *pixio_world_init(void)
{
    ecs_world_t *ecs = ecs_init();

    // registro de componentes
    ECS_IMPORT(ecs, pixio_rendering_module);

    // ecs_insert(ecs, ecs_value(pixio_render_t, {.viewport = {800, 600}, .clear_color = WHITE, .viewport_lineColor = SKYBLUE, .grid_size = 64}));
    ecs_singleton_set(ecs, pixio_render_t, {.viewport = {800, 600}, .clear_color = WHITE, .viewport_lineColor = SKYBLUE, .grid_size = 64, .grid_enabled = true});

    return ecs;
}

void pixio_world_process(ecs_world_t *ecs, float deltaTime)
{
    ecs_progress(ecs, deltaTime);
}

// Creates a new pixio_entity_t structure for a given entity in the ECS world.
// The caller is responsible for freeing the returned structure using pixio_entity_free().
pixio_entity *pixio_entity_new(ecs_world_t *world, ecs_entity_t entity)
{
    pixio_entity *pentity = g_new0(pixio_entity, 1);
    pentity->entity = entity;
    pentity->world = world;
    return pentity;
}

// Frees a pixio_entity_t structure and its associated resources.
void pixio_entity_free(pixio_entity *pixio_entity)
{
    if (pixio_entity)
    {
        g_free(pixio_entity);
    }
}

ecs_entity_t pixio_new(ecs_world_t *world, ecs_entity_t parent, const char *name)
{
    ecs_entity_t entity = ecs_new_low_id(world);
    if (parent > 0)
        pixio_set_parent(world, entity, parent);

    // asignamos un nombre
    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, parent, name, ".", ".", true);
    const char *name_id = (lookup == 0) ? name : g_strdup_printf("%s%ld", name, entity);
    ecs_set_name(world, entity, name_id);

    // component:transform
    // ecs_set(world, entity, pixio_entity_t, {.name = g_strdup(name_id), .enabled = TRUE});
    ecs_set(world, entity, pixio_transform_t, {.position = {0, 0}, .scale = {1, 1}, .rotation = 0, .origin = PIXIO_CENTER});

    return entity;
}

ecs_entity_t pixio_get_root(ecs_world_t *world)
{
    return ecs_lookup(world, "Root");
}

void pixio_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}

ecs_entity_t pixio_get_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_target(world, entity, EcsChildOf, 0);
    // return ecs_childof(entity);
}

bool pixio_has_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_has_id(world, entity, ecs_pair(EcsChildOf, EcsWildcard));
}

ecs_entity_t pixio_clone(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t clone = ecs_clone(world, 0, entity, TRUE);

    ecs_iter_t it = ecs_children(world, entity);
    while (ecs_children_next(&it))
    {
        for (int i = 0; i < it.count; i++)
        {
            ecs_entity_t en1 = it.entities[i];
            if (ecs_is_alive(world, en1))
            {
                ecs_entity_t child = pixio_clone(world, en1);
                pixio_set_parent(world, child, clone);
            }
        }
    }

    return clone;
}

void pixio_set_name(ecs_world_t *world, ecs_entity_t entity, const char *name)
{
    if (pixio_get_root(world) != entity)
        ecs_set_name(world, entity, name);
}

// Retrieves the name of a given entity from the ECS world.
// Note: The caller is responsible for freeing the returned string.
const char *pixio_get_name(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_name(world, entity);
}

void pixio_set_enabled(ecs_world_t *world, ecs_entity_t entity, bool enabled)
{
    ecs_enable(world, entity, enabled);
}

bool pixio_get_enabled(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_has_id(world, entity, EcsDisabled) == 0;
}
