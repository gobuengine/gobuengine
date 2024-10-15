#pragma once
#ifndef PIXI_TYPE_H
#define PIXI_TYPE_H

#include "pixio.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef TextureFilter pixio_texture_filter_t;
    typedef Vector2 pixio_vector2_t;
    typedef Rectangle pixio_rect_t;
    typedef Color pixio_color_t;
    typedef Font pixio_font_t;
    typedef Texture2D pixio_texture_t;
    typedef ecs_string_t pixio_resource_t;
    typedef pixio_resource_t pixio_resource_font_t;
    typedef pixio_resource_t pixio_resource_texture_t;
    typedef pixio_resource_t pixio_script_t;

    typedef struct pixio_size_t
    {
        ecs_f32_t width;
        ecs_f32_t height;
    } pixio_size_t;

    typedef struct pixio_entity_t
    {
        ecs_bool_t enabled;
        ecs_string_t name;
    } pixio_entity_t;

    typedef struct pixio_transform_t
    {
        pixio_vector2_t position;
        pixio_vector2_t scale;
        ecs_f32_t rotation;
        pixio_vector2_t origin;
    } pixio_transform_t;

    typedef struct pixio_text_t
    {
        ecs_string_t text;
        ecs_u32_t fontSize;
        ecs_f32_t spacing;
        pixio_color_t color;
        pixio_resource_font_t font;
        pixio_font_t sresource;
    } pixio_text_t;

    typedef struct pixio_sprite_t
    {
        pixio_resource_texture_t texture;
        pixio_color_t tint;
        pixio_texture_filter_t filter;
        pixio_rect_t srcRect;
        pixio_rect_t dstRect;
        pixio_texture_t sresource;
    } pixio_sprite_t;

    typedef struct pixio_shape_circle_t
    {
        ecs_f32_t radius;
        ecs_f32_t thickness;
        pixio_color_t color;
    } pixio_shape_circle_t;

    typedef struct pixio_shape_rec_t
    {
        ecs_f32_t width;
        ecs_f32_t height;
        ecs_f32_t roundness;
        pixio_color_t color;
        ecs_f32_t lineWidth;
        pixio_color_t lineColor;
        ecs_f32_t segments;
    } pixio_shape_rec_t;

    extern ECS_COMPONENT_DECLARE(pixio_size_t);
    extern ECS_COMPONENT_DECLARE(pixio_entity_t);
    extern ECS_COMPONENT_DECLARE(pixio_texture_filter_t);
    extern ECS_COMPONENT_DECLARE(pixio_color_t);
    extern ECS_COMPONENT_DECLARE(pixio_vector2_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_font_t);
    extern ECS_COMPONENT_DECLARE(pixio_resource_texture_t);
    extern ECS_COMPONENT_DECLARE(pixio_script_t);
    extern ECS_COMPONENT_DECLARE(pixio_rect_t);
    extern ECS_COMPONENT_DECLARE(pixio_font_t);
    extern ECS_COMPONENT_DECLARE(pixio_texture_t);
    extern ECS_COMPONENT_DECLARE(pixio_transform_t);
    extern ECS_COMPONENT_DECLARE(pixio_text_t);
    extern ECS_COMPONENT_DECLARE(pixio_sprite_t);
    extern ECS_COMPONENT_DECLARE(pixio_shape_circle_t);
    extern ECS_COMPONENT_DECLARE(pixio_shape_rec_t);

    void pixio_type_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // PIXI_TYPE_H
