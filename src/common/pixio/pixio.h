#pragma once
#ifndef PIXIO_H
#define PIXIO_H

#include <stdint.h>
#include <glib.h>
#include "../pixi/pixi.h"
#include "flecs.h"
#include "pixio_type.h"

typedef struct pixio_entity
{
    ecs_world_t *world;
    ecs_entity_t entity;
} pixio_entity;

ecs_world_t *pixio_world_init(void);
void pixio_world_process(ecs_world_t *ecs, float deltaTime);

pixio_entity *pixio_entity_new(ecs_world_t *world, ecs_entity_t entity);
void pixio_entity_free(pixio_entity *pixio_entity);

ecs_entity_t pixio_new(ecs_world_t *world, ecs_entity_t parent, const char *name);

ecs_entity_t pixio_new_root(ecs_world_t *world);
ecs_entity_t pixio_get_root(ecs_world_t *world);
ecs_entity_t pixio_find_by_name(ecs_world_t *world, const char *name);

void pixio_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent);
ecs_entity_t pixio_get_parent(ecs_world_t *world, ecs_entity_t entity);
bool pixio_has_parent(ecs_world_t *world, ecs_entity_t entity);

ecs_entity_t pixio_clone(ecs_world_t *world, ecs_entity_t entity);

bool pixio_set_name(ecs_world_t *world, ecs_entity_t entity, const char *name);
const char *pixio_get_name(ecs_world_t *world, ecs_entity_t entity);

void pixio_set_enabled(ecs_world_t *world, ecs_entity_t entity, bool enabled);
bool pixio_get_enabled(ecs_world_t *world, ecs_entity_t entity);

#endif // PIXIO_H
