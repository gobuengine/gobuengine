#ifndef GB_ECS_H
#define GB_ECS_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"

#include "gobu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // global component

    // API::ECS
#define gb_ecs_vec_remove_t(vec, T, elem) \
    gb_ecs_vec_remove(vec, ECS_SIZEOF(T), elem)

#define gb_ecs_vec_swap_t(vec, T, elem, elem2) \
    gb_ecs_vec_swap(vec, ECS_SIZEOF(T), elem, elem2)

    ecs_entity_t gb_ecs_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name, const gb_transform_t t);

    void ecs_set_parent(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t entity);

    ecs_entity_t gb_ecs_entity_get_parent(ecs_world_t *world, ecs_entity_t entity);

    const char *gb_ecs_entity_get_name(ecs_world_t *world, ecs_entity_t entity);

    ecs_entity_t gb_ecs_set_name(ecs_world_t *world, ecs_entity_t entity, const char *name);

    void gb_ecs_vec_remove(ecs_vec_t *v, ecs_size_t size, int32_t index);

    void gb_ecs_vec_swap(ecs_vec_t *v, ecs_size_t size, int32_t index_a, int32_t index_b);

    // module import
    void gb_rendering_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // GB_ECS_H
