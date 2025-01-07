
#ifndef GOBU_H
#define GOBU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#include "gb_type.h"
#include "gb_input.h"
#include "gb_backend.h"
#include "gb_ecs.h"

// MARK: GFXBACKEND
gfx_backend_t *gfxb_viewport_create(void);
void gfxb_destroy(gfx_backend_t *gfx_backend);
void gfxb_viewport_begin(gfx_backend_t *gfx_backend);
void gfxb_viewport_end(gfx_backend_t *gfx_backend);
void gfxb_viewport_resize(gfx_backend_t *gfx_backend, int width, int height);
void gfxb_viewport_color(gfx_backend_t *gfx_backend, uint8_t r, uint8_t g, uint8_t b);
void gfxb_viewport_framebuffer(gfx_backend_t *gfx_backend, int framebuffer_id);
void gfxb_begin(int mode);
void gfxb_end(void);
void gfxb_vertex2f_t2f(float x, float y, float u, float v);
void gfxb_vertex2f(float x, float y);
void gfxb_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void gfxb_matrix_mode(int mode);
void gfxb_enable_texture(void);
void gfxb_disable_texture(void);
gb_texture_t gfxb_create_texture(unsigned char *data, int width, int height);
void gfxb_destroy_texture(gb_texture_t texture);
void gfxb_texture_apply(gb_texture_t texture);
void gfxb_load_identity(void);
void gfxb_viewport(int x, int y, int width, int height);
void gfxb_scissor(int x, int y, int width, int height);
void gfxb_ortho(float l, float r, float b, float t, float n, float f);
void gfxb_push_transform(void);
void gfxb_pop_transform(void);
void gfxb_translate(float x, float y);
void gfxb_scale(float x, float y);
void gfxb_rotate(float angle);
void gfxb_layer(int layer);

// MARK:texture
gb_texture_t gobu_load_texture(const char *filename);
bool gobu_texture_is_valid(gb_texture_t texture);
void gobu_free_texture(gb_texture_t texture);
void gobu_draw_texture_pro(gb_texture_t texture, gb_rect_t src, gb_rect_t dst, gb_vec2_t scale, gb_vec2_t origin, float angle, gb_color_t tint, int layer_index);
void gobu_draw_texture_rect(gb_texture_t texture, gb_rect_t src, gb_vec2_t position, gb_color_t tint, int layer_index);
void gobu_draw_texture(gb_texture_t texture, gb_vec2_t position, gb_color_t tint, int layer_index);

// MARK: shapes
void gobu_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, gb_color_t color, int layer_index);
void gobu_draw_rect(float x, float y, float w, float h, gb_color_t fill_color, gb_color_t outline_color, float outline_thickness, int layer_index);
void gobu_draw_line(float x0, float y0, float x1, float y1, float thickness, gb_color_t color, int layer_index);
void bdr_draw_circle(float x, float y, float radius, gb_color_t fill_color, gb_color_t outline_color, float outline_thickness, int layer_index);
void gobu_draw_grid(int width, int height, int cell_size, gb_color_t color, int layer_index);

// MARK: UTIL
#define gobu_util_path_build(...) gobu_util_path_build_(__VA_ARGS__, NULL)

bool gobu_util_path_isdir(const char *pathname);
bool gobu_util_path_create(const char *pathname);
const char *gobu_util_path_user(void);
char *gobu_util_path_normalize(const char *path);
char *gobu_util_path_build_(const char *first_path, ...);
char *gobu_util_path_basename(const char *filename);
char *gobu_util_path_dirname(const char *filename);
char *gobu_util_path_current_dir(void);
bool gobu_util_path_exist(const char *filename);
char *gobu_util_string_format(const char *format, ...);
char *gobu_util_string(const char *str);
char *gobu_util_string_tolower(const char *text);
char *gobu_util_string_uppercase(const char *text);
bool gobu_util_string_isequal(const char *text1, const char *text2);
char **gobu_util_string_split(const char *string, const char *delimiter);
void gobu_util_string_split_free(char **str_array);
char *gobu_util_string_trim(char *str);
char *gobu_util_string_remove_spaces(char *str);
char *gobu_util_string_sanitize(char *str);
char *gobu_util_string_replace(const char *str, const char *find, const char *replace);
bool gobu_util_copy_file(const char *src, const char *dest);
bool gobu_util_write_text_file(const char *filename, const char *contents);
bool gobu_util_exist_file(const char *filename);
const char *gobu_util_extname(const char *filename);
char *gobu_util_name(const char *filename, bool with_ext);
bool gobu_util_is_extension(const char *filename, const char *ext);
char *gobu_util_read_text_file(const char *filename);

// MARK: UTIL  COLOR
gb_color_t gobu_color_adjust_contrast(gb_color_t color, float mixFactor);

#endif // GOBU_H
