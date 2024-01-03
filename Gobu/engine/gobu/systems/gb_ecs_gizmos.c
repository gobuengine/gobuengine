#include "gb_ecs_gizmos.h"

static void update_gb_gizmos_t(ecs_iter_t* it);

void gb_gizmos_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_gizmos_module);
    ECS_IMPORT(world, gb_type_gizmos_module);
    ECS_IMPORT(world, gb_type_bounding_module);
    ECS_IMPORT(world, gb_type_transform_module);
    ECS_IMPORT(world, gb_type_camera_module);

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = {
            {.id = ecs_id(gb_gizmos_t)},
            {.id = ecs_id(gb_transform_t)},
            {.id = ecs_id(gb_bounding_t)}
        },
        .callback = update_gb_gizmos_t
    });
}

// -- 
// gb_sprite_t:EVENTS
// --  
static void update_gb_gizmos_t(ecs_iter_t* it)
{
    gb_gizmos_t* gizmos = ecs_field(it, gb_gizmos_t, 1);
    gb_transform_t* transform = ecs_field(it, gb_transform_t, 2);
    gb_bounding_t* bounding = ecs_field(it, gb_bounding_t, 3);

    ecs_entity_t Engine = ecs_lookup(it->world, "Engine");
    gb_camera_t* camera = ecs_get(it->world, Engine, gb_camera_t);

    gb_vec2_t mouse = screen_to_world(*camera, input_mouse_position());
    gb_vec2_t delta = input_mouse_delta();

    bool shift = input_is_key_down(KEY_LEFT_SHIFT);
    // bool ctrl = input_is_key_down(KEY_LEFT_CONTROL);
    bool mouse_btn_pres_left = input_is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    bool mouse_btn_down_left = input_is_mouse_button_down(MOUSE_BUTTON_LEFT);

    for (int i = it->count - 1; i >= 0; i--)
    {
        Rectangle bonding = (Rectangle){ bounding[i].min.x, bounding[i].min.y, bounding[i].max.x, bounding[i].max.y };
        ecs_entity_t entity = it->entities[i];

        // seleccionamos una sola entidad por click
        if (mouse_btn_pres_left)
        {
            bool selected = CheckCollisionPointRec(mouse, bonding);

            // no deseleccionamos cuando tenemos shift presionado
            if (!shift)
            {
                for (int n = it->count - 1; n >= 0; n--)
                {
                    gizmos[n].selected = false;
                }
            }

            if (selected)
            {
                gizmos[i].selected = true;
                break;
            }
        }

        if (mouse_btn_down_left)
        {
            if (gizmos[i].selected)
            {
                transform[i].position.x += delta.x;
                transform[i].position.y += delta.y;
            }
        }
    }
}

// -- 
// gb_sprite_t:API
// -- 

