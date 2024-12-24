#pragma once
#ifndef PIXI_TYPE_H
#define PIXI_TYPE_H

#include "pixio.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef Color pixio_color_t;
    typedef Font pixio_font_t;
    typedef ecs_string_t pixio_resource_t;
    typedef pixio_resource_t pixio_resource_audio_t;
    typedef pixio_resource_t pixio_resource_animated_t;
    typedef pixio_resource_t pixio_resource_font_t;
    typedef pixio_resource_t pixio_resource_texture_t;
    typedef pixio_resource_t pixio_script_t;
    typedef Rectangle pixio_rect_t;
    typedef Texture2D pixio_texture_t;
    typedef TextureFilter pixio_texture_filter_t;
    typedef Vector2 pixio_vector2_t;

    typedef enum _pixio_origin_t
    {
        PIXIO_TOP_LEFT,      // Esquina superior izquierda
        PIXIO_TOP_CENTER,    // Centro superior
        PIXIO_TOP_RIGHT,     // Esquina superior derecha
        PIXIO_CENTER_LEFT,   // Centro izquierdo
        PIXIO_CENTER,        // Centro
        PIXIO_CENTER_RIGHT,  // Centro derecho
        PIXIO_BOTTOM_LEFT,   // Esquina inferior izquierda
        PIXIO_BOTTOM_CENTER, // Centro inferior
        PIXIO_BOTTOM_RIGHT   // Esquina inferior derecha
    } pixio_origin_t;

    typedef enum _pixio_texture_flip_t
    {
        PIXIO_NO_FLIP = 0,                                            // No flip applied
        PIXIO_FLIP_HORIZONTAL = 1 << 0,                               // Flip horizontally (mirror horizontally)
        PIXIO_FLIP_VERTICAL = 1 << 1,                                 // Flip vertically (mirror vertically)
        PIXIO_FLIP_BOTH = PIXIO_FLIP_HORIZONTAL | PIXIO_FLIP_VERTICAL // Flip both horizontally and vertically
    } pixio_texture_flip_t;

    typedef struct _EcsPixioSceneActive
    {
        ecs_entity_t entity;
    } EcsPixioSceneActive;

    typedef EcsPixioSceneActive EcsPixioSceneMain;

    typedef struct _pixio_size_t
    {
        ecs_f32_t width;
        ecs_f32_t height;
    } pixio_size_t;

    typedef struct _pixio_boundingbox_t
    {
        pixio_vector2_t min;
        pixio_vector2_t max;
        pixio_size_t size;
    } pixio_boundingbox_t;

    typedef struct _pixio_entity_t
    {
        ecs_bool_t enabled;
        ecs_string_t name;
    } pixio_entity_t;

    typedef struct _pixio_transform_t
    {
        pixio_vector2_t position;
        pixio_vector2_t scale;
        ecs_f32_t rotation;
        pixio_origin_t origin;
        pixio_boundingbox_t box;
    } pixio_transform_t;

    typedef struct _pixio_text_t
    {
        ecs_string_t text;
        ecs_u32_t fontSize;
        ecs_f32_t spacing;
        pixio_color_t color;
        pixio_resource_font_t font_resource;
        pixio_font_t font;
    } pixio_text_t;

    typedef struct _pixio_sprite_t
    {
        pixio_resource_texture_t texture_resource;
        pixio_texture_filter_t filter;
        pixio_texture_flip_t flip;
        pixio_color_t tint;
        ecs_u32_t hframes;
        ecs_u32_t vframes;
        ecs_u32_t frame;
        pixio_rect_t srcRect;
        pixio_rect_t dstRect;
        pixio_texture_t texture;
    } pixio_sprite_t;

    typedef struct _pixio_shape_circle_t
    {
        ecs_f32_t radius;
        ecs_f32_t thickness;
        pixio_color_t color;
        ecs_f32_t lineWidth;
        pixio_color_t lineColor;
    } pixio_shape_circle_t;

    typedef struct _pixio_shape_rec_t
    {
        ecs_f32_t width;
        ecs_f32_t height;
        ecs_f32_t roundness;
        pixio_color_t color;
        ecs_f32_t lineWidth;
        pixio_color_t lineColor;
        ecs_f32_t segments;
    } pixio_shape_rec_t;

    // una struct para manejar los datos de las animaciones
    typedef struct _pixio_animated_data_t
    {
        char *name;
        int frame;
        int frameCount;
        float frameTime;
        float frameSpeed;
        bool loop;
    } pixio_animated_data_t;

    typedef struct _pixio_animated_t
    {
        pixio_resource_animated_t animated;
        // ecs_u32_t Hframes;
        // ecs_u32_t Vframes;
        // ecs_u32_t frame;
    } pixio_animated_t;

    extern ECS_TAG_DECLARE(EcsPixioTagScene);
    extern ECS_TAG_DECLARE(EcsPixioOnOpenScene);
    extern ECS_TAG_DECLARE(EcsPixioOnCreateScene);
    extern ECS_TAG_DECLARE(EcsPixioOnDeleteScene);
    extern ECS_COMPONENT_DECLARE(EcsPixioSceneActive);
    extern ECS_COMPONENT_DECLARE(EcsPixioSceneMain);

    extern ECS_COMPONENT_DECLARE(pixio_animated_t);
    extern ECS_COMPONENT_DECLARE(pixio_color_t);
    extern ECS_COMPONENT_DECLARE(pixio_entity_t);
    extern ECS_COMPONENT_DECLARE(pixio_font_t);
    extern ECS_COMPONENT_DECLARE(pixio_origin_t);
    extern ECS_COMPONENT_DECLARE(pixio_rect_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_animated_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_audio_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_font_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_texture_t);
    extern ECS_COMPONENT_DECLARE(pixio_script_t);
    extern ECS_COMPONENT_DECLARE(pixio_shape_circle_t);
    extern ECS_COMPONENT_DECLARE(pixio_shape_rec_t);
    extern ECS_COMPONENT_DECLARE(pixio_size_t);
    extern ECS_COMPONENT_DECLARE(pixio_sprite_t);
    extern ECS_COMPONENT_DECLARE(pixio_text_t);
    extern ECS_COMPONENT_DECLARE(pixio_texture_t);
    extern ECS_COMPONENT_DECLARE(pixio_texture_filter_t);
    extern ECS_COMPONENT_DECLARE(pixio_texture_flip_t);
    extern ECS_COMPONENT_DECLARE(pixio_transform_t);
    extern ECS_COMPONENT_DECLARE(pixio_vector2_t);

    void pixio_type_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // PIXI_TYPE_H
