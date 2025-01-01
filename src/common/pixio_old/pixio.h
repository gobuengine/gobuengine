#pragma once
#ifndef PIXIO_H
#define PIXIO_H

#include <stdint.h>
#include <glib.h>
#include "../pixi/pixi.h"
#include "flecs.h"
#include "pixio_type.h"

#define PIXIO_ENTITY_ROOT_NAME "Root"

ecs_world_t *pixio_world_init(void);
void pixio_free(ecs_world_t *ecs);
void pixio_world_process(ecs_world_t *ecs, float deltaTime);
char *pixio_world_serialize(ecs_world_t *world);
bool pixio_world_deserialize(ecs_world_t *world, const char *json);
bool pixio_world_deserialize_filename(ecs_world_t *world, const char *filename);

// ---------------------
// SceneEntity
// ---------------------
ecs_entity_t pixio_scene_new(ecs_world_t *world, const char *name);
ecs_entity_t pixio_scene_clone(ecs_world_t *world, ecs_entity_t entity);
void pixio_scene_open(ecs_world_t *world, ecs_entity_t entity);
void pixio_scene_delete(ecs_world_t *world, ecs_entity_t entity);
ecs_entity_t pixio_scene_get_open(ecs_world_t *world);
void pixio_scene_reload(ecs_world_t *world);
ecs_entity_t pixio_scene_get_by_name(ecs_world_t *world, const char *name);
int pixio_scene_count(ecs_world_t *world);
void pixio_scene_rename(ecs_world_t *world, ecs_entity_t entity, const char *name);

// ---------------------
// EventEntity
// ---------------------
ecs_entity_t pixio_observer(ecs_world_t *world, ecs_entity_t event, ecs_iter_action_t callback, void *ctx);
void pixio_emit(ecs_world_t *world, ecs_entity_t event, ecs_entity_t entity);

// ---------------------
// Entity
// ---------------------
ecs_entity_t pixio_entity_new_low(ecs_world_t *world, ecs_entity_t parent);
ecs_entity_t pixio_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name);
void pixio_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent);
ecs_entity_t pixio_get_parent(ecs_world_t *world, ecs_entity_t entity);
bool pixio_has_parent(ecs_world_t *world, ecs_entity_t entity);
ecs_entity_t pixio_clone(ecs_world_t *world, ecs_entity_t entity);
void pixio_set_name(ecs_world_t *world, ecs_entity_t entity, const char *name);
const char *pixio_get_name(ecs_world_t *world, ecs_entity_t entity);
ecs_entity_t pixio_find_by_name(ecs_world_t *world, const char *name);
void pixio_enable(ecs_world_t *world, ecs_entity_t entity, bool enabled);
bool pixio_is_enabled(ecs_world_t *world, ecs_entity_t entity);
bool pixio_is_alive(ecs_world_t *world, ecs_entity_t entity);
void pixio_delete(ecs_world_t *world, ecs_entity_t entity);
void pixio_set_component_by_name(ecs_world_t *world, ecs_entity_t entity, const char *component);

#endif // PIXIO_H
