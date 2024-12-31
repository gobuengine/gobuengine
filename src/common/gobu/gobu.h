
#ifndef GOBU_H
#define GOBU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "externs/flecs.h"
#include "math.h"
#include "input.h"

#define GOBU_VERSION "0.1.0"

#define GFXB_LINES 0x0001      // LINES
#define GFXB_TRIANGLES 0x0004  // TRIANGLES
#define GFXB_QUADS 0x0007      // QUADS
#define GFXB_POINTS 0x0008     // QUADS
#define GFXB_MODELVIEW 0x1700  // MODELVIEW
#define GFXB_PROJECTION 0x1701 // PROJECTION
#define GFXB_TEXTURE 0x1702    // TEXTURE

// Some Basic Colors
#define GRIDBLACK \
    (buder_color_t) { 20, 20, 20, 255 }
#define LIGHTGRAY \
    (buder_color_t) { 211, 211, 211, 255 }
#define GRAY \
    (buder_color_t) { 150, 150, 150, 255 }
#define DARKGRAY \
    (buder_color_t) { 90, 90, 90, 255 }

#define YELLOW \
    (buder_color_t) { 255, 245, 60, 255 }
#define GOLD \
    (buder_color_t) { 255, 200, 30, 255 }
#define ORANGE \
    (buder_color_t) { 255, 140, 20, 255 }

#define PINK \
    (buder_color_t) { 255, 120, 200, 255 }
#define RED \
    (buder_color_t) { 230, 50, 60, 255 }
#define MAROON \
    (buder_color_t) { 175, 40, 60, 255 }

#define GREEN \
    (buder_color_t) { 0, 220, 70, 255 }
#define LIME \
    (buder_color_t) { 50, 180, 50, 255 }
#define DARKGREEN \
    (buder_color_t) { 0, 100, 40, 255 }

#define SKYBLUE \
    (buder_color_t) { 120, 200, 255, 255 }
#define BLUE \
    (buder_color_t) { 0, 130, 245, 255 }
#define DARKBLUE \
    (buder_color_t) { 0, 90, 170, 255 }

#define PURPLE \
    (buder_color_t) { 190, 130, 255, 255 }
#define VIOLET \
    (buder_color_t) { 140, 70, 200, 255 }
#define DARKPURPLE \
    (buder_color_t) { 115, 40, 140, 255 }

#define BEIGE \
    (buder_color_t) { 225, 200, 150, 255 }
#define BROWN \
    (buder_color_t) { 130, 100, 75, 255 }
#define DARKBROWN \
    (buder_color_t) { 85, 65, 50, 255 }

#define WHITE \
    (buder_color_t) { 255, 255, 255, 255 }
#define BLACK \
    (buder_color_t) { 0, 0, 0, 255 }
#define BLANK \
    (buder_color_t) { 0, 0, 0, 0 }

#define MAGENTA \
    (buder_color_t) { 255, 0, 255, 255 }
#define BUDEWHITE \
    (buder_color_t) { 240, 240, 240, 255 }

typedef struct gfx_backend_t gfx_backend_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} gobu_color_t;

typedef struct
{
    float x;
    float y;
    float w;
    float h;
} gobu_rect_t;

typedef struct
{
    float x;
    float y;
} gobu_vec2_t;

typedef struct
{
    uint32_t id;
} gobu_image_t;

typedef struct
{
    uint32_t id;
    uint32_t sampler;
    int width;
    int height;
} gobu_texture_t;

typedef struct buder_font_t
{
    int id;
} buder_font_t;

typedef struct
{
    gfx_backend_t *backend;
    int width;
    int height;
} gbContext;

