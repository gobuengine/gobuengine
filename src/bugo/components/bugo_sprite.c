#include "bugo_sprite.h"

static ecs_entity_t ecs_sprite;
static ecs_entity_t sys_sprite;

static void bugo_system_on_draw(ecs_iter_t *it)
{
    ComponentRenderer *r = ecs_field(it, ComponentRenderer, 1);
    ComponentSprite *sprite = ecs_field(it, ComponentSprite, 2);

    for (int i = 0; i < it->count; i++)
    {
        Vector2 scale = r[i].scale;
        float rotation = r[i].rotation;
        float x = r[i].position.x;
        float y = r[i].position.y;

        Rectangle src = sprite[i].src;
        Rectangle dst = (Rectangle){x, y, fabs(src.width) * scale.x, fabs(src.height) * scale.y};

        float ox = (dst.width  * r[i].origin.x);
        float oy = (dst.height * r[i].origin.y);

        int flip_h = !sprite[i].flip_h ? 1 : -1;
        int flip_v = !sprite[i].flip_v ? 1 : -1;

        src.width  *= flip_h;
        src.height *= flip_v;

        bugo_draw_texture(sprite[i].texture, src, dst, (Vector2){ox, oy}, rotation, sprite[i].tint);
    }
}

static void bugo_system_on_set(ecs_iter_t *it)
{
    ComponentSprite *sprite = ecs_field(it, ComponentSprite, 1);

    for (int i = 0; i < it->count; i++)
    {
        sprite[i].src.x = 0;
        sprite[i].src.y = 0;
        sprite[i].src.width  = (float)sprite[i].texture.width;
        sprite[i].src.height = (float)sprite[i].texture.height;
    }
}

void bugo_ecs_init_sprite(ecs_world_t *world)
{
    ecs_sprite = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "ComponentSprite"}),
        .type.size = ECS_SIZEOF(ComponentSprite),
        .type.alignment = ECS_ALIGNOF(ComponentSprite),
    });

    ecs_struct(world, {
        .entity = ecs_sprite,
        .members = {
            { .name = "color", .type = bugo_ecs_get_color_id() },
        },
    });

    sys_sprite = ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = {ecs_dependson(bugo_ecs_get_phaser(PHASER_BEGIN_CAMERA))},
        }),
        .query.filter.terms = {
            { .id = bugo_ecs_get_renderer_id()},
            { .id = ecs_sprite},
        },
        .callback = bugo_system_on_draw,
    });

    ecs_observer(world, {
        .filter = { .terms = {{ .id = ecs_sprite }}},
        .events = { EcsOnSet },
        .callback = bugo_system_on_set
    });
}

void bugo_ecs_set_sprite(ecs_entity_t entity, ComponentSprite *props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_sprite, sizeof(ComponentSprite), props);
}

ecs_entity_t bugo_ecs_get_sprite_id(void)
{
    return ecs_sprite;
}
