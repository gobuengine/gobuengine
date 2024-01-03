#include "gb_ecs_sprite.h"
#include "gb_ecs_resource.h"
#include "gb_log.h"

static void observe_set_gb_sprite(ecs_iter_t *it);

void gb_sprite_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, gb_sprite_module);
    ECS_IMPORT(world, gb_type_sprite_module);

    ecs_observer(world, {.filter = {
                             .terms = {
                                 {.id = ecs_id(gb_sprite_t)},
                             },
                         },
                         .events = {EcsOnSet},
                         .callback = observe_set_gb_sprite});
}

// --
// gb_sprite_t:EVENTS
// --
static void observe_set_gb_sprite(ecs_iter_t *it)
{
    gb_sprite_t *sprite = ecs_field(it, gb_sprite_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        gb_resource_t *resource = gb_resource(it->world, sprite[i].resource);
        gb_return_if_fail(resource != NULL);
        gb_return_if_fail(resource->texture.id != 0);

        if (resource->json != NULL)
        {
            sprite[i] = gb_sprite_deserialize(resource->json);
        }

        sprite[i].texture = resource->texture;

        gb_if_default_value(sprite[i].src.width, resource->texture.width);
        gb_if_default_value(sprite[i].src.height, resource->texture.height);
        gb_if_default_value(sprite[i].dst.width, resource->texture.width);
        gb_if_default_value(sprite[i].dst.height, resource->texture.height);

        sprite[i].tint = (sprite[i].tint.a == 0) ? (gb_color_t){255, 255, 255, 255} : sprite[i].tint;
    }
}

// --
// gb_sprite_t:API
// --
gb_sprite_t gb_sprite_deserialize(binn *fsprite)
{
    gb_sprite_t sprite = {0};

    sprite.resource = binn_object_str(fsprite, "resource");

    sprite.src.x = binn_object_float(fsprite, "x");
    sprite.src.y = binn_object_float(fsprite, "y");
    sprite.dst.width = binn_object_float(fsprite, "width");
    sprite.dst.height = binn_object_float(fsprite, "height");
    sprite.src.width = sprite.dst.width;
    sprite.src.height = sprite.dst.height;
    sprite.dst.x = 0.0f;
    sprite.dst.y = 0.0f;

    return sprite;
}

binn *gb_sprite_serialize(gb_sprite_t sprite)
{
    binn *fsprite = binn_object();
    binn_object_set_str(fsprite, "resource", sprite.resource);
    binn_object_set_float(fsprite, "x", sprite.src.x);
    binn_object_set_float(fsprite, "y", sprite.src.y);
    binn_object_set_float(fsprite, "width", sprite.src.width);
    binn_object_set_float(fsprite, "height", sprite.src.height);
    return fsprite;
}

gb_sprite_t gb_sprite_init(void)
{
    gb_sprite_t sprite = {0};
    return sprite;
}
