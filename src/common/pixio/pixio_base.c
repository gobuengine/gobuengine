#include "pixio_base.h"
#include "pixio_type.h"

// -- -- -- -- -- -- -- -- --
// MARK: - ENTITY_INFO COMPONENT
// -- -- -- -- -- -- -- -- --
static void observe_set_entity_info(ecs_iter_t *it)
{
    pixio_entity_t *info = ecs_field(it, pixio_entity_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        ecs_enable(it->world, it->entities[i], info[i].enabled);
        //ecs_set_name(it->world, it->entities[i], info[i].name);
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - TEXT COMPONENT
// -- -- -- -- -- -- -- -- --
static void observe_set_text_default(ecs_iter_t *it)
{
    pixio_text_t *text = ecs_field(it, pixio_text_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        text[i].text = (text[i].text == NULL) ? "Text" : text[i].text;
        text[i].fontSize = (text[i].fontSize == 0) ? 10 : text[i].fontSize;
        text[i].spacing = (text[i].spacing <= 0.0f) ? (text[i].fontSize / 10) : text[i].spacing;
        text[i].color = (text[i].color.a == 0) ? PURPLE : text[i].color;
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - SPRITE COMPONENT
// -- -- -- -- -- -- -- -- --

// -- -- -- -- -- -- -- -- --
// MARK: - SHAPE COMPONENT
// -- -- -- -- -- -- -- -- --
static void observe_set_shape_circle_default(ecs_iter_t *it)
{
    pixio_shape_circle_t *shape = ecs_field(it, pixio_shape_circle_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        shape[i].radius = (shape[i].radius == 0) ? 25 : shape[i].radius;
        shape[i].thickness = (shape[i].thickness == 0) ? 1 : shape[i].thickness;
        shape[i].color = (shape[i].color.a == 0) ? PURPLE : shape[i].color;
    }
}

static void observe_set_shape_rect_default(ecs_iter_t *it)
{
    pixio_shape_rec_t *shape = ecs_field(it, pixio_shape_rec_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        shape[i].width = (shape[i].width == 0) ? 50 : shape[i].width;
        shape[i].height = (shape[i].height == 0) ? 50 : shape[i].height;
        shape[i].roundness = (shape[i].roundness == 0) ? 0 : shape[i].roundness;
        shape[i].segments = (shape[i].segments == 0) ? 60 : shape[i].segments;
        shape[i].color = (shape[i].color.a == 0) ? PURPLE : shape[i].color;

        shape[i].lineWidth = (shape[i].lineWidth == 0) ? 2.0 : shape[i].lineWidth;
        shape[i].lineColor = (shape[i].lineColor.a == 0) ? PURPLE : shape[i].lineColor;
    }
}

static void observe_set_sprite_default(ecs_iter_t *it)
{
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        //sprite[i].texture = gobu_resource(it->world, sprite[i].resource)->texture;
        sprite[i].srcRect = (Rectangle){ 0.0f, 0.0f, sprite[i].texture.width, sprite[i].texture.height };
        sprite[i].dstRect = (Rectangle){ 0.0f, 0.0f, sprite[i].texture.width, sprite[i].texture.height };
        sprite[i].tint = (sprite[i].tint.a == 0) ? WHITE : sprite[i].tint;
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - MODULE IMPORT
// -- -- -- -- -- -- -- -- --
void pixio_base_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, pixio_base_module);
    ECS_IMPORT(world, pixio_type_module);

    ecs_observer(world, {.query = {.terms = {{ecs_id(pixio_text_t)}}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_text_default});

    ecs_observer(world, {.query = {.terms = {{ecs_id(pixio_shape_circle_t)}}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_shape_circle_default});

    ecs_observer(world, {.query = {.terms = {{ecs_id(pixio_shape_rec_t)}}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_shape_rect_default});

    ecs_observer(world, {.query = {.terms = {{ecs_id(pixio_sprite_t)}}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_sprite_default});
}
