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
// :EVENTS
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
// :API
// -- 

const char* gb_resource_key_normalize(const char* path)
{
    if (strstr(path, "resource://") != NULL)
        return path;
    else
        return gb_path_normalize(gb_strdups("resource://%s", gb_str_replace(path, ".", "!")));
}

/**
 * @brief Establece un recurso en el mundo de entidades.
 *
 * Esta función permite establecer un recurso en el mundo de entidades especificado.
 * El recurso se identifica mediante una clave y se especifica su ruta de acceso.
 *
 * @param world El mundo de entidades en el que se establecerá el recurso.
 * @param key La clave que identifica el recurso.
 * @param path La ruta de acceso al recurso.
 * @return true si se estableció el recurso correctamente, false en caso contrario.
 */
const char* gb_resource_set(ecs_world_t* world, const char* path)
{
    const char* path_relative = gb_path_relative_content(path);
    const char* key = gb_path_normalize(gb_strdups("resource://%s", gb_str_replace(path_relative, ".", "!")));

    if (ecs_is_valid(world, ecs_lookup(world, key)) == false) {
        ecs_entity_t resource = ecs_new_entity(world, key);
        ecs_set(world, resource, gb_resource_t, { .path = gb_strdup(path_relative) });
    }

    return key;
}

/**
 * @brief Obtiene un recurso de Gobu.
 *
 * Esta función devuelve un puntero al recurso identificado por la clave especificada.
 *
 * @param world Puntero al mundo de entidades de Gobu.
 * @param key Clave del recurso a obtener.
 * @return Puntero al recurso correspondiente a la clave especificada, o NULL si no se encuentra.
 */
const gb_resource_t* gb_resource(ecs_world_t* world, const char* key)
{
    ecs_entity_t resource = ecs_lookup(world, key);
    gb_return_val_if_fail(resource != 0, NULL);
    return (gb_resource_t*)ecs_get(world, resource, gb_resource_t);
}

/**
 * @brief Elimina un recurso del mundo.
 *
 * Esta función elimina un recurso del mundo dado utilizando la clave especificada.
 *
 * @param world El mundo del cual eliminar el recurso.
 * @param key La clave del recurso a eliminar.
 * @return true si el recurso se eliminó correctamente, false en caso contrario.
 */
bool gb_resource_remove(ecs_world_t* world, const char* key)
{
    ecs_entity_t resource = ecs_lookup(world, key);
    gb_return_val_if_fail(resource != 0, false);
    ecs_delete(world, resource);
    return ecs_lookup(world, key) == 0;
}

