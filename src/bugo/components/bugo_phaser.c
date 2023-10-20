#include "bugo_renderer.h"

static ecs_entity_t cphaser[PHASER_NUMS];

static void
handler_PHASER_BEGIN_DRAWING(ecs_iter_t* it)
{
  bugo_gfx_render_begin(GetScreenWidth(), GetScreenHeight());
  bugo_gfx_render_clear_color((Color){28, 28, 28, 255});
}

static void
handler_PHASER_BEGIN_CAMERA(ecs_iter_t* it)
{
  ecs_entity_t camera_t = bugo_ecs_get_camera2d_id();
  // const Camera2D *camera = (ECS_CAST(const Camera2D*, ecs_get_id(it->world, camera_t, camera_t)));
  // BeginMode2D(camera[0]);
  DrawGrid(200, 24);
}

static void
handler_PHASER_END_CAMERA(ecs_iter_t* it)
{
  // EndMode2D();
}

static void
handler_PHASER_END_DRAWING(ecs_iter_t* it)
{
  bugo_gfx_render_end(GetScreenWidth(), GetScreenHeight());
}

void
bugo_ecs_init_phaser(ecs_world_t* world)
{
  cphaser[PHASER_PHYSICS] = ecs_new_w_id(world, EcsPhase);
  cphaser[PHASER_UPDATE] = ecs_new_w_id(world, EcsPhase);

  cphaser[PHASER_BEGIN_DRAWING] = ecs_new_w_id(world, EcsPhase);
  cphaser[PHASER_BEGIN_CAMERA] = ecs_new_w_id(world, EcsPhase);
  cphaser[PHASER_END_CAMERA] = ecs_new_w_id(world, EcsPhase);
  cphaser[PHASER_UI] = ecs_new_w_id(world, EcsPhase);
  cphaser[PHASER_END_DRAWING] = ecs_new_w_id(world, EcsPhase);

  ecs_add_pair(world, cphaser[PHASER_PHYSICS], EcsDependsOn, EcsOnUpdate);
  ecs_add_pair(world, cphaser[PHASER_UPDATE], EcsDependsOn, cphaser[PHASER_PHYSICS]);

  ecs_add_pair(world, cphaser[PHASER_BEGIN_DRAWING], EcsDependsOn, EcsOnStore);
  ecs_add_pair(world, cphaser[PHASER_BEGIN_CAMERA], EcsDependsOn, cphaser[PHASER_BEGIN_DRAWING]);
  ecs_add_pair(world, cphaser[PHASER_END_CAMERA], EcsDependsOn, cphaser[PHASER_BEGIN_CAMERA]);
  ecs_add_pair(world, cphaser[PHASER_UI], EcsDependsOn, cphaser[PHASER_END_CAMERA]);
  ecs_add_pair(world, cphaser[PHASER_END_DRAWING], EcsDependsOn, cphaser[PHASER_UI]);

  ECS_SYSTEM(world, handler_PHASER_BEGIN_DRAWING, cphaser[PHASER_BEGIN_DRAWING], 0);
  ECS_SYSTEM(world, handler_PHASER_BEGIN_CAMERA, cphaser[PHASER_BEGIN_CAMERA], 0);
  ECS_SYSTEM(world, handler_PHASER_END_CAMERA, cphaser[PHASER_END_CAMERA], 0);
  ECS_SYSTEM(world, handler_PHASER_END_DRAWING, cphaser[PHASER_END_DRAWING], 0);
}

ecs_entity_t
bugo_ecs_get_phaser(BugoEcsPhaser phaser)
{
  return cphaser[phaser];
}
