#define PIXI_RLGL_API
#include "pixio_render.h"
#include "pixio_type.h"
#include "pixio_base.h"

ECS_COMPONENT_DECLARE(pixio_render_phases_t);
ECS_COMPONENT_DECLARE(pixio_render_t);

static pixio_render_phases_t render_phases;

static void pixio_render_pre_draw(ecs_iter_t *it);
static void pixio_render_draw(ecs_iter_t *it);
static void pixio_render_post_draw(ecs_iter_t *it);
static void pixio_calculate_bounding_box(ecs_iter_t *it);

void pixio_rendering_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, pixio_rendering_module);
    ECS_COMPONENT_DEFINE(world, pixio_render_phases_t);
    ECS_COMPONENT_DEFINE(world, pixio_render_t);

    ECS_IMPORT(world, pixio_type_module);
    ECS_IMPORT(world, pixio_base_module);

    render_phases.PreDraw = ecs_new_w_id(world, EcsPhase);
    render_phases.Background = ecs_new_w_id(world, EcsPhase);
    render_phases.Draw = ecs_new_w_id(world, EcsPhase);
    render_phases.PostDraw = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, render_phases.PreDraw, EcsDependsOn, EcsOnStore);
    ecs_add_pair(world, render_phases.Background, EcsDependsOn, render_phases.PreDraw);
    ecs_add_pair(world, render_phases.Draw, EcsDependsOn, render_phases.Background);
    ecs_add_pair(world, render_phases.PostDraw, EcsDependsOn, render_phases.Draw);

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
                       .query.terms = {{ecs_id(pixio_entity_t)}, {ecs_id(pixio_transform_t)}, {ecs_id(pixio_text_t), .oper = EcsOptional}, {ecs_id(pixio_shape_circle_t), .oper = EcsOptional}, {ecs_id(pixio_shape_rec_t), .oper = EcsOptional}, {ecs_id(pixio_sprite_t), .oper = EcsOptional}},
                       .callback = pixio_calculate_bounding_box});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.PreDraw))}),
                       .query.terms = {{ecs_id(pixio_render_t)}},
                       .callback = pixio_render_pre_draw});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.Draw))}),
                       .query.terms = {{ecs_id(pixio_entity_t)}, {ecs_id(pixio_transform_t)}, {ecs_id(pixio_text_t), .oper = EcsOptional}, {ecs_id(pixio_shape_circle_t), .oper = EcsOptional}, {ecs_id(pixio_shape_rec_t), .oper = EcsOptional}, {ecs_id(pixio_sprite_t), .oper = EcsOptional}},
                       .callback = pixio_render_draw});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.PostDraw))}),
                       .query.terms = {{ecs_id(pixio_render_t)}},
                       .callback = pixio_render_post_draw});
}

pixio_vector2_t calculate_origin(pixio_transform_origin_t alignment, float width, float height)
{
    pixio_vector2_t origin = {0.0f, 0.0f};

    switch (alignment)
    {
    case PIXIO_TOP_LEFT:
        origin.x = 0.0f;
        origin.y = 0.0f;
        break;
    case PIXIO_TOP_CENTER:
        origin.x = width / 2.0f;
        origin.y = 0.0f;
        break;
    case PIXIO_TOP_RIGHT:
        origin.x = width;
        origin.y = 0.0f;
        break;
    case PIXIO_CENTER_LEFT:
        origin.x = 0.0f;
        origin.y = height / 2.0f;
        break;
    case PIXIO_CENTER:
        origin.x = width / 2.0f;
        origin.y = height / 2.0f;
        break;
    case PIXIO_CENTER_RIGHT:
        origin.x = width;
        origin.y = height / 2.0f;
        break;
    case PIXIO_BOTTOM_LEFT:
        origin.x = 0.0f;
        origin.y = height;
        break;
    case PIXIO_BOTTOM_CENTER:
        origin.x = width / 2.0f;
        origin.y = height;
        break;
    case PIXIO_BOTTOM_RIGHT:
        origin.x = width;
        origin.y = height;
        break;
    }

    return origin;
}

