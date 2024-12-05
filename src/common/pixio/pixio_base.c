#include "pixio_base.h"
#include "pixio_type.h"

// -- -- -- -- -- -- -- -- --
// MARK: - ENTITY_INFO
// -- -- -- -- -- -- -- -- --
// static void observe_set_entity_info(ecs_iter_t *it)
// {
//     pixio_entity_t *info = ecs_field(it, pixio_entity_t, 0);

//     for (int i = 0; i < it->count; i++)
//     {
//         ecs_enable(it->world, it->entities[i], info[i].enabled);
//         //ecs_set_name(it->world, it->entities[i], info[i].name);
//     }
// }

// -- -- -- -- -- -- -- -- --
// MARK: - TEXT-RENDERER
// -- -- -- -- -- -- -- -- --
static void observe_set_text_default(ecs_iter_t *it)
{
    pixio_text_t *text = ecs_field(it, pixio_text_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        text[i].font = (text[i].font.texture.id == 0) ? GetFontDefault() : text[i].font;
        text[i].text = (text[i].text == NULL) ? strdup("Text") : text[i].text;
        text[i].fontSize = (text[i].fontSize == 0) ? 10 : text[i].fontSize;
        text[i].spacing = (text[i].spacing <= 0.0f) ? (text[i].fontSize / 10) : text[i].spacing;
        text[i].color = (text[i].color.a == 0) ? PURPLE : text[i].color;
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - SPRITE-RENDERER
// -- -- -- -- -- -- -- -- --

static void observe_set_sprite_default(ecs_iter_t *it)
{
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        // sprite[i].texture = gobu_resource(it->world, sprite[i].resource)->texture;
        sprite[i].srcRect = (Rectangle){0.0f, 0.0f, sprite[i].texture.width, sprite[i].texture.height};
        sprite[i].dstRect = (Rectangle){0.0f, 0.0f, sprite[i].texture.width, sprite[i].texture.height};
        sprite[i].tint = (sprite[i].tint.a == 0) ? WHITE : sprite[i].tint;
    }
}

static void observe_update_sprite_default(ecs_iter_t *it)
{
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        int textureWidth = sprite[i].texture.width;
        int textureHeight = sprite[i].texture.height;

        int frameWidth  = textureWidth / sprite[i].hframes;
        int frameHeight = textureHeight / sprite[i].vframes;

        pixio_rect_t dst = (pixio_rect_t){0, 0, textureWidth, textureHeight};
        pixio_rect_t src = dst;

        if (frameWidth != textureWidth || frameHeight != textureHeight)
        {
            dst.width = src.width = frameWidth;
            dst.height = src.height = frameHeight;
            src.x = (sprite[i].frame % (int)(textureWidth/ frameWidth)) * frameWidth;
            src.y = (sprite[i].frame / (textureWidth/ frameHeight)) * frameHeight;
        }

        src.width = sprite[i].flip == PIXIO_FLIP_HORIZONTAL ? -src.width : src.width;
        src.height = sprite[i].flip == PIXIO_FLIP_VERTICAL ? -src.height : src.height;
        
        sprite[i].srcRect = (Rectangle){src.x, src.y, src.width, src.height};
        sprite[i].dstRect = (Rectangle){dst.x, dst.y, dst.width, dst.height};
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - ANIMATE-SPRITE 
// -- -- -- -- -- -- -- -- --

static void system_update_animate_sprite(ecs_iter_t *it)
{
    pixio_sprite_t *sprite = ecs_field(it, pixio_sprite_t, 0);
    pixio_animated_t *anim = ecs_field(it, pixio_animated_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (strlen(anim[i].animated) == 0)
            continue;

        // if (anim[i].frameCount > 0)
        // {
        //     anim[i].frameTime += GetFrameTime();
        //     if (anim[i].frameTime >= anim[i].frameSpeed)
        //     {
        //         anim[i].frame++;
        //         anim[i].frameTime = 0;
        //         if (anim[i].frame >= anim[i].frameCount)
        //             anim[i].frame = 0;
        //     }
        //     sprite[i].srcRect.x = (float)anim[i].frame * sprite[i].texture.width / anim[i].frameCount;
        //     sprite[i].srcRect.y = 0;
        //     sprite[i].srcRect.width = (float)sprite[i].texture.width / anim[i].frameCount;
        //     sprite[i].srcRect.height = (float)sprite[i].texture.height;
        // }

        printf("Animate Sprite: %d\n", i);
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - SHAPE
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

        // shape[i].lineWidth = (shape[i].lineWidth == 0) ? 0.0 : shape[i].lineWidth;
        // shape[i].lineColor = (shape[i].lineColor.a == 0) ? BLANK : shape[i].lineColor;
    }
}

// -- -- -- -- -- -- -- -- --
// MARK: - MODULE IMPORT
// -- -- -- -- -- -- -- -- --
void pixio_base_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, pixio_base_module);
    // ECS_IMPORT(world, pixio_type_module);

    ecs_observer(world, {.query.terms = {{ecs_id(pixio_text_t)}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_text_default});

    ecs_observer(world, {.query.terms = {{ecs_id(pixio_shape_circle_t)}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_shape_circle_default});

    ecs_observer(world, {.query.terms = {{ecs_id(pixio_shape_rec_t)}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_shape_rect_default});

    ecs_observer(world, {.query.terms = {{ecs_id(pixio_sprite_t)}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_sprite_default});

    ecs_system(world, {.query.terms = {{ecs_id(pixio_sprite_t)}, {ecs_id(pixio_animated_t)}},
                       .callback = system_update_animate_sprite});
}
