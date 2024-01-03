#include "gb_ecs_rendering.h"

#include "gb_type_transform.h"
#include "gb_type_rect.h"
#include "gb_type_vec2.h"
#include "gb_type_color.h"
#include "gb_type_texture.h"
#include "gb_type_shape_circle.h"
#include "gb_type_shape_rect.h"

#include "gb_ecs_resource.h"
#include "gb_ecs_bounding.h"
#include "gb_ecs_camera.h"
#include "gb_ecs_gizmos.h"
#include "gb_ecs_text.h"
#include "gb_ecs_sprite.h"

ECS_COMPONENT_DECLARE(gb_render_phases_t);
ECS_COMPONENT_DECLARE(gb_app_t);

static void observer_set_gb_app_t(ecs_iter_t *it);
static void predraw_begin_rendering(ecs_iter_t *it);
static void drawing_rendering(ecs_iter_t *it);
static void postdraw_end_rendering(ecs_iter_t *it);

static gb_render_phases_t render_phases;

void gb_rendering_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, gb_rendering_module);
    ECS_COMPONENT_DEFINE(world, gb_render_phases_t);
    ECS_COMPONENT_DEFINE(world, gb_app_t);

    ECS_IMPORT(world, gb_type_transform_module);
    ECS_IMPORT(world, gb_type_bounding_module);
    ECS_IMPORT(world, gb_type_shape_circle_module);
    ECS_IMPORT(world, gb_type_shape_rect_module);

    ECS_IMPORT(world, gb_resource_module);
    ECS_IMPORT(world, gb_camera_module);
    ECS_IMPORT(world, gb_gizmos_module);
    ECS_IMPORT(world, gb_sprite_module);
    ECS_IMPORT(world, gb_text_module);

    render_phases.PreDraw = ecs_new_w_id(world, EcsPhase);
    render_phases.Background = ecs_new_w_id(world, EcsPhase);
    render_phases.Draw = ecs_new_w_id(world, EcsPhase);
    render_phases.PostDraw = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, render_phases.PreDraw, EcsDependsOn, EcsOnStore);
    ecs_add_pair(world, render_phases.Background, EcsDependsOn, render_phases.PreDraw);
    ecs_add_pair(world, render_phases.Draw, EcsDependsOn, render_phases.Background);
    ecs_add_pair(world, render_phases.PostDraw, EcsDependsOn, render_phases.Draw);

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_app_t)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = observer_set_gb_app_t
    });

    ecs_system(world, {.entity = ecs_entity(world, {.add = {ecs_dependson(render_phases.PreDraw)}}),
        .query.filter.terms = {
            {.id = ecs_id(gb_app_t)},
            {.id = ecs_id(gb_camera_t)},
        },
        .callback = predraw_begin_rendering
    });

    ecs_system(world, {.entity = ecs_entity(world, {.add = {ecs_dependson(render_phases.Draw)}}),
        .query.filter.terms = {
            {.id = ecs_id(gb_transform_t)},
            {.id = ecs_id(gb_bounding_t)},
            {.id = ecs_id(gb_gizmos_t), .oper = EcsOptional},
            {.id = ecs_id(gb_shape_rect_t), .oper = EcsOptional},
            {.id = ecs_id(gb_sprite_t), .oper = EcsOptional},
            {.id = ecs_id(gb_text_t), .oper = EcsOptional},
        },
        .callback = drawing_rendering
    });

    ecs_system(world, {.entity = ecs_entity(world,{.add = {ecs_dependson(render_phases.PostDraw)}}),
        .query.filter.terms = {
            {.id = ecs_id(gb_app_t)},
        },
        .callback = postdraw_end_rendering
    });
}

// --
// :EVENTS
// --

static void observer_set_gb_app_t(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    gb_app_t* win = ecs_field(it, gb_app_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (event == EcsOnSet)
            InitWindow(win->width, win->height, win->title);
        else if (event == EcsOnRemove)
            CloseWindow();
    }
}

static void predraw_begin_rendering(ecs_iter_t *it)
{
    gb_app_t *win = ecs_field(it, gb_app_t, 1);
    gb_camera_t *camera = ecs_field(it, gb_camera_t, 2);

    for (int i = 0; i < it->count; i++)
    {
        Camera2D cam = {
            .offset = camera[i].offset,
            .target = camera[i].target,
            .rotation = camera[i].rotation,
            .zoom = camera[i].zoom,
        };

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(cam);

        if (win[i].show_grid)
            gb_gfx_draw_grid_2d(win[i].width, 48, (gb_color_t){10, 10, 10, 255});
    }
}

static void drawing_rendering(ecs_iter_t *it)
{
    gb_transform_t *transform = ecs_field(it, gb_transform_t, 1);
    gb_bounding_t *bounding = ecs_field(it, gb_bounding_t, 2);

    // Drawing
    gb_gizmos_t *gismos = ecs_field(it, gb_gizmos_t, 3);
    gb_shape_rect_t *rect = ecs_field(it, gb_shape_rect_t, 4);
    gb_sprite_t *sprite = ecs_field(it, gb_sprite_t, 5);
    gb_text_t *text = ecs_field(it, gb_text_t, 6);

    for (int i = 0; i < it->count; i++)
    {
        gb_transform_t trans = transform[i];

        gb_gfx_push_matrix();
        {
            gb_gfx_translate(trans.position.x, trans.position.y, 0.0f);
            gb_gfx_rotate(trans.rotation, 0.0f, 0.0f, 1.0f);
            gb_gfx_translate(-(trans.origin.x * bounding[i].size.x), -(trans.origin.y * bounding[i].size.y), 0.0f);
            gb_gfx_scale(trans.scale.x, trans.scale.y, 1.0f);

            if (rect)
            {
                gb_gfx_draw_rect(rect[i]);
            }

            if (sprite)
            {
                if (sprite[i].texture.id != 0)
                    gb_gfx_draw_sprite(sprite[i]);
            }

            if (text)
            {
                if (text[i].font.texture.id != 0)
                    gb_gfx_draw_text(text[i]);
            }

            if (gismos)
            {
                if (gismos[i].selected)
                    gb_gfx_draw_gismos(trans, bounding[i]);
            }
        }
        gb_gfx_pop_matrix();
    }
}

static void postdraw_end_rendering(ecs_iter_t *it)
{
    gb_app_t *win = ecs_field(it, gb_app_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (win[i].show_fps)
            DrawFPS(10, 20);

        EndMode2D();
        EndDrawing();

        if (WindowShouldClose())
        {
            ecs_quit(it->world);
        }
    }
}

// --
// :API
// --
