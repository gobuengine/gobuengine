#ifndef GOBU_TYPE_H
#define GOBU_TYPE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include "externs/flecs.h"

#ifndef go_extern
#ifdef __cplusplus
    #define go_extern extern "C"
#else
    #define go_extern extern
#endif
#endif

#define go_public go_extern
#define go_internal static

#define GOBU_VERSION "0.1.0"

#define GFXB_LINES 0x0001      // LINES
#define GFXB_TRIANGLES 0x0004  // TRIANGLES
#define GFXB_QUADS 0x0007      // QUADS
#define GFXB_POINTS 0x0008     // QUADS
#define GFXB_MODELVIEW 0x1700  // MODELVIEW
#define GFXB_PROJECTION 0x1701 // PROJECTION
#define GFXB_TEXTURE 0x1702    // TEXTURE

// Some Basic Colors
#define VIEWPORTMODEDARK    (go_color_t) { 18, 18, 18, 255 }
#define GRIDMODEDARK        (go_color_t) { 24, 24, 24, 255 }
#define LIGHTGRAY           (go_color_t) { 211, 211, 211, 255 }
#define GRAY                (go_color_t) { 150, 150, 150, 255 }
#define DARKGRAY            (go_color_t) { 90, 90, 90, 255 }
#define YELLOW              (go_color_t) { 255, 245, 60, 255 }
#define GOLD                (go_color_t) { 255, 200, 30, 255 }
#define ORANGE              (go_color_t) { 255, 140, 20, 255 }
#define PINK                (go_color_t) { 255, 120, 200, 255 }
#define RED                 (go_color_t) { 230, 50, 60, 255 }
#define MAROON              (go_color_t) { 175, 40, 60, 255 }
#define GREEN               (go_color_t) { 0, 220, 70, 255 }
#define LIME                (go_color_t) { 50, 180, 50, 255 }
#define DARKGREEN           (go_color_t) { 0, 100, 40, 255 }
#define SKYBLUE             (go_color_t) { 120, 200, 255, 255 }
#define BLUE                (go_color_t) { 0, 130, 245, 255 }
#define DARKBLUE            (go_color_t) { 0, 90, 170, 255 }
#define PURPLE              (go_color_t) { 190, 130, 255, 255 }
#define VIOLET              (go_color_t) { 140, 70, 200, 255 }
#define DARKPURPLE          (go_color_t) { 115, 40, 140, 255 }
#define BEIGE               (go_color_t) { 225, 200, 150, 255 }
#define BROWN               (go_color_t) { 130, 100, 75, 255 }
#define DARKBROWN           (go_color_t) { 85, 65, 50, 255 }
#define WHITE               (go_color_t) { 255, 255, 255, 255 }
#define BLACK               (go_color_t) { 0, 0, 0, 255 }
#define BLANK               (go_color_t) { 0, 0, 0, 0 }
#define MAGENTA             (go_color_t) { 255, 0, 255, 255 }
#define GOBUWHITE           (go_color_t) { 240, 240, 240, 255 }

typedef enum
{
    GB_PROPERTY_TYPE_NONE = 0,
    GB_PROPERTY_TYPE_TEXT,
    GB_PROPERTY_TYPE_TEXTURE,
    GB_PROPERTY_TYPE_FONT,
    GB_PROPERTY_TYPE_AUDIO
}go_property_type_t;

typedef enum
{
    GB_ORIGIN_TOP_LEFT = 0,
    GB_ORIGIN_TOP_CENTER,
    GB_ORIGIN_TOP_RIGHT,
    GB_ORIGIN_CENTER_LEFT,
    GB_ORIGIN_CENTER,
    GB_ORIGIN_CENTER_RIGHT,
    GB_ORIGIN_BOTTOM_LEFT,
    GB_ORIGIN_BOTTOM_CENTER,
    GB_ORIGIN_BOTTOM_RIGHT
} go_origin_t;

typedef enum
{
    GB_NO_FLIP = 0,
    GB_FLIP_HORIZONTAL = 1 << 0,
    GB_FLIP_VERTICAL = 1 << 1,
    GB_FLIP_BOTH = GB_FLIP_HORIZONTAL | GB_FLIP_VERTICAL
} go_texture_flip_t;

