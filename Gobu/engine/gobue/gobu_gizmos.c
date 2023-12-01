#include "gobu_gizmos.h"
#include "gobu_rendering.h"
#include "gobu_bounding.h"
#include "gobu_input.h"
#include "gobu_camera.h"
#include "gobu_transform.h"

ECS_COMPONENT_DECLARE(ecs_gizmos_t);

static void GobuGizmos_Update(ecs_iter_t* it);
static void GobuGizmos_Draw(ecs_iter_t* it);

void GobuGizmosImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuGizmos);
    ECS_IMPORT(world, GobuRendering);
    ECS_IMPORT(world, GobuBoundingBox);
    ECS_IMPORT(world, GobuInputSystem);
    ECS_IMPORT(world, GobuCamera);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, ecs_gizmos_t);

    RenderPhases* phases = ecs_singleton_get(world, RenderPhases);

    ecs_struct(world, {
        .entity = ecs_id(ecs_gizmos_t),
        .members = {
            {.name = "selected", .type = ecs_id(ecs_bool_t) },
        }
    });

    ECS_SYSTEM(world, GobuGizmos_Draw, phases->Draw, ecs_gizmos_t, GBoundingBox);
    ECS_SYSTEM(world, GobuGizmos_Update, EcsPostUpdate, ecs_gizmos_t, GBoundingBox);
}


// // movemos la entidad seleccionada
// if (input->mouse.button_down(MOUSE_BUTTON_LEFT))
// {
//     if (entity_selected == it->entities[i])
//     {
//         bdraggable = true;
//         Vector2 delta = input->mouse.get_delta();
//         ecs_set(it->world, entity_selected, GPosition, { mouse_start->x + delta.x, mouse_start->y + delta.y });
//     }
// }

// if (input->mouse.button_released(MOUSE_BUTTON_LEFT) && bdraggable == true)
// {
//     entity_selected = -1;
//     bdraggable = false;
// }

static void GobuGizmos_Update(ecs_iter_t* it)
{
    ecs_gizmos_t* gizmos = ecs_field(it, ecs_gizmos_t, 1);
    GBoundingBox* box = ecs_field(it, GBoundingBox, 2);

    ecs_entity_t Engine = ecs_lookup(it->world, "Engine");
    GCamera* camera = ecs_get(it->world, Engine, GCamera);
    GInputSystem* input = ecs_get(it->world, Engine, GInputSystem);

    for (int i = it->count; i >= 0; i--)
    {
        Rectangle bonding = (Rectangle){ box[i].min.x, box[i].min.y, box[i].max.x, box[i].max.y };
        ecs_entity_t entity = it->entities[i];

        // seleccionamos una sola entidad por click
        if (input->mouse.button_pressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = input->mouse.get_screen_to_world(*camera);
            if (CheckCollisionPointRec(mouse, bonding))
            {
                // si presionamos shift, seleccionamos mas de una entidad
                if (!input->keyboard.down(KEY_LEFT_SHIFT))
                {
                    for (int j = 0; j < it->count; j++)
                    {
                        gizmos[j].selected = false;
                    }
                }

                gizmos[i].selected = true;
                break;
            }
        }

        // deseleccionamos la entidad si hacemos click en el mundo
        if (input->mouse.button_pressed(MOUSE_BUTTON_LEFT) && !input->keyboard.down(KEY_LEFT_SHIFT))
        {
            Vector2 mouse = input->mouse.get_screen_to_world(*camera);
            if (!CheckCollisionPointRec(mouse, bonding))
            {
                gizmos[i].selected = false;
            }
        }

        // movemos la entidad seleccionadas
        if (input->mouse.button_down(MOUSE_BUTTON_LEFT))
        {
            if (gizmos[i].selected)
            {
                Vector2 delta = input->mouse.get_delta();
                ecs_set(it->world, entity, GPosition, { box[i].min.x + delta.x, box[i].min.y + delta.y });
            }
        }
    }
}

static void GobuGizmos_Draw(ecs_iter_t* it)
{
    ecs_gizmos_t* gizmos = ecs_field(it, ecs_gizmos_t, 1);
    GBoundingBox* box = ecs_field(it, GBoundingBox, 2);

    for (int i = 0; i < it->count; i++)
    {
        if (!gizmos[i].selected) continue;
        Rectangle bonding = (Rectangle){ box[i].min.x, box[i].min.y, box[i].max.x, box[i].max.y };
        DrawRectangleLinesEx(bonding, 4, RED);
    }
}