// GFXBACKEND
gfx_backend_t *gfxb_create(void);
void gfxb_destroy(gfx_backend_t *gfx_backend);
void gfxb_present(gfx_backend_t *gfx_backend, int width, int height);
void gfxb_viewport_color(gfx_backend_t *gfx_backend, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void gfxb_begin(int mode);
void gfxb_end(void);
void gfxb_vertex2f_t2f(float x, float y, float u, float v);
void gfxb_vertex2f(float x, float y);
void gfxb_color4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void gfxb_matrix_mode(int mode);
void gfxb_enable_texture(void);
void gfxb_disable_texture(void);
gobu_texture_t gfxb_create_texture(unsigned char *data, int width, int height);
void gfxb_destroy_texture(gobu_texture_t texture);
void gfxb_texture_apply(gobu_texture_t texture);
void gfxb_load_identity(void);
void gfxb_viewport(int x, int y, int width, int height);
void gfxb_scissor(int x, int y, int width, int height);
void gfxb_ortho(float left, float right, float bottom, float top, float near, float far);
void gfxb_push_transform(void);
void gfxb_pop_transform(void);
void gfxb_translate(float x, float y);
void gfxb_scale(float x, float y);
void gfxb_rotate(float angle);
void gfxb_layer(int layer);

// texture
gobu_texture_t gobu_load_texture(const char *filename);
bool gobu_texture_is_valid(gobu_texture_t texture);
void gobu_free_texture(gobu_texture_t texture);
void gobu_draw_texture_pro(gobu_texture_t texture, gobu_rect_t src, gobu_rect_t dst, gobu_vec2_t scale, gobu_vec2_t origin, float angle, gobu_color_t tint, int layer_index);
void gobu_draw_texture_rect(gobu_texture_t texture, gobu_rect_t src, gobu_vec2_t position, gobu_color_t tint, int layer_index);
void gobu_draw_texture(gobu_texture_t texture, gobu_vec2_t position, gobu_color_t tint, int layer_index);

// text
// shapes

// GOBU
void gobu_init(gbContext *ctx, int width, int height);
void gobu_destroy(gbContext *ctx);
void gobu_present(gbContext *ctx);

// ECS
ecs_world_t *gobu_ecs_init(void);
void gobu_ecs_free(ecs_world_t *ecs);
void gobu_ecs_process(ecs_world_t *ecs, float deltaTime);
void gobu_ecs_save_to_file(ecs_world_t *world, const char *filename);
bool gobu_ecs_load_from_file(ecs_world_t *world, const char *filename);

ecs_entity_t gobu_ecs_entity_new_low(ecs_world_t *world, ecs_entity_t parent);
ecs_entity_t gobu_ecs_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name);
void gobu_ecs_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent);
ecs_entity_t gobu_ecs_get_parent(ecs_world_t *world, ecs_entity_t entity);
bool gobu_ecs_has_parent(ecs_world_t *world, ecs_entity_t entity);
ecs_entity_t gobu_ecs_clone(ecs_world_t *world, ecs_entity_t entity);
bool gobu_ecs_is_enabled(ecs_world_t *world, ecs_entity_t entity);
void gobu_ecs_add_component_name(ecs_world_t *world, ecs_entity_t entity, const char *component);

ecs_entity_t gobu_ecs_observer(ecs_world_t *world, ecs_entity_t event, ecs_iter_action_t callback, void *ctx);
void gobu_ecs_emit(ecs_world_t *world, ecs_entity_t event, ecs_entity_t entity);

ecs_entity_t gobu_scene_new(ecs_world_t *world, const char *name);
ecs_entity_t gobu_scene_clone(ecs_world_t *world, ecs_entity_t entity);
void gobu_scene_delete(ecs_world_t *world, ecs_entity_t entity);
void gobu_scene_open(ecs_world_t *world, ecs_entity_t entity);
ecs_entity_t gobu_scene_get_open(ecs_world_t *world);
void gobu_scene_reload(ecs_world_t *world);
int gobu_scene_count(ecs_world_t *world);
ecs_entity_t gobu_scene_get_by_name(ecs_world_t *world, const char *name);
void gobu_scene_rename(ecs_world_t *world, ecs_entity_t entity, const char *name);

// UTIL
bool gobu_util_path_isdir(const char *pathname);
bool gobu_util_path_create(const char *pathname);
const char *gobu_util_path_user(void);
char *gobu_util_path_normalize(const char *path);
char *gobu_util_path_build(const char *first_path, ...);
char *gobu_util_path_basename(const char *filename);
char *gobu_util_path_dirname(const char *filename);
char *gobu_util_path_current_dir(void);
bool gobu_util_path_exist(const char *filename);
char *gobu_util_string_format(const char *format, ...);
char *gobu_util_string(const char *str);
char *gobu_util_string_tolower(const char *text);
char *gobu_util_string_uppercase(const char *text);
bool gobu_util_string_isequal(const char *text1, const char *text2);
char **gobu_util_split(const char *string, const char *delimiter);
void gobu_util_splitFree(char **str_array);
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

#endif // GOBU_H