typedef enum
{
    GB_FILTER_POINT = 0,
    GB_FILTER_BILINEAR,
    GB_FILTER_TRILINEAR
} go_texture_filter_t;

typedef enum
{
    GB_SCALE_MODE_NEAREST = 0,
    GB_SCALE_MODE_LINEAR
} go_scale_mode_t;

typedef enum
{
    GB_RESIZE_MODE_NO_CHANGE = 0,
    GB_RESIZE_MODE_FILL_SCREEN,
    GB_RESIZE_MODE_ADJUST_WIDTH,
    GB_RESIZE_MODE_ADJUST_HEIGHT,
} go_resolution_mode_t;

typedef struct go_gfx_context_t go_gfx_context_t;
typedef ecs_string_t go_resource_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} go_color_t;

typedef struct
{
    float x;
    float y;
    float w;
    float h;
} go_rect_t;

typedef struct
{
    float x;
    float y;
} go_vec2_t;

typedef struct
{
    float width;
    float height;
} go_size_t;

typedef struct
{
    go_vec2_t min;
    go_vec2_t max;
    go_size_t size;
} go_boundingbox_t;

typedef struct
{
    uint32_t id;
} go_image_t;

typedef struct
{
    uint32_t id;
    uint32_t sampler;
    int width;
    int height;
} go_texture_t;

typedef struct go_font_t
{
    int id;
} go_font_t;

typedef struct
{
    ecs_u32_t hframes;
    ecs_u32_t vframes;
    ecs_u32_t frame;
} go_frame_t;

typedef struct
{
    go_vec2_t position;
    go_vec2_t scale;
    ecs_f32_t rotation;
    go_origin_t origin;
    go_boundingbox_t box;
} go_transform_t;

typedef struct
{
    ecs_string_t text;
    ecs_u32_t fontSize;
    ecs_f32_t spacing;
    go_color_t color;
    go_resource_t font_resource;
    go_font_t font;
} go_comp_text_t;

typedef struct
{
    go_resource_t texture_resource;
    go_texture_filter_t filter;
    go_texture_flip_t flip;
    go_color_t tint;
    go_rect_t srcRect;
    go_rect_t dstRect;
    go_texture_t texture;
} go_comp_sprite_t;

typedef struct
{
    ecs_f32_t radius;
    ecs_f32_t thickness;
    go_color_t color;
    ecs_f32_t lineWidth;
    go_color_t lineColor;
} go_comp_circle_t;

typedef struct
{
    ecs_f32_t width;
    ecs_f32_t height;
    ecs_f32_t roundness;
    go_color_t color;
    ecs_f32_t lineWidth;
    go_color_t lineColor;
    ecs_f32_t segments;
} go_comp_rectangle_t;

// MARK: CORE SCENE
typedef struct
{
    ecs_entity_t PreDraw;
    ecs_entity_t Background;
    ecs_entity_t Draw;
    ecs_entity_t PostDraw;
    ecs_entity_t ClearDraw;
} go_core_scene_phases_t;

typedef struct
{
    go_color_t color;
    bool debugBoundingBox;
} go_core_scene_t;

typedef struct
{
    ecs_u32_t gravity;
    go_vec2_t gravityDirection;
    bool enabled;
    bool debug;
} go_core_scene_physics_t;

typedef struct
{
    bool enabled;
    ecs_u32_t size;
} go_core_scene_grid_t;

// MARK: PROJECT SETTINGS
typedef struct
{
    ecs_string_t name;
    ecs_string_t description;
    ecs_string_t author;
}go_core_project_settings1_t;

typedef struct
{
    ecs_string_t name;
    ecs_string_t version;
    ecs_string_t publisher;
}go_core_project_settings2_t;

typedef struct
{
    go_size_t resolution;
    int targetFps;
    go_resolution_mode_t resolutionMode;
    go_scale_mode_t scaleMode;
}go_core_project_settings3_t;

// MARK: INSPECTOR PROPERTIES
typedef struct go_property_t {
    uint32_t type;
    bool hidden;
} go_property_t;

#endif // GOBU_TYPE_H

