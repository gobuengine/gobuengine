#include "gobu_resource.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"
#include "gobu_utility.h"
#include "gobu_project.h"

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
        // Buscamos la ruta relativa al directorio Content
        char* npath = strstr(path, "Content");
        if (npath != NULL) {
            npath += strlen("Content");
        }

        ecs_entity_t resource = ecs_new_entity(world, key);
        ecs_set(world, resource, gb_resource, { .path = npath });
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
        char* path = gb_path_join(gb_project_get_path(), "Content", resource[i].path, NULL);
        path = gb_path_normalize(path);

        if (event == EcsOnSet) {
            if (IsFileExtension(path, ".png") || IsFileExtension(path, ".jpg"))
                resource[i].texture = LoadTexture(path);
            if (IsFileExtension(path, ".ttf"))
                resource[i].font = LoadFont(path);
            if (IsFileExtension(path, ".anim"))
                resource[i].anim2d = binn_serialize_from_file(path);

        }
        else if (event == EcsOnRemove) {
            if (IsFileExtension(path, ".png") || IsFileExtension(path, ".jpg"))
                UnloadTexture(resource[i].texture);
            if (IsFileExtension(path, ".ttf"))
                UnloadFont(resource[i].font);
            if (IsFileExtension(path, ".anim"))
                binn_free(resource[i].anim2d);
        }

        free(path);
    }
}

