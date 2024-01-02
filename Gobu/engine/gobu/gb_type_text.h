#ifndef GB_TYPE_TEXT_H
#define GB_TYPE_TEXT_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"

#include "gb_type_color.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef Font gb_font_t;

    typedef struct gb_text_t
    {
        char* resource;
        char* text;
        float size;
        float spacing;
        gb_color_t color;
        gb_font_t font;
    }gb_text_t;

    // global component
    extern ECS_COMPONENT_DECLARE(gb_font_t);
    extern ECS_COMPONENT_DECLARE(gb_text_t);

    // module import
    void gb_type_text_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_TEXT_H
