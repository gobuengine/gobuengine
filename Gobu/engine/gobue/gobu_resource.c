#include "gobu_resource.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(gb_resource);

static void GobuShapes_SetResource(ecs_iter_t* it);

void GobuResourceImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuResource);

    ECS_COMPONENT_DEFINE(world, gb_resource);

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_resource)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = GobuShapes_SetResource
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_resource),
        .members = {
            {.name = "path", .type = ecs_id(ecs_string_t) },
        }
    });
}

bool gobu_resource_set(ecs_world_t* world, const char* key, const char* path)
{
    if (ecs_is_valid(world, ecs_lookup(world, key) == false)) {
        ecs_entity_t resource = ecs_new_entity(world, key);
        ecs_set(world, resource, gb_resource, { .path = path });
        return true;
    }
    return false;
}

const gb_resource* gobu_resource(ecs_world_t* world, const char* key)
{
    ecs_entity_t resource = ecs_lookup(world, key);
    return (gb_resource*)ecs_get(world, resource, gb_resource);
}

static void GobuShapes_SetResource(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    gb_resource* resource = ecs_field(it, gb_resource, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (event == EcsOnSet) {
            if (IsFileExtension(resource[i].path, ".png") || IsFileExtension(resource[i].path, ".jpg"))
                resource[i].texture = LoadTexture(resource[i].path);
            if (IsFileExtension(resource[i].path, ".ttf"))
                resource[i].font = LoadFont(resource[i].path);
            if (IsFileExtension(resource[i].path, ".anim"))
                resource[i].anim2d = binn_serialize_from_file(resource[i].path);

        }
        else if (event == EcsOnRemove) {
            if (IsFileExtension(resource[i].path, ".png") || IsFileExtension(resource[i].path, ".jpg"))
                UnloadTexture(resource[i].texture);
            if (IsFileExtension(resource[i].path, ".ttf"))
                UnloadFont(resource[i].font);
            if (IsFileExtension(resource[i].path, ".anim"))
                binn_free(resource[i].anim2d);
        }
    }
}

