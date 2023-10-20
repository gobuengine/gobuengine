/**********************************************************************************
 * gobu.h                                                                         *
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

#ifndef __BUGO_H__
#define __BUGO_H__
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <glib.h>
#include "external/tmp_render/raylib.h"
#include "external/binn_json.h"
#include "external/flecs.h"
#include "components/component.h"

#define GOBU_MODE_LINES 0x0001     // GL_LINES
#define GOBU_MODE_TRIANGLES 0x0004 // GL_TRIANGLES
#define GOBU_MODE_QUADS 0x0007     // GL_QUADS

//------------------------------------------------------------------------------------
//  Component
//------------------------------------------------------------------------------------
typedef struct ComponentCamera ComponentCamera;

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

// Inicializamos el context
void bugo_init(int width, int height);
void bugo_gfx_render_begin(int width, int height);
void bugo_gfx_render_end(int width, int height);
void bugo_gfx_render_clear_color(Color color);
void bugo_gfx_begin_mode(int mode);
void bugo_gfx_end_mode(void);
void bugo_gfx_color3f(float r, float g, float b);
void bugo_gfx_vert2f(float x, float y);
void bugo_gfx_vert3f(float x, float y, float z);

void bugo_draw_shape_rect(Rectangle rec, Vector2 origin, float rotation, Color color);
void bugo_draw_shape_grid(int slices, float spacing);
void bugo_draw_texture(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
void bugo_draw_text(const char* text, int x, int y, int size, Color color);

Texture2D bugo_load_texture(const char* filename);

const gchar* bugo_json_stringify(binn* b);
binn* bugo_json_parse(gchar* json_string);
binn* bugo_json_load_from_file(const gchar* filename);
bool bugo_json_save_to_file(binn* b, const gchar* filename);

gboolean gobu_project_load(const gchar* path);
const gchar* bugo_project_get_path(void);
const gchar* bugo_project_get_name(void);

Color bugo_color_random_rgb(Color color);

int bugo_mouse_get_x(void);
int bugo_mouse_get_y(void);
Vector2 bugo_mouse_get_position(void);
Vector2 bugo_mouse_get_delta(void);
float bugo_mouse_get_wheel_move(void);
bool bugo_mouse_button_is_pressed(int button);
bool bugo_mouse_button_is_down(int button);
bool bugo_mouse_button_is_released(int button);
bool bugo_mouse_button_is_up(int button);

Vector2 bugo_screen_to_world_2d(Vector2 position, ComponentCamera camera);
Vector2 bugo_world_to_screen_2d(Vector2 position, ComponentCamera camera);
float bugo_math_random(float number);
int bugo_math_random_range(int min, int max);
Vector2 bugo_math_vector2_zero(void);
Vector2 bugo_math_vector2_one(void);
Vector2 bugo_math_vector2_add(Vector2 v1, Vector2 v2);
Vector2 bugo_math_vector2_scale(Vector2 v, float scale);

const gchar* bugo_file_get_name(const gchar* filepath);
const gchar* bugo_file_get_name_without_ext(const gchar* filepath);

void bugo_ecs_progress(float delta);
ecs_entity_t bugo_ecs_entity_new(Vector2 position, Vector2 scale, float rotation, Vector2 origin);
void bugo_ecs_entity_delete(ecs_entity_t entity);
const void* bugo_ecs_get(ecs_entity_t entity, ecs_entity_t component);

void bugo_ecs_init(void);
int32_t bugo_ecs_world_count(void);
ecs_world_t* bugo_ecs_world(void);

bool bugo_script_string_load(const char *str);
void bugo_script_reset(void);
char *bugo_script_traceback(void);

#endif // __BUGO_H__
