#include "gb_ecs_text.h"
#include "gb_ecs_resource.h"

static void observe_set_gb_text_t(ecs_iter_t* it);

void gb_text_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_text_module);
    ECS_IMPORT(world, gb_type_text_module);

    ecs_observer(world, {
        .filter = {.terms = { {.id = ecs_id(gb_text_t)} }},
        .events = { EcsOnSet },
        .callback = observe_set_gb_text_t
    });
}

// -- 
// gb_sprite_t:EVENTS
// --  
static void observe_set_gb_text_t(ecs_iter_t* it)
{
    gb_text_t* text = ecs_field(it, gb_text_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        text[i].font = gb_resource(it->world, text[i].resource)->font;
        text[i].size = (text[i].size <= 0) ? 20.0f : text[i].size;
        text[i].spacing = (text[i].spacing <= 0.0f) ? (text[i].size / 10) : text[i].spacing;
    }
}

// -- 
// gb_sprite_t:API
// --  
