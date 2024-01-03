#include "gobu.h"
#include "config.h"

#include "gb_ecs_rendering.h"
#include "gb_ecs_resource.h"
#include "gb_ecs_bounding.h"
#include "gb_ecs_camera.h"
#include "gb_ecs_gizmos.h"
#include "gb_ecs_text.h"
#include "gb_ecs_sprite.h"

ecs_world_t* gb_init(gb_app_t* app)
{
    ecs_world_t* world = ecs_init();

    ECS_IMPORT(world, gb_type_transform_module);
    ECS_IMPORT(world, gb_type_bounding_module);
    ECS_IMPORT(world, gb_type_shape_circle_module);
    ECS_IMPORT(world, gb_type_shape_rect_module);
    ECS_IMPORT(world, gb_type_color_module);
    ECS_IMPORT(world, gb_type_texture_module);
    ECS_IMPORT(world, gb_type_gizmos_module);
    ECS_IMPORT(world, gb_type_camera_module);
    ECS_IMPORT(world, gb_type_sprite_module);
    ECS_IMPORT(world, gb_type_text_module);

    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    ecs_entity_t Engine = ecs_new_entity(world, "Engine");
    ecs_set(world, Engine, gb_app_t, {
        .title = app->title,         // Establece el título de la ventana
        .width = app->width,         // Establece el ancho de la ventana
        .height = app->height,       // Establece la altura de la ventana
        .fps = app->fps,             // Establece los cuadros por segundo
        .show_fps = app->show_fps,   // Establece si se muestra los FPS
        .show_grid = app->show_grid  // Establece si se muestra la cuadrícula
    });
    ecs_set(world, Engine, gb_camera_t, { .zoom = 1.0f, .rotation = 0.0f });
    SetTargetFPS(app->fps ? app->fps : 60);

    return world;
}

void gb_app_main(ecs_world_t* world)
{
    while (ecs_progress(world, GetFrameTime())) {}
}

void gb_app_progress(ecs_world_t* world)
{
    ecs_progress(world, GetFrameTime());
}


