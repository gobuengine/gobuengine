#include "gb_ecs_resource.h"

static void observe_set_gb_resource(ecs_iter_t* it);

void gb_resource_moduleImport(ecs_world_t* world)
{
    ECS_MODULE(world, gb_resource_module);
    ECS_IMPORT(world, gb_type_resource_module);

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_resource_t)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = observe_set_gb_resource
    });
}

// -- 
// gb_sprite_t:EVENTS
// --  
static void observe_set_gb_resource(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    gb_resource_t* resource = ecs_field(it, gb_resource_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        char* path = gb_path_join_relative_content(resource[i].path);

        if (event == EcsOnSet) {
            if (IsFileExtension(path, ".png") || IsFileExtension(path, ".jpg"))
                resource[i].texture = LoadTexture(path);
            if (IsFileExtension(path, ".ttf"))
                resource[i].font = LoadFont(path);
            if (IsFileExtension(path, ".asprites"))
                resource[i].json = binn_serialize_from_file(path);
            if (IsFileExtension(path, ".sprite")) {
                resource[i].json = binn_serialize_from_file(path);
                resource[i].texture = LoadTexture(gb_path_join_relative_content(binn_object_str(resource[i].json, "resource")));
            }
        }
        else if (event == EcsOnRemove) {
            if (IsFileExtension(path, ".png") || IsFileExtension(path, ".jpg"))
                UnloadTexture(resource[i].texture);
            if (IsFileExtension(path, ".ttf"))
                UnloadFont(resource[i].font);
            if (IsFileExtension(path, ".asprites") || IsFileExtension(path, ".sprite"))
                binn_free(resource[i].json);
        }

        free(path);
    }
}

// -- 
// gb_sprite_t:API
// -- 

