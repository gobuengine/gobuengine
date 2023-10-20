#include "bugo_bcamera2d.h"

static ecs_entity_t ecs_camera2d_t;
static ecs_entity_t enum_camera2d_t;
static ecs_entity_t sys_camera2d_t;

static void
bugo_system_on_update(ecs_iter_t* it)
{
    ComponentCamera* camera = ecs_field(it, ComponentCamera, 1);

    for (int i = 0; i < it->count; i++) {
        if (camera[i].mode == CAMERA_MODE_EDITOR) {
            if (bugo_mouse_button_is_down(MOUSE_BUTTON_RIGHT))
            {
                Vector2 delta = bugo_mouse_get_delta();
                delta = bugo_math_vector2_scale(delta, -1.0f / camera[i].zoom);
                camera[i].target = bugo_math_vector2_add(camera[i].target, delta);
            }

            float wheel = bugo_mouse_get_wheel_move();
            if (wheel != 0)
            {
                Vector2 mouseWorldPos = bugo_screen_to_world_2d(bugo_mouse_get_position(), camera[i]);
                camera[i].offset = bugo_mouse_get_position();
                camera[i].target = mouseWorldPos;
                const float zoomIncrement = 0.125f;
                camera[i].zoom += (wheel * zoomIncrement);
                if (camera[i].zoom < zoomIncrement) camera[i].zoom = zoomIncrement;
            }
        }
    }
}

void bugo_ecs_init_camera2d(ecs_world_t* world)
{
    ecs_camera2d_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, { .name = "ComponentCamera" }),
            .type.size = ECS_SIZEOF(ComponentCamera),
            .type.alignment = ECS_ALIGNOF(ComponentCamera),
    });

    enum_camera2d_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, { .name = "ComponentCameraMode" }),
            .type.size = ECS_SIZEOF(ComponentCameraMode),
            .type.alignment = ECS_ALIGNOF(ComponentCameraMode),
    });

    ecs_enum(world, {
        .entity = enum_camera2d_t,
        .constants = {
            {.name = "EDITOR", .value = CAMERA_MODE_EDITOR},
            {.name = "FOLLOW", .value = CAMERA_MODE_FOLLOW},
            {.name = "SMOOTH_FOLLOW", .value = CAMERA_MODE_SMOOTH_FOLLOW},
        }
    });

    ecs_struct(world, {
        .entity = ecs_camera2d_t,
        .members = {
            {.name = "offset", .type = bugo_ecs_get_vector2_id() },
            {.name = "target", .type = bugo_ecs_get_vector2_id() },
            {.name = "rotation", .type = ecs_id(ecs_byte_t) },
            {.name = "zoom", .type = ecs_id(ecs_byte_t) },
            {.name = "mode", .type = enum_camera2d_t },
        },
    });

    sys_camera2d_t = ecs_system(world, {
          .entity = ecs_entity(world, {
              .add = {ecs_dependson(bugo_ecs_get_phaser(PHASER_UPDATE))},
          }),
          .query.filter.terms = {
              {.id = ecs_camera2d_t, .inout = EcsIn},
          },
          .callback = bugo_system_on_update,
      });

    ecs_set_id(bugo_ecs_world(), ecs_camera2d_t, ecs_camera2d_t, sizeof(ComponentCamera),
    &(ComponentCamera){.zoom = 1.0f, .rotation = 0.0f, .target = bugo_math_vector2_one(),
     .offset = bugo_math_vector2_one(), .mode = CAMERA_MODE_NONE});
}

void bugo_ecs_set_camera2d(ecs_entity_t entity, ComponentCamera* props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_camera2d_t, sizeof(ComponentCamera), props);
}

ecs_entity_t bugo_ecs_get_camera2d_id(void)
{
    return ecs_camera2d_t;
}
