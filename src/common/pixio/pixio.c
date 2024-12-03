#include "pixio.h"
#include "pixio_type.h"
#include "pixio_render.h"

static ecs_entity_to_json_desc_t desc_entity_json_init = ECS_ENTITY_TO_JSON_INIT;

ecs_world_t *pixio_world_init(void)
{
    ecs_world_t *ecs = ecs_init();
    ECS_IMPORT(ecs, pixio_rendering_module);
    ecs_singleton_set(ecs, pixio_render_t, {.viewport = {800, 600}, .clear_color = WHITE, .viewport_lineColor = SKYBLUE, .grid_size = 64, .grid_enabled = true});
    return ecs;
}

void pixio_free(ecs_world_t *ecs)
{
    ecs_fini(ecs);
}

void pixio_world_process(ecs_world_t *ecs, float deltaTime)
{
    ecs_progress(ecs, deltaTime);
}

char *pixio_world_serialize(ecs_world_t *world)
{
    return ecs_world_to_json(world, NULL);
}

void pixio_world_deserialize(ecs_world_t *world, const char *json)
{
    ecs_world_from_json(world, json, NULL);
}

char *pixio_entity_stringify(ecs_world_t *world, ecs_entity_t entity)
{
    desc_entity_json_init.serialize_doc = true;
    return ecs_entity_to_json(world, entity, &desc_entity_json_init);
}

void pixio_entity_parse(ecs_world_t *world, ecs_entity_t entity, const char *json)
{
    desc_entity_json_init.serialize_doc = true;
    ecs_entity_from_json(world, entity, json, NULL);
}

ecs_entity_t pixio_entity_new_low(ecs_world_t *world, ecs_entity_t parent)
{
    ecs_entity_t entity = ecs_new_low_id(world);
    if (parent > 0)
        pixio_set_parent(world, entity, parent);

    return entity;
}

ecs_entity_t pixio_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name)
{
    ecs_entity_t entity = pixio_entity_new_low(world, parent);
    gchar *name_entity = strcmp(name, PIXIO_ENTITY_ROOT_NAME)!=0 ? g_strdup_printf("%s%ld", name, entity) : g_strdup(name);
    pixio_set_name(world, entity, name_entity);
    ecs_set(world, entity, pixio_transform_t, {.position = {0, 0}, .scale = {1, 1}, .rotation = 0, .origin = PIXIO_CENTER});

    return entity;
}

ecs_entity_t pixio_entity_new_root(ecs_world_t *world)
{
    ecs_entity_t root = pixio_entity_new(world, 0, PIXIO_ENTITY_ROOT_NAME);
    ecs_set_name(world, root, PIXIO_ENTITY_ROOT_NAME);
    return root;
}

ecs_entity_t pixio_get_root(ecs_world_t *world)
{
    return ecs_lookup(world, PIXIO_ENTITY_ROOT_NAME);
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

bool pixio_is_alive(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_is_alive(world, entity);
}

void pixio_delete(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_delete(world, entity);
}

ecs_entity_t pixio_clone(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t clone = ecs_clone(world, 0, entity, TRUE);
    pixio_set_name(world, clone, g_strdup_printf("%s%ld", pixio_get_name(world, entity), clone));
    pixio_set_parent(world, clone, pixio_get_parent(world, entity));

    ecs_iter_t it = ecs_children(world, entity);
    while (ecs_children_next(&it))
    {
        for (int i = 0; i < it.count; i++)
        {
            ecs_entity_t en1 = it.entities[i];
            if (pixio_is_alive(world, en1))
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
    ecs_set_name(world, entity, name);
}

const char *pixio_get_name(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_name(world, entity);
}

ecs_entity_t pixio_find_by_name(ecs_world_t *world, const char *name)
{
    return ecs_lookup_path_w_sep(world, pixio_get_root(world), name, ".", ".", true);
}

void pixio_set_enabled(ecs_world_t *world, ecs_entity_t entity, bool enabled)
{
    ecs_enable(world, entity, enabled);
}

bool pixio_get_enabled(ecs_world_t *world, ecs_entity_t entity)
{
    return !ecs_has_id(world, entity, EcsDisabled);
}

void pixio_set_component_by_name(ecs_world_t *world, ecs_entity_t entity, const char *component)
{
    ecs_add_id(world, entity, ecs_lookup(world, component));
}
