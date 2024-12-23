#include "pixio.h"
#include "pixio_type.h"
#include "pixio_render.h"

// CONFIGS
static ecs_entity_to_json_desc_t desc_entity_json_init = ECS_ENTITY_TO_JSON_INIT;

// World
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

bool pixio_world_deserialize(ecs_world_t *world, const char *json)
{
    return ecs_world_from_json(world, json, NULL) == NULL ? false : true;
}

bool pixio_world_deserialize_filename(ecs_world_t *world, const char *filename)
{
    return ecs_world_from_json_file(world, filename, NULL) == NULL ? false : true;
}

// ---------------------
// SceneEntity
// ---------------------
ecs_entity_t pixio_scene_new(ecs_world_t *world, const char *name)
{
    const gchar *new_name = g_strdup_printf("pixioScene.%s", name);
    if (ecs_lookup(world, new_name) > 0)
        return 0;
 
    ecs_entity_t scene = ecs_entity(world, {.name = g_strdup(new_name), .add = ecs_ids(EcsPixioTagScene)});
    pixio_emit(world, EcsPixioOnCreateScene, scene);

    return scene;
}

void pixio_scene_open(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_query_t *q = ecs_query(world, { .terms = {{.id = ecs_id(EcsPixioTagScene)}} });
    ecs_iter_t it = ecs_query_iter(world, q);

    while (ecs_query_next(&it))
    {
        for (int i = 0; i < it.count; i++)
        {
            ecs_entity_t en1 = it.entities[i];
            ecs_enable(world, en1, FALSE);
        }
    }
    ecs_query_fini(q);

    ecs_enable(world, entity, TRUE);
    pixio_emit(world, EcsPixioOnOpenScene, entity);
}

ecs_entity_t pixio_scene_get_open(ecs_world_t *world)
{
    ecs_entity_t scene_active = 0;

    ecs_query_t *q = ecs_query(world, { .terms = {{.id = ecs_id(EcsPixioTagScene)}} });
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it))
    {
        for (int i = 0; i < it.count; i++)
        {
            scene_active = it.entities[i];
        }
    }
    ecs_query_fini(q);
    return scene_active;
}

void pixio_scene_reload(ecs_world_t *world)
{
    ecs_entity_t scene_active = pixio_scene_get_open(world);
    if (scene_active > 0)
    {
        pixio_scene_open(world, scene_active);
    }
}

ecs_entity_t pixio_scene_get(ecs_world_t *world, const char *name)
{
    return 0;
}

// ---------------------
// EventEntity
// ---------------------
void pixio_observer(ecs_world_t *world, ecs_entity_t event, ecs_iter_action_t callback, void *ctx)
{
    ecs_observer(world, {
        .query.terms = {{.id = EcsAny}},
        .events = {event},
        .callback = callback,
        .ctx = ctx,
    });
}

void pixio_emit(ecs_world_t *world, ecs_entity_t event, ecs_entity_t entity)
{
    ecs_emit(world, &(ecs_event_desc_t){.entity = entity, .event = event});
}

// ---------------------
// Entity
// ---------------------

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
    g_autofree gchar *name_entity = g_strcmp0(name, PIXIO_ENTITY_ROOT_NAME) != 0 ? g_strdup_printf("%s%ld", name, entity) : g_strdup(name);
    pixio_set_name(world, entity, name_entity);
    ecs_set(world, entity, pixio_transform_t, {.position = {0, 0}, .scale = {1, 1}, .rotation = 0, .origin = PIXIO_CENTER});

    return entity;
}

void pixio_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}

ecs_entity_t pixio_get_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_target(world, entity, EcsChildOf, 0);
}

bool pixio_has_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_has_id(world, entity, ecs_pair(EcsChildOf, EcsWildcard));
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
    return 0;//ecs_lookup_path_w_sep(world, pixio_get_root(world), name, ".", ".", true);
}

void pixio_set_enabled(ecs_world_t *world, ecs_entity_t entity, bool enabled)
{
    ecs_enable(world, entity, enabled);
}

bool pixio_get_enabled(ecs_world_t *world, ecs_entity_t entity)
{
    return !ecs_has_id(world, entity, EcsDisabled);
}

bool pixio_is_alive(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_is_alive(world, entity);
}

void pixio_delete(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_delete(world, entity);
}

void pixio_set_component_by_name(ecs_world_t *world, ecs_entity_t entity, const char *component)
{
    ecs_add_id(world, entity, ecs_lookup(world, component));
}
