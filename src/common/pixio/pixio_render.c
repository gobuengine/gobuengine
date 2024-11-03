
#define PIXI_RLGL_API
#include "pixio_render.h"

#include "pixio_type.h"
#include "pixio_base.h"

enum
{
    PIXIO_COMPONENT_TRANSFORM = 0,
    PIXIO_COMPONENT_TEXT_RENDER,
    PIXIO_COMPONENT_CIRCLE_RENDER,
    PIXIO_COMPONENT_RECTANGLE_RENDER,
    PIXIO_COMPONENT_SPRITE_RENDER
} ORDER_COMPONENTS;

ECS_COMPONENT_DECLARE(pixio_render_phases_t);
ECS_COMPONENT_DECLARE(pixio_render_t);

static pixio_render_phases_t render_phases;

/**
 * Calcula el punto de origen basado en la alineación y las dimensiones dadas.
 *
 * @param alignment El tipo de alineación deseada.
 * @param width El ancho del objeto.
 * @param height La altura del objeto.
 * @return pixio_vector2_t El punto de origen calculado.
 */
static pixio_vector2_t transform_calculate_origin(pixio_origin_t alignment, float width, float height)
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

/**
 * Calcula las cajas delimitadoras para las entidades renderizables.
 *
 * @param it Iterador de entidades proporcionado por el sistema ECS.
 */
static void pixio_calculate_bounding_box(ecs_iter_t *it)
{
    // pixio_entity_t *einfo = ecs_field(it, pixio_entity_t, 0);
    pixio_transform_t *transform = ecs_field(it, pixio_transform_t, PIXIO_COMPONENT_TRANSFORM);
    pixio_text_t *draw_text = ecs_field(it, pixio_text_t, PIXIO_COMPONENT_TEXT_RENDER);
    pixio_shape_circle_t *shape_circle = ecs_field(it, pixio_shape_circle_t, PIXIO_COMPONENT_CIRCLE_RENDER);
    pixio_shape_rec_t *shape_rect = ecs_field(it, pixio_shape_rec_t, PIXIO_COMPONENT_RECTANGLE_RENDER);
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, PIXIO_COMPONENT_SPRITE_RENDER);

    for (int i = 0; i < it->count; i++)
    {
        pixio_vector2_t size = {0, 0};
        float padding = 0.0f;

        if (sprite && sprite[i].texture.id > 0)
        {
            size = (pixio_vector2_t){sprite[i].dstRect.width, sprite[i].dstRect.height};
        }
        else if (shape_circle && shape_circle[i].radius > 0)
        {
            float diameter = shape_circle[i].radius * 2;
            size = (pixio_vector2_t){diameter, diameter};
            padding = shape_circle[i].lineWidth / 2;
        }
        else if (shape_rect)
        {
            padding = shape_rect[i].lineWidth / 2;
            size = (pixio_vector2_t){shape_rect[i].width, shape_rect[i].height};
        }
        else if (draw_text)
        {
            size = MeasureTextEx(draw_text[i].font, draw_text[i].text, draw_text[i].fontSize, draw_text[i].spacing);
        }
        else
        {
            continue; // Si no hay componente de renderizado, saltamos esta entidad
        }

        // Calcular el origen basado en la alineación
        // pixio_vector2_t origin = transform_calculate_origin(transform[i].origin, size.x, size.y);

        // Actualizar la caja delimitadora
        transform[i].box.min.x = transform[i].position.x - padding;
        transform[i].box.min.y = transform[i].position.y - padding;
        transform[i].box.max.x = transform[i].box.min.x + size.x + padding * 2;
        transform[i].box.max.y = transform[i].box.min.y + size.y + padding * 2;
        transform[i].box.size = (pixio_size_t){
            transform[i].box.max.x - transform[i].box.min.x,
            transform[i].box.max.y - transform[i].box.min.y};
    }
}

/**
 * Realiza las operaciones de pre-renderizado para cada entidad de renderizado.
 *
 * Esta función se encarga de iniciar el renderizado, limpiar la pantalla con un color específico
 * y dibujar una cuadrícula de fondo si es necesario.
 *
 * @param it Iterador de entidades proporcionado por el sistema ECS.
 */
static void pixio_render_pre_draw(ecs_iter_t *it)
{
    pixio_render_t *render = ecs_field(it, pixio_render_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        pixi_render_begin();
        pixi_render_clear_color(render[i].clear_color);
        if (render[i].grid_enabled)
            pixi_draw_grid(pixi_screen_width(), pixi_screen_height(), render[i].grid_size);
    }
}

/**
 * Renderiza las entidades del juego.
 *
 * Esta función se encarga de dibujar sprites, formas y texto para cada entidad,
 * aplicando las transformaciones necesarias y dibujando elementos de depuración si es necesario.
 *
 * @param it Iterador de entidades proporcionado por el sistema ECS.
 */
