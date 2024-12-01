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
void pixio_world_deserialize(ecs_world_t *world, const char *json);

char *pixio_entity_stringify(ecs_world_t *world, ecs_entity_t entity);
void pixio_entity_parse(ecs_world_t *world, ecs_entity_t entity, const char *json);

ecs_entity_t pixio_entity_new_low(ecs_world_t *world, ecs_entity_t parent);
ecs_entity_t pixio_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name);
ecs_entity_t pixio_entity_new_root(ecs_world_t *world);

ecs_entity_t pixio_get_root(ecs_world_t *world);
ecs_entity_t pixio_find_by_name(ecs_world_t *world, const char *name);

void pixio_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent);
ecs_entity_t pixio_get_parent(ecs_world_t *world, ecs_entity_t entity);

bool pixio_has_parent(ecs_world_t *world, ecs_entity_t entity);
bool pixio_is_alive(ecs_world_t *world, ecs_entity_t entity);

void pixio_delete(ecs_world_t *world, ecs_entity_t entity);

ecs_entity_t pixio_clone(ecs_world_t *world, ecs_entity_t entity);

void pixio_set_name(ecs_world_t *world, ecs_entity_t entity, const char *name);
const char *pixio_get_name(ecs_world_t *world, ecs_entity_t entity);

void pixio_set_enabled(ecs_world_t *world, ecs_entity_t entity, bool enabled);
bool pixio_get_enabled(ecs_world_t *world, ecs_entity_t entity);

void pixio_set_component_by_name(ecs_world_t *world, ecs_entity_t entity, const char *component);

#endif // PIXIO_H
