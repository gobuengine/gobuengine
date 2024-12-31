#include "ecs.h"

ecs_world_t *gobu_ecs_init(void)
{
    ecs_world_t *ecs = ecs_init();
    return ecs;
}

void gobu_ecs_free(ecs_world_t *ecs)
{
    ecs_fini(ecs);
}

void gobu_ecs_process(ecs_world_t *ecs, float deltaTime)
{
    ecs_progress(ecs, deltaTime);
}

void gobu_ecs_save_to_file(ecs_world_t *world, const char *filename)
{
    gobu_util_write_text_file(filename, ecs_world_to_json(world));
}

bool gobu_ecs_load_from_file(ecs_world_t *world, const char *filename)
{
    return ecs_world_from_json_file(world, filename, NULL) == NULL ? false : true;
}

// enity
ecs_entity_t gobu_ecs_entity_new_low(ecs_world_t *world, ecs_entity_t parent)
{
    ecs_entity_t entity = ecs_new_low_id(world);
    if (parent > 0)
        gobu_ecs_set_parent(world, entity, parent);

    return entity;
}

ecs_entity_t gobu_ecs_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name)
{
    ecs_entity_t entity = gobu_ecs_entity_new_low(world, parent);
    g_autofree gchar *name_entity = g_strcmp0(name, PIXIO_ENTITY_ROOT_NAME) != 0 ? gobu_util_string_format("%s%ld", name, entity) : g_strdup(name);
    ecs_set_name(world, entity, name_entity);
    ecs_set(world, entity, pixio_transform_t, {.position = {0, 0}, .scale = {1, 1}, .rotation = 0, .origin = PIXIO_CENTER});

    return entity;
}

void gobu_ecs_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}

ecs_entity_t gobu_ecs_get_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_target(world, entity, EcsChildOf, 0);
}

bool gobu_ecs_has_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_has_id(world, entity, ecs_pair(EcsChildOf, EcsWildcard));
}

ecs_entity_t gobu_ecs_clone(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t clone = ecs_clone(world, 0, entity, TRUE);
    ecs_set_name(world, clone, gobu_util_string_format("%s%ld", ecs_get_name(world, entity), clone));
    gobu_ecs_set_parent(world, clone, gobu_ecs_get_parent(world, entity));

    ecs_iter_t it = ecs_children(world, entity);
    while (ecs_children_next(&it))
        for (int i = 0; i < it.count; i++)
            if (ecs_is_alive(world, it.entities[i]))
                gobu_ecs_set_parent(world, gobu_ecs_clone(world, it.entities[i]), clone);

    return clone;
}

bool gobu_ecs_is_enabled(ecs_world_t *world, ecs_entity_t entity)
{
    return !ecs_has_id(world, entity, EcsDisabled);
}

void gobu_ecs_add_component_name(ecs_world_t *world, ecs_entity_t entity, const char *component)
{
    ecs_add_id(world, entity, ecs_lookup(world, component));
}

// entity events
ecs_entity_t gobu_ecs_observer(ecs_world_t *world, ecs_entity_t event, ecs_iter_action_t callback, void *ctx)
{
    return ecs_observer(world, {
        .query.terms = {{.id = EcsAny}, {EcsDisabled, .oper = EcsOptional}},
        .events = {event},
        .callback = callback,
        .ctx = ctx,
    });
}

void gobu_ecs_emit(ecs_world_t *world, ecs_entity_t event, ecs_entity_t entity)
{
    ecs_emit(world, &(ecs_event_desc_t){.entity = entity, .event = event});
}

// entity scene

ecs_entity_t gobu_scene_new(ecs_world_t *world, const char *name)
{
    ecs_entity_t parent = ecs_lookup(world, "pixioScene");
    g_autofree gchar *new_name = gobu_util_string_format("%s", name);

    // Le agregamos un numero al final si ya existe
    int counter = 1;
    while (ecs_lookup_child(world, parent, new_name) > 0)
        new_name = gobu_util_string_format("%s%d", name, counter++);

    ecs_entity_t scene = ecs_entity(world, {.name = g_strdup(new_name), .add = ecs_ids(EcsPixioTagScene)});
    gobu_ecs_set_parent(world, scene, parent);
    ecs_enable(world, scene, FALSE);
    gobu_ecs_emit(world, EcsPixioOnCreateScene, scene);

    return scene;
}

ecs_entity_t gobu_scene_clone(ecs_world_t *world, ecs_entity_t entity)
{
    const char *orig_name = ecs_get_name(world, entity);

    g_autofree gchar *new_name = g_strdup(orig_name);
    ecs_entity_t parent = ecs_lookup(world, "pixioScene");

    // Le agregamos un numero al final si ya existe
    int counter = 1;
    while (ecs_lookup_child(world, parent, new_name) > 0)
        new_name = gobu_util_string_format("%s%d", orig_name, counter++);

    ecs_entity_t scene_clone = gobu_ecs_clone(world, entity);
    ecs_set_name(world, scene_clone, new_name);
    ecs_enable(world, scene_clone, FALSE);

    return scene_clone;
}

void gobu_scene_delete(ecs_world_t *world, ecs_entity_t entity)
{
    gobu_ecs_emit(world, EcsPixioOnDeleteScene, entity);
    ecs_delete(world, entity);
}

void gobu_scene_open(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t scene_active = pixio_scene_get_open(world);
    if (scene_active > 0)
        ecs_enable(world, scene_active, FALSE);

    ecs_enable(world, entity, TRUE);
    gobu_ecs_emit(world, EcsPixioOnOpenScene, entity);
}

ecs_entity_t gobu_scene_get_open(ecs_world_t *world)
{
    ecs_query_t *q = ecs_query(world, {.terms = {{EcsPixioTagScene}}});
    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_entity_t scene = ecs_iter_first(&it);
    ecs_query_fini(q);
    return scene;
}

void gobu_scene_reload(ecs_world_t *world)
{
    ecs_entity_t scene_active = pixio_scene_get_open(world);
    if (scene_active > 0)
    {
        pixio_scene_open(world, scene_active);
    }
}

int gobu_scene_count(ecs_world_t *world)
{
    return ecs_count(world, EcsPixioTagScene);
}

ecs_entity_t gobu_scene_get_by_name(ecs_world_t *world, const char *name)
{
    ecs_entity_t parent = ecs_lookup(world, "pixioScene");
    return ecs_lookup_child(world, parent, name);
}

void gobu_scene_rename(ecs_world_t *world, ecs_entity_t entity, const char *name)
{
    ecs_set_name(world, entity, name);
    gobu_ecs_emit(world, EcsPixioOnRenameScene, entity);
}
