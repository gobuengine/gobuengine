#include "gb_ecs_bounding.h"

static void update_gb_bounding_t(ecs_iter_t* it);

void gb_bounding_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_bounding_module);
    ECS_IMPORT(world, gb_type_transform_module);
    ECS_IMPORT(world, gb_type_bounding_module);
    ECS_IMPORT(world, gb_type_shape_circle_module);
    ECS_IMPORT(world, gb_type_shape_rect_module);
    ECS_IMPORT(world, gb_type_text_module);
    ECS_IMPORT(world, gb_type_sprite_module);

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = {
            {.id = ecs_id(gb_bounding_t)},
            {.id = ecs_id(gb_transform_t)},
            {.id = ecs_id(gb_shape_rect_t), .oper = EcsOptional},
            {.id = ecs_id(gb_sprite_t), .oper = EcsOptional},
            {.id = ecs_id(gb_text_t), .oper = EcsOptional}
        },
        .callback = update_gb_bounding_t
    });
}

// -- 
// gb_sprite_t:EVENTS
// --  
static void update_gb_bounding_t(ecs_iter_t* it)
{
    gb_bounding_t* bounding = ecs_field(it, gb_bounding_t, 1);
    gb_transform_t* transform = ecs_field(it, gb_transform_t, 2);

    // Drawing
    gb_shape_rect_t* rect = ecs_field(it, gb_shape_rect_t, 3);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 4);
    gb_text_t* text = ecs_field(it, gb_text_t, 5);

    for (int i = 0; i < it->count; i++)
    {
        bounding[i].size.x = 0.0f;
        bounding[i].size.y = 0.0f;

        if (rect) {
            bounding[i].size = (gb_vec2_t){ rect[i].width, rect[i].height };
        }

        if (sprite) {
            bounding[i].size = (gb_vec2_t){ sprite[i].dst.width, sprite[i].dst.height };
        }

        if (text) {
            bounding[i].size = MeasureTextEx(text[i].font, text[i].text, text[i].size, text[i].spacing);
        }

        bounding[i].min.x = transform[i].position.x - (transform[i].origin.x * bounding[i].size.x);
        bounding[i].min.y = transform[i].position.y - (transform[i].origin.y * bounding[i].size.y);
        bounding[i].max.x = bounding[i].size.x;
        bounding[i].max.y = bounding[i].size.y;
        bounding[i].center.x = bounding[i].min.x + (bounding[i].size.x / 2);
        bounding[i].center.y = bounding[i].min.y + (bounding[i].size.y / 2);
    }
}

// -- 
// gb_sprite_t:API
// -- 

