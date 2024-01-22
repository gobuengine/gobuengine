#include "gb_ecs.h"

ecs_entity_t gb_ecs_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name, const gb_transform_t t)
{
    ecs_entity_t entity = ecs_new_id(world);
    if (parent != 0)
        ecs_set_parent(world, parent, entity);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, parent, name, ".", ".", true);
    char *name_id = (lookup == 0 ? name : gb_strdups("%s%lld", name, entity));
    ecs_set_name(world, entity, name_id);

    ecs_set(world, entity, gb_transform_t, {.position = t.position, .scale = t.scale, .rotation = t.rotation, .origin = t.origin});
    ecs_set(world, entity, gb_bounding_t, {0});
    ecs_set(world, entity, gb_gizmos_t, {.selected = false});

    return entity;
}

void ecs_set_parent(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t entity)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}

ecs_entity_t gb_ecs_entity_get_parent(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t parent = ecs_get_parent(world, entity);
    return parent;
}

const char *gb_ecs_entity_get_name(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_name(world, entity);
}

ecs_entity_t gb_ecs_set_name(ecs_world_t *world, ecs_entity_t entity, const char *name)
{
    return ecs_set_name(world, entity, gb_strdup(name));
}

void gb_ecs_vec_remove(ecs_vec_t *v, ecs_size_t size, int32_t index)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index < v->count, ECS_OUT_OF_RANGE, NULL);
    if (index == --v->count)
    {
        return;
    }

    for (int32_t i = index; i < v->count; i++)
    {
        ecs_os_memcpy(
            ECS_ELEM(v->array, size, i),
            ECS_ELEM(v->array, size, i + 1),
            size);
    }
}

void gb_ecs_vec_swap(ecs_vec_t *v, ecs_size_t size, int32_t index_a, int32_t index_b)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index_a < v->count, ECS_OUT_OF_RANGE, NULL);
    ecs_assert(index_b < v->count, ECS_OUT_OF_RANGE, NULL);

    if (index_a == index_b)
    {
        return;
    }

    void *buffer = ecs_os_malloc(size);
    ecs_os_memcpy(buffer, ECS_ELEM(v->array, size, index_a), size);
    ecs_os_memcpy(ECS_ELEM(v->array, size, index_a), ECS_ELEM(v->array, size, index_b), size);
    ecs_os_memcpy(ECS_ELEM(v->array, size, index_b), buffer, size);
    ecs_os_free(buffer);
}