static void pixio_calculate_bounding_box(ecs_iter_t *it)
{
    pixio_entity_t *einfo = ecs_field(it, pixio_entity_t, 0);
    pixio_transform_t *transform = ecs_field(it, pixio_transform_t, 1);
    pixio_text_t *draw_text = ecs_field(it, pixio_text_t, 2);
    pixio_shape_circle_t *shape_circle = ecs_field(it, pixio_shape_circle_t, 3);
    pixio_shape_rec_t *shape_rect = ecs_field(it, pixio_shape_rec_t, 4);
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, 5);

    for (int i = 0; i < it->count; i++)
    {
        if (!einfo[i].enabled)
            continue;

        pixio_vector2_t size = {0, 0};
        float paddind = 0.0f;

        if (sprite && sprite[i].texture.id > 0)
        {
            size = (pixio_vector2_t){sprite[i].dstRect.width, sprite[i].dstRect.height};
        }
        else if (shape_circle)
        {
            // TODO: Implementar cálculo de bounding box para círculos
            continue;
        }
        else if (shape_rect)
        {
            paddind = shape_rect[i].lineWidth;
            size = (pixio_vector2_t){shape_rect[i].width + paddind, shape_rect[i].height + paddind};
        }
        else if (draw_text)
        {
            size = MeasureTextEx(draw_text[i].font, draw_text[i].text, draw_text[i].fontSize, draw_text[i].spacing);
        }
        else
        {
            continue; // Si no hay componente de renderizado, saltamos esta entidad
        }

        // Actualizar la caja delimitadora
        transform[i].box.min = Vector2SubtractValue(transform[i].position, paddind);
        transform[i].box.max.x = transform[i].position.x + size.x;
        transform[i].box.max.y = transform[i].position.y + size.y;
        transform[i].box.size = (pixio_size_t){transform[i].box.max.x - transform[i].box.min.x, transform[i].box.max.y - transform[i].box.min.y};
    }
}

static void pixio_render_pre_draw(ecs_iter_t *it)
{
    pixio_render_t *render = ecs_field(it, pixio_render_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        pixi_render_begin();
        pixi_render_clear_color(render[i].clear_color);
        pixi_draw_grid(pixi_screen_width(), pixi_screen_height(), render[i].grid_size);
    }
}

// ecs_iter_t child_it = ecs_children(it->world, it->entities[i]);
// while (ecs_children_next(&it)) {
//     printf("it.count: %d\n",child_it.count);
// }

static void pixio_render_draw(ecs_iter_t *it)
{
    pixio_entity_t *einfo = ecs_field(it, pixio_entity_t, 0);
    pixio_transform_t *transform = ecs_field(it, pixio_transform_t, 1);
    pixio_text_t *draw_text = ecs_field(it, pixio_text_t, 2);
    pixio_shape_circle_t *shape_circle = ecs_field(it, pixio_shape_circle_t, 3);
    pixio_shape_rec_t *shape_rect = ecs_field(it, pixio_shape_rec_t, 4);
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, 5);

    for (int i = 0; i < it->count; i++)
    {
        pixio_transform_t t = transform[i];

        if (einfo[i].enabled == false)
            continue;

        rlPushMatrix();
        {
            pixio_vector2_t origin = calculate_origin(t.origin, t.box.size.width, t.box.size.height);

            rlTranslatef(t.position.x, t.position.y, 0.0f);
            rlRotatef(t.rotation, 0.0f, 0.0f, 1.0f);
            rlTranslatef(-origin.x, -origin.y, 0.0f);
            rlScalef(t.scale.x, t.scale.y, 1.0f);

            if (sprite && sprite[i].texture.id > 0)
            {
                DrawTexturePro(sprite[i].texture, sprite[i].srcRect, sprite[i].dstRect, (pixio_vector2_t){0.0, 0.0}, 0.0f, sprite[i].tint);
                SetTextureFilter(sprite[i].texture, sprite[i].filter);
            }
            else if (shape_circle)
            {
                // DrawCircle(shape_circle[i].center.x, shape_circle[i].center.y, shape_circle[i].radius, shape_circle[i].color);
                // DrawCircleLines(shape_circle[i].center.x, shape_circle[i].center.y, shape_circle[i].radius, shape_circle[i].color);
            }
            else if (shape_rect)
            {
                Rectangle shape_rect_rec = {0, 0, shape_rect[i].width, shape_rect[i].height};

                DrawRectangleRounded(shape_rect_rec, shape_rect[i].roundness, shape_rect[i].segments, shape_rect[i].color);
                if (shape_rect[i].lineWidth > 0)
                    DrawRectangleRoundedLinesEx(shape_rect_rec, shape_rect[i].roundness, shape_rect[i].segments, shape_rect[i].lineWidth, shape_rect[i].lineColor);
            }
            else if (draw_text)
            {
                DrawTextEx(draw_text[i].font, draw_text[i].text, (pixio_vector2_t){0, 0}, draw_text[i].fontSize, draw_text[i].spacing, draw_text[i].color);
            }

            // Draw Bounding Box
            DrawRectangleLines(t.box.min.x - t.position.x, t.box.min.y - t.position.y, t.box.size.width, t.box.size.height, RED);

            // Draw Origin Point
            DrawCircle(origin.x, origin.y, 5, RED);
        }
        rlPopMatrix();
    }
}

static void pixio_render_post_draw(ecs_iter_t *it)
{
    pixio_render_t *render = ecs_field(it, pixio_render_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        int vwidth = render[i].viewport.width;
        int vheight = render[i].viewport.height;
        DrawRectangleRoundedLinesEx((Rectangle){0, 0, vwidth, vheight}, 0.0, 1, 4.0, render[i].viewport_lineColor);
        pixi_render_end();
    }
}
