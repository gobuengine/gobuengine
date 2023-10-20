#include "bugo_draw_shape_rect.h"

static ecs_entity_t ecs_shape_rect_t;
static ecs_entity_t sys_shape_rect_t;

static void
bugo_system_on_draw(ecs_iter_t* it)
{
  ComponentRenderer* r = ecs_field(it, ComponentRenderer, 1);
  ComponentShapeRectangle* rect = ecs_field(it, ComponentShapeRectangle, 2);

  for (int i = 0; i < it->count; i++) {
    Vector2 scale = r[i].scale;
    float rotation = r[i].rotation;
    float x = r[i].position.x;
    float y = r[i].position.y;

    float w = rect[i].width;
    float h = rect[i].height;
    float ox = w * r[i].origin.x;
    float oy = h * r[i].origin.y;
    Color color = rect[i].color;

    bugo_draw_shape_rect(
      (Rectangle){ x, y, w, h }, (Vector2){ ox, oy }, rotation, color);
  }
}

void
bugo_ecs_init_shape_rect(ecs_world_t* world)
{
  ecs_shape_rect_t = ecs_component_init(
    world,
    &(ecs_component_desc_t){
      .entity = ecs_entity(world, { .name = "ComponentShapeRectangle" }),
      .type.size = ECS_SIZEOF(ComponentShapeRectangle),
      .type.alignment = ECS_ALIGNOF(ComponentShapeRectangle),
    });

  ecs_struct(world, {
        .entity = ecs_shape_rect_t,
        .members = {
            { .name = "width", .type = ecs_id(ecs_f32_t)},
            { .name = "height", .type = ecs_id(ecs_f32_t)},
            { .name = "color", .type = bugo_ecs_get_color_id() },
        },
    });

  sys_shape_rect_t = ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = {ecs_dependson(bugo_ecs_get_phaser(PHASER_BEGIN_CAMERA))},
        }),
        .query.filter.terms = {
            { .id = bugo_ecs_get_renderer_id() },
            { .id = ecs_shape_rect_t, .inout = EcsIn},
        },
        .callback = bugo_system_on_draw,
    });
}

void
bugo_ecs_set_shape_rect(ecs_entity_t entity, ComponentShapeRectangle* props)
{
  ecs_set_id(bugo_ecs_world(),
             entity,
             ecs_shape_rect_t,
             sizeof(ComponentRenderer),
             props);
}

ecs_entity_t
bugo_ecs_get_shape_rect_id(void)
{
  return ecs_shape_rect_t;
}
