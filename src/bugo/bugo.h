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
#include "bugo_ecs.h"

#define GOBU_MODE_LINES                                0x0001      // GL_LINES
#define GOBU_MODE_TRIANGLES                            0x0004      // GL_TRIANGLES
#define GOBU_MODE_QUADS                                0x0007      // GL_QUADS

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
void bugo_draw_text(const char *text, int x, int y, int size, Color color);

const gchar *bugo_json_stringify(binn *b);
binn *bugo_json_parse(gchar *json_string);
binn *bugo_json_load_from_file(const gchar *filename);
bool bugo_json_save_to_file(binn *b, const gchar *filename);

gboolean gobu_project_load(const gchar *path);
const gchar *bugo_project_get_path(void);
const gchar *bugo_project_get_name(void);

Color bugo_color_random_rgb(Color color);

float bugo_math_random(float number);
int bugo_math_random_range(int min, int max);
Vector2 bugo_math_vector2_zero(void);
Vector2 bugo_math_vector2_one(void);

const gchar *bugo_file_get_name(const gchar *filepath);
const gchar *bugo_file_get_name_without_ext(const gchar *filepath);

#endif // __BUGO_H__
