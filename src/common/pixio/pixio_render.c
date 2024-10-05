#define PIXI_RLGL_API
#include "pixio_render.h"
#include "pixio_type.h"
#include "pixio_text.h"

ECS_COMPONENT_DECLARE(pixio_render_phases_t);
ECS_COMPONENT_DECLARE(pixio_render_t);

static pixio_render_phases_t render_phases;

static void pixio_render_pre_draw(ecs_iter_t *it);
static void pixio_render_draw(ecs_iter_t *it);
static void pixio_render_post_draw(ecs_iter_t *it);

void pixio_rendering_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, pixio_rendering_module);
    ECS_COMPONENT_DEFINE(world, pixio_render_phases_t);
    ECS_COMPONENT_DEFINE(world, pixio_render_t);

    ECS_IMPORT(world, pixio_type_module);
    ECS_IMPORT(world, pixio_text_module);

    render_phases.PreDraw = ecs_new_w_id(world, EcsPhase);
    render_phases.Background = ecs_new_w_id(world, EcsPhase);
    render_phases.Draw = ecs_new_w_id(world, EcsPhase);
    render_phases.PostDraw = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, render_phases.PreDraw, EcsDependsOn, EcsOnStore);
    ecs_add_pair(world, render_phases.Background, EcsDependsOn, render_phases.PreDraw);
    ecs_add_pair(world, render_phases.Draw, EcsDependsOn, render_phases.Background);
    ecs_add_pair(world, render_phases.PostDraw, EcsDependsOn, render_phases.Draw);

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.PreDraw))}),
                       .query.terms = {{ecs_id(pixio_render_t)}},
                       .callback = pixio_render_pre_draw});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.Draw))}),
                       .query.terms = {{ecs_id(pixio_transform_t)}, {ecs_id(pixio_text_t), .oper = EcsOptional}},
                       .callback = pixio_render_draw});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.PostDraw))}),
                       .query.terms = {{ecs_id(pixio_render_t)}},
                       .callback = pixio_render_post_draw});
}

static void pixio_render_pre_draw(ecs_iter_t *it)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
}

static void pixio_render_draw(ecs_iter_t *it)
{
    pixio_transform_t *transform = ecs_field(it, pixio_transform_t, 0);
    pixio_text_t *draw_text = ecs_field(it, pixio_text_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        pixio_transform_t t = transform[i];

        rlPushMatrix();
        {
            rlTranslatef(t.position.x, t.position.y, 0.0f);
            rlRotatef(t.rotation, 0.0f, 0.0f, 1.0f);
            rlTranslatef(-t.origin.x, -t.origin.y, 0.0f);
            rlScalef(t.scale.x, t.scale.y, 1.0f);

            // draw textures

            // draw graphics

            // draw text
            if (draw_text)
            {
                DrawTextEx(draw_text[i].sresource, draw_text[i].text, (pixio_vector2_t){0, 0}, draw_text[i].fontSize, draw_text[i].spacing, draw_text[i].color);
            }
        }
        rlPopMatrix();
    }
}

static void pixio_render_post_draw(ecs_iter_t *it)
{
    EndDrawing();
}
