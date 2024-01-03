#include "gb_ecs_camera.h"
#include "gb_input.h"

static void observer_set_gb_camera_t(ecs_iter_t* it);
static void update_gb_camera_t(ecs_iter_t* it);

void gb_camera_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_camera_module);
    ECS_IMPORT(world, gb_type_camera_module);

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_camera_t)}}},
        .events = { EcsOnSet },
        .callback = observer_set_gb_camera_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_camera_t)} },
        .callback = update_gb_camera_t
    });
}

// -- 
// gb_sprite_t:EVENTS
// --  
static void observer_set_gb_camera_t(ecs_iter_t* it)
{
    gb_camera_t* camera = ecs_field(it, gb_camera_t, 1);
    for (int i = 0; i < it->count; i++)
    {
        camera[i].zoom = (camera[i].zoom == 0) ? 1.0f : camera[i].zoom;
    }
}

static void update_gb_camera_t(ecs_iter_t* it)
{
    gb_camera_t* camera = ecs_field(it, gb_camera_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (camera[i].mode == GB_CAMERA_FOLLOWING)
        {
        }
        else if (camera[i].mode == GB_CAMERA_EDITOR)
        {
            // move camera mouse movement
            if (input_is_mouse_button_down(MOUSE_BUTTON_RIGHT))
            {
                gb_vec2_t delta = input_mouse_delta();
                camera[i].target.x -= delta.x / camera[i].zoom;
                camera[i].target.y -= delta.y / camera[i].zoom;
            }

            // zoom with mouse wheel
            float wheel = input_mouse_wheel();
            if (wheel != 0)
            {
                gb_vec2_t mouseWorld = screen_to_world(camera[i], input_mouse_position());

                camera[i].offset = input_mouse_position();
                camera[i].target = mouseWorld;
                camera[i].zoom -= wheel * 0.05f;
                if (camera[i].zoom < 0.1f) camera[i].zoom = 0.1f;
            }
        }
    }
}

// -- 
// gb_sprite_t:API
// -- 

