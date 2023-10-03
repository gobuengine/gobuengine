/**********************************************************************************
 * gobu.h                                                          *
 **********************************************************************************
 * GOBU ENGINE                                                                    *
 * https://gobuengine.org                                                         *
 **********************************************************************************
 * Copyright (c) 2023 Jhonson Ozuna Mejia                                         *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                                *
 *                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
 * SOFTWARE.                                                                      *
 **********************************************************************************/

#ifndef __GOBU_H__
#define __GOBU_H__
#include <glib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "thirdparty/binn/binn.h"
#include "thirdparty/flecs/flecs.h"

#define GOBU_API

#undef ECS_META_IMPL
#ifndef FLECS_GAME_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#define GobuEcsEntity ecs_entity_t
#define GobuEcsWorld ecs_world_t
#define GobuEcsIter ecs_iter_t

GOBU_API ECS_STRUCT(GobuColor, {
    float r;
    float g;
    float b;
    float a;
});

GOBU_API ECS_STRUCT(GobuTexture, {
    unsigned int id;
    int width;
    int height;
});

GOBU_API ECS_STRUCT(GobuRectangle, {
    float x;
    float y;
    float w;
    float h;
});

GOBU_API ECS_STRUCT(GobuVec2, {
    float x;
    float y;
});

GOBU_API ECS_STRUCT(GobuCamera, {
    GobuVec2 target;
    GobuVec2 offset;
    float rotation;
    float zoom;
});

GOBU_API ECS_STRUCT(GobuSprite, {
    GobuTexture texture;
    GobuColor color;
    float opacity;
    bool flipX;
    bool flipY;
    // config spriteSheet
    uint32_t Hframes;
    uint32_t Vframes;
    uint32_t frame;
    // hide params
    GobuRectangle dest_rect;
    GobuRectangle src_rect;
});

GOBU_API ECS_STRUCT(GobuEcsComponentRender, {
    bool enable;
    char *name;
    GobuVec2 position;
    GobuVec2 scale;
    float rotation;
});

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

/**
 * Variable externa que representa la entidad de actualización de escenario.
 *
 * Esta variable externa se utiliza para acceder a la entidad que controla la actualización
 * del escenario en el contexto de Gobu. La entidad puede estar definida en otro archivo.
 */
extern GobuEcsEntity GobuEcsOnEvent;
extern GobuEcsEntity GobuEcsOnUpdate;
extern GobuEcsEntity GobuEcsOnPreRender;
extern GobuEcsEntity GobuEcsOnUiRender;
extern GobuEcsEntity GobuEcsOnRender;
extern GobuEcsEntity GobuEcsOnPostRender;
extern GobuEcsEntity GobuEcsEntityWorldRoot;

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

GOBU_API const char *gobu_json_stringify(binn *b);
GOBU_API binn *gobu_json_parse(char *json_string);
GOBU_API binn *gobu_json_load_from_file(const char *filename);
GOBU_API bool gobu_json_save_to_file(binn *b, const char *filename);

GOBU_API gboolean gobu_project_load(const gchar *path);
GOBU_API const char *gobu_project_get_path(void);
GOBU_API const char *gobu_project_get_name(void);

GOBU_API GobuCamera gobu_camera_new(void);
GOBU_API void gobu_camera_set_main(GobuCamera *camera);
GOBU_API void gobu_camera_set_position(GobuCamera camera, float x, float y);
GOBU_API void gobu_camera_set_offset(GobuCamera camera, float x, float y);
GOBU_API void gobu_camera_set_rotation(GobuCamera camera, float rotation);
GOBU_API void gobu_camera_set_zoom(GobuCamera camera, float zoom);

GOBU_API void gobu_render_init(void);
GOBU_API void gobu_render_shutdown(void);
GOBU_API void gobu_render_frame_begin(int width, int height, GobuColor color);
GOBU_API void gobu_render_frame_end(int width, int height);
GOBU_API void gobu_render_viewport(int x, int y, int width, int height);
GOBU_API void gobu_render_project(float left, float right, float top, float bottom);
GOBU_API void gobu_render_clear_color(GobuColor color);
GOBU_API void gobu_render_clear(void);
GOBU_API void gobu_render_reset_color(void);
GOBU_API void gobu_render_set_color(GobuColor color);
GOBU_API void gobu_render_set_rotate(float theta, float x, float y);
GOBU_API void gobu_render_set_scale(float sx, float sy, float x, float y);
GOBU_API void gobu_render_set_translate(float x, float y);

GOBU_API void gobu_shape_draw_filled_rect(GobuRectangle rect, GobuVec2 scale, GobuVec2 origin, float rotation, GobuColor color);
GOBU_API void gobu_shape_draw_checkboard(int width, int height, int screen_width, int screen_height);

GOBU_API void gobu_texture_draw_texture(GobuTexture texture, GobuRectangle source, GobuRectangle dest, GobuColor tint);
GOBU_API GobuTexture gobu_texture_load_file(const char *filename);
GOBU_API void gobu_texture_free(GobuTexture texture);

GOBU_API GobuColor gobu_color_rgb_to_color(uint8_t r, uint8_t g, uint8_t b);
GOBU_API GobuColor gobu_color_rgba_to_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
GOBU_API GobuColor gobu_color_random_rgb(uint8_t r, uint8_t g, uint8_t b);

GOBU_API float gobu_math_random(float number);

GOBU_API void gobu_ecs_world_init(void);
GOBU_API GobuEcsWorld *gobu_ecs_world_get(void);
GOBU_API void gobu_ecs_world_free(void);
GOBU_API void gobu_ecs_world_progress(void);
GOBU_API void gobu_ecs_world_run(GobuEcsEntity system, float delta);
GOBU_API GobuEcsEntity gobu_ecs_entity_new(const char *name);
GOBU_API GobuEcsIter gobu_ecs_entity_get_children(GobuEcsEntity entity);
GOBU_API const char *gobu_ecs_entity_get_name(GobuEcsEntity entity);
GOBU_API void gobu_ecs_entity_delete(GobuEcsEntity entity);
GOBU_API bool gobu_ecs_iter_next(GobuEcsIter *iter);

#endif // __GOBU_H__