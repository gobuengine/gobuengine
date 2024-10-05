#pragma once
#ifndef PIXIO_H
#define PIXIO_H

#include <stdint.h>
#include <glib.h>
#include "../pixi/pixi.h"
#include "flecs.h"

#include "pixio_type.h"

ecs_world_t *pixio_init(void);

void pixio_process(ecs_world_t *ecs, float deltaTime);

ecs_entity_t pixio_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name);

void pixio_entity_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent);

#endif // PIXIO_H
