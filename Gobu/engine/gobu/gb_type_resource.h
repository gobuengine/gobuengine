#ifndef GB_TYPE_RESOURCE_H
#define GB_TYPE_RESOURCE_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/binn/binn_json.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_texture.h"
#include "gb_type_text.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gb_resource_t {
        char* path;
        gb_texture_t texture;
        gb_font_t font;
        binn* json;
    }gb_resource_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_resource_t);

    // module import
    void gb_type_resource_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_RESOURCE_H