static void pixio_render_draw(ecs_iter_t *it)
{
    const pixio_render_t *render = ecs_singleton_get(it->world, pixio_render_t);

    pixio_transform_t *transform = ecs_field(it, pixio_transform_t, PIXIO_COMPONENT_TRANSFORM);
    pixio_text_t *draw_text = ecs_field(it, pixio_text_t, PIXIO_COMPONENT_TEXT_RENDER);
    pixio_shape_circle_t *shape_circle = ecs_field(it, pixio_shape_circle_t, PIXIO_COMPONENT_CIRCLE_RENDER);
    pixio_shape_rec_t *shape_rect = ecs_field(it, pixio_shape_rec_t, PIXIO_COMPONENT_RECTANGLE_RENDER);
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, PIXIO_COMPONENT_SPRITE_RENDER);

    for (int i = 0; i < it->count; i++)
    {
        pixio_transform_t t = transform[i];
        pixio_vector2_t origin = transform_calculate_origin(t.origin, t.box.size.width, t.box.size.height);

        rlPushMatrix();
        rlTranslatef(t.position.x, t.position.y, 0.0f);
        rlRotatef(t.rotation, 0.0f, 0.0f, 1.0f);
        rlTranslatef(-origin.x, -origin.y, 0.0f);
        rlScalef(t.scale.x, t.scale.y, 1.0f);

        if (sprite && sprite[i].texture.id > 0)
        {
            DrawTexturePro(sprite[i].texture, sprite[i].srcRect, sprite[i].dstRect, (pixio_vector2_t){0.0, 0.0}, 0.0f, sprite[i].tint);
            SetTextureFilter(sprite[i].texture, sprite[i].filter);
        }
        else if (shape_circle && shape_circle[i].radius > 0)
        {
            // DrawCircleV(shape_circle[i].center, shape_circle[i].radius, shape_circle[i].color);
            // if (shape_circle[i].lineWidth > 0)
            //     DrawCircleLinesV(shape_circle[i].center, shape_circle[i].radius, shape_circle[i].lineColor);
        }
        else if (shape_rect)
        {
            Rectangle rect = {0, 0, shape_rect[i].width, shape_rect[i].height};
            DrawRectangleRounded(rect, shape_rect[i].roundness, shape_rect[i].segments, shape_rect[i].color);
            if (shape_rect[i].lineWidth > 0)
                DrawRectangleRoundedLinesEx(rect, shape_rect[i].roundness, shape_rect[i].segments, shape_rect[i].lineWidth, shape_rect[i].lineColor);
        }
        else if (draw_text)
        {
            DrawTextEx(draw_text[i].font, draw_text[i].text, (pixio_vector2_t){0, 0}, draw_text[i].fontSize, draw_text[i].spacing, draw_text[i].color);
        }

        // Dibujar elementos de depuración
        if (render->debug_bounding_box)
        {
            DrawRectangleLines(t.box.min.x - t.position.x, t.box.min.y - t.position.y, t.box.size.width, t.box.size.height, RED);
        }
        DrawCircleV(origin, 2, RED); // Punto de origen

        rlPopMatrix();
    }
}

/**
 * Renderiza efectos post-dibujo para el sistema de renderizado pixio.
 * Esta función dibuja un contorno rectangular redondeado alrededor del viewport
 * y llama a pixi_render_end() para cada entidad.
 *
 * @param it Puntero al iterador ECS que contiene los datos de los componentes
 */
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

/**
 * Importa y configura el módulo de renderizado pixio en el mundo ECS.
 * Esta función realiza las siguientes tareas:
 * - Define los componentes necesarios para el renderizado.
 * - Importa módulos dependientes.
 * - Crea y configura las fases de renderizado (PreDraw, Background, Draw, PostDraw).
 * - Establece las dependencias entre las fases de renderizado.
 * - Crea sistemas ECS para el cálculo de cajas delimitadoras y las diferentes etapas de renderizado.
 *
 * @param world Puntero al mundo ECS donde se importará el módulo de renderizado
 */

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
                       .query.terms = {{ecs_id(pixio_transform_t)}, {ecs_id(pixio_text_t), .oper = EcsOptional}, {ecs_id(pixio_shape_circle_t), .oper = EcsOptional}, {ecs_id(pixio_shape_rec_t), .oper = EcsOptional}, {ecs_id(pixio_sprite_t), .oper = EcsOptional}},
                       .callback = pixio_calculate_bounding_box});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.PreDraw))}),
                       .query.terms = {{ecs_id(pixio_render_t)}},
                       .callback = pixio_render_pre_draw});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.Draw))}),
                       .query.terms = {{ecs_id(pixio_transform_t)}, {ecs_id(pixio_text_t), .oper = EcsOptional}, {ecs_id(pixio_shape_circle_t), .oper = EcsOptional}, {ecs_id(pixio_shape_rec_t), .oper = EcsOptional}, {ecs_id(pixio_sprite_t), .oper = EcsOptional}},
                       .callback = pixio_render_draw});

    ecs_system(world, {.entity = ecs_entity(world, {.add = ecs_ids(ecs_dependson(render_phases.PostDraw))}),
                       .query.terms = {{ecs_id(pixio_render_t)}},
                       .callback = pixio_render_post_draw});
}
