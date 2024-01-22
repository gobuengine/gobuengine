#include "gb_type_camera.h"

ECS_COMPONENT_DECLARE(eCameraMode);
ECS_COMPONENT_DECLARE(gb_camera_t);

void gb_type_camera_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, gb_type_camera_module);
    ECS_COMPONENT_DEFINE(world, eCameraMode);
    ECS_COMPONENT_DEFINE(world, gb_camera_t);

    ECS_IMPORT(world, gb_type_vec2_module);

    ecs_enum(world, {.entity = ecs_id(eCameraMode),
                     .constants = {
                         {.name = "GB_CAMERA_NONE", .value = GB_CAMERA_NONE},
                         {.name = "GB_CAMERA_EDITOR", .value = GB_CAMERA_EDITOR},
                         {.name = "GB_CAMERA_FOLLOWING", .value = GB_CAMERA_FOLLOWING}}});

    ecs_struct(world, {.entity = ecs_id(gb_camera_t),
                       .members = {
                           {.name = "offset", .type = ecs_id(gb_vec2_t)},
                           {.name = "target", .type = ecs_id(gb_vec2_t)},
                           {.name = "rotation", .type = ecs_id(ecs_f32_t)},
                           {.name = "zoom", .type = ecs_id(ecs_f32_t)},
                           {.name = "mode", .type = ecs_id(eCameraMode)},
                       }});
}
