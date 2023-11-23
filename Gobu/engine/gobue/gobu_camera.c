#include "gobu_camera.h"
#include "gobu_input.h"
#include "gobu_custom.h"
#include "raygo/rlgl.h"

ECS_COMPONENT_DECLARE(GCamera);
ECS_COMPONENT_DECLARE(enumCameraMode);
ECS_COMPONENT_DECLARE(Camera2D);

static void GobuCamera_Update(ecs_iter_t* it);

void GobuCameraImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuCamera);
    ECS_IMPORT(world, GobuCustom);

    ECS_COMPONENT_DEFINE(world, GCamera);
    ECS_COMPONENT_DEFINE(world, enumCameraMode);
    ECS_COMPONENT_DEFINE(world, Camera2D);

    ECS_SYSTEM(world, GobuCamera_Update, EcsOnUpdate, GCamera);

    ecs_enum(world, {
        .entity = ecs_id(enumCameraMode),
        .constants = {
            {.name = "NONE", .value = CAMERA_NONE },
            {.name = "EDITOR", .value = CAMERA_EDITOR },
            {.name = "FOLLOWING", .value = CAMERA_FOLLOWING },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Camera2D),
        .members = {
            {.name = "offset", .type = ecs_id(ecs_vec2_t) },
            {.name = "target", .type = ecs_id(ecs_vec2_t) },
            {.name = "rotation", .type = ecs_id(ecs_f64_t) },
            {.name = "zoom", .type = ecs_id(ecs_f64_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(GCamera),
        .members = {
            {.name = "camera", .type = ecs_id(Camera2D) },
            {.name = "mode", .type = ecs_id(enumCameraMode) },
        }
    });
}

static void GobuCamera_Update(ecs_iter_t* it)
{
    GCamera* camera = ecs_field(it, GCamera, 1);
    GInputSystem* input = ecs_singleton_get(it->world, GInputSystem);

    for (int i = 0; i < it->count; i++)
    {
        if (camera[i].mode == CAMERA_FOLLOWING)
        {
        }
        else if (camera[i].mode == CAMERA_EDITOR)
        {
            // move camera mouse movement
            if (input->mouse.button_down(MOUSE_BUTTON_RIGHT))
            {
                Vector2 delta = input->mouse.get_delta();
                camera[i].camera.target.x -= delta.x / camera[i].camera.zoom;
                camera[i].camera.target.y -= delta.y / camera[i].camera.zoom;
            }

            // zoom with mouse wheel
            float wheel = input->mouse.get_wheel();
            if (wheel != 0)
            {
                Vector2 mouseWorld = input->mouse.get_screen_to_world(camera[i]);

                camera[i].camera.offset = input->mouse.get_position();
                camera[i].camera.target = mouseWorld;
                camera[i].camera.zoom += wheel * 0.05f;
                if (camera[i].camera.zoom < 0.1f) camera[i].camera.zoom = 0.1f;
            }
        }
    }
}

