#ifndef __GOBU_ENGINE_H__
#define __GOBU_ENGINE_H__
#include <stdio.h>
#include <stdint.h>
#include <glib.h>
#include <gio/gio.h>
#include "thirdparty/flecs/flecs.h"
#include "thirdparty/goburender/raylib.h"
#include "thirdparty/goburender/raymath.h"
#include "thirdparty/binn/binn_json.h"

#if gb_log_error
#define gb_log_error
#endif

#if gb_log_warn
#define gb_log_warn
#endif

#if gb_log_success
#define gb_log_success
#endif

#if gb_log_info
#define gb_log_info
#endif

#define gb_return_if_fail(expr) if(!(expr)) {gb_log_warn(TF("Assertion '%s' failed", #expr)); return;}
#define gb_return_val_if_fail(expr, val) if(!(expr)) {gb_log_warn(TF("Assertion '%s' failed", #expr)); return val;}

#define gb_if_default_value(value, default_value) value = ((value) == 0 ? (default_value) : (value))

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum
    {
        G_FS_TEST_IS_REGULAR = 1 << 0,
        G_FS_TEST_IS_SYMLINK = 1 << 1,
        G_FS_TEST_IS_DIR = 1 << 2,
        G_FS_TEST_IS_EXECUTABLE = 1 << 3,
        G_FS_TEST_EXISTS = 1 << 4
    } GFSTest;

    typedef struct gb_render_phases_t
    {
        ecs_entity_t PreDraw;
        ecs_entity_t Background;
        ecs_entity_t Draw;
        ecs_entity_t PostDraw;
    }gb_render_phases_t;

    typedef struct gb_app_t
    {
        char* title;
        uint32_t width;
        uint32_t height;
        bool no_window;
        // RenderPhases phases;
        int fps;
        bool show_fps;
        bool show_grid;
    }gb_app_t;

    typedef struct gb_info_t
    {
        ecs_string_t name;
        ecs_u8_t type;              // 0: entity empty, 1: sprite, 2: text, 3: tiling sprite...
    }gb_info_t;

    typedef struct gb_engine_t {
        struct {
            bool (*mouse_button_down)(int button);
            bool (*mouse_button_pressed)(int button);
            bool (*mouse_button_released)(int button);
            bool (*key_down)(int key);
            bool (*key_pressed)(int key);
            bool (*key_released)(int key);
            gb_vec2_t(*mouse_position)(void);
            gb_vec2_t(*mouse_delta)(void);
            float (*mouse_wheel)(void);
            void (*mouse_position_set)(gb_vec2_t position);
            void (*mouse_cursor_set)(int cursor);
        }input;

        gb_vec2_t(*screen_to_world)(gb_camera_t camera, gb_vec2_t position);
        gb_vec2_t(*world_to_screen)(gb_camera_t camera, gb_vec2_t position);
    }gb_engine_t;

    extern ECS_COMPONENT_DECLARE(gb_render_phases_t);
    extern ECS_COMPONENT_DECLARE(gb_app_t);
    extern ECS_COMPONENT_DECLARE(gb_info_t);
    extern ECS_COMPONENT_DECLARE(gb_gizmos_t);

#define gb_ecs_world_new ecs_init
#define gb_ecs_entity_set ecs_set
#define gb_ecs_entity_get ecs_get
#define gb_ecs_entity ecs_new_entity
#define gb_world_t ecs_world_t

// --------------------------
// PROJECT MODULE
// --------------------------
    bool gb_project_load_from_file(const char* filename);
    const char* gb_project_get_path(void);

    // --------------------------
    // SETTING PROJECT MODULE
    // --------------------------
    void gb_setting_unload(void);
    const char* gb_setting_project_name(void);
    GKeyFile* gb_setting_project_setting(void);
    void gb_setting_save(void);
    void gb_setting_project_set_name(const char* name);
    void gb_setting_project_set_description(const char* description);
    void gb_setting_project_set_author(const char* author);
    void gb_setting_project_set_license(const char* license);
    void gb_setting_project_set_version(const char* version);
    void gb_setting_project_set_thumbnail(const char* thumbnail);
    void gb_setting_project_set_width(int width);
    void gb_setting_project_set_height(int height);
    void gb_setting_project_set_mode(int mode);
    void gb_setting_project_set_resizable(bool resizable);
    void gb_setting_project_set_borderless(bool borderless);
    void gb_setting_project_set_alwayontop(bool alwayontop);
    void gb_setting_project_set_transparent(bool transparent);
    void gb_setting_project_set_nofocus(bool nofocus);
    void gb_setting_project_set_highdpi(bool highdpi);
    void gb_setting_project_set_vsync(bool vsync);
    void gb_setting_project_set_aspectration(bool aspectration);
    const char* gb_setting_project_name(void);
    const char* gb_setting_project_description(void);
    const char* gb_setting_project_author(void);
    const char* gb_setting_project_license(void);
    const char* gb_setting_project_version(void);
    const char* gb_setting_project_thumbnail(void);
    int gb_setting_project_width(void);
    int gb_setting_project_height(void);
    int gb_setting_project_mode(void);
    bool gb_setting_project_resizable(void);
    bool gb_setting_project_borderless(void);
    bool gb_setting_project_alwayontop(void);
    bool gb_setting_project_transparent(void);
    bool gb_setting_project_nofocus(void);
    bool gb_setting_project_highdpi(void);
    bool gb_setting_project_vsync(void);
    bool gb_setting_project_aspectration(void);

    // --------------------------
    // UTILITY MODULE
    // --------------------------
    char* gb_path_relative_content(const char* path);
    char* gb_path_join_relative_content(const char* path);
    char* gb_path_normalize(const char* path);
    char* gb_path_join(const char* first_path, ...);
    char* gb_path_basename(const char* filename);
    char* gb_path_dirname(const char* filename);
    char* gb_path_current_dir(void);
    bool gb_path_exist(const char* filename);
    bool gb_fs_copyc(GFile* src, GFile* dest, GError** error);
    bool gb_fs_write_file(const char* filename, const char* contents);
    bool gb_fs_mkdir(const char* pathname);
    bool gb_fs_file_exist(const char* filename);
    bool gb_fs_test(const char* filename, GFSTest test);
    const char* gb_fs_extname(const char* filename);
    char* gb_fs_get_name(const char* filename, bool with_ext);
    bool gb_fs_is_extension(const char* filename, const char* ext);
    gchar* gb_fs_get_contents(const char* filename, gsize* length);
    char* gb_strdups(const char* format, ...);
    char* gb_strdup(const char* str);
    char* gb_str_tolower(const char* text);
    char* gb_str_uppercase(const char* text);
    bool gb_str_isequal(const char* text1, const char* text2);
    char** gb_str_split(const char* string, const char* delimiter);
    void gb_str_split_free(char** str_array);
    char* gb_str_trim(char* str);
    char* gb_str_remove_spaces(char* str);
    char* gb_str_sanitize(char* str);
    char* gb_str_replace(const char* str, const char* find, const char* replace);

    // --------------------------
    // RENDERING MODULE
    // --------------------------
    void gb_rendering_draw_grid_2d(int slices, float spacing, gb_color_t color);
    void gb_rendering_draw_gismos(gb_transform_t transform, gb_bounding_t bonding_box);
    void gb_rendering_draw_rect(gb_shape_rect_t rect);
    void gb_rendering_draw_circle(gb_shape_circle_t circle);
    void gb_rendering_draw_sprite(gb_sprite_t sprite);
    void gb_rendering_draw_text(gb_text_t text);

    // --------------------------
    // RESOURCE MODULE
    // --------------------------
    const char* gb_resource_key_normalize(const char* path);
    const char* gb_resource_set(gb_world_t* world, const char* path);
    const gb_resource_t* gb_resource(gb_world_t* world, const char* key);
    bool gb_resource_remove(gb_world_t* world, const char* key);

    // --------------------------
    // ECS MODULE
    // --------------------------

#define gb_ecs_vec_remove_t(vec, T, elem) \
    gb_ecs_vec_remove(vec, ECS_SIZEOF(T), elem)

#define gb_ecs_vec_swap_t(vec, T, elem, elem2) \
    gb_ecs_vec_swap(vec, ECS_SIZEOF(T), elem, elem2)

#define gb_ecs_transform(x, y) (gb_transform_t){ .position = (gb_vec2_t){ x, y}, .scale = Vector2One(), .rotation = 0.0f, .origin = (gb_vec2_t){0.5f, 0.5f} }

    ecs_entity_t gb_ecs_entity_new(gb_world_t* world, ecs_entity_t parent, const char* name, const gb_transform_t t);
    void gb_ecs_entity_set_parent(gb_world_t* world, ecs_entity_t parent, ecs_entity_t entity);
    ecs_entity_t gb_ecs_entity_get_parent(gb_world_t* world, ecs_entity_t entity);
    const char* gb_ecs_entity_get_name(gb_world_t* world, ecs_entity_t entity);
    ecs_entity_t gb_ecs_entity_set_name(gb_world_t* world, ecs_entity_t entity, const char* name);
    void gb_ecs_vec_remove(ecs_vec_t* v, ecs_size_t size, int32_t index);
    void gb_ecs_vec_swap(ecs_vec_t* v, ecs_size_t size, int32_t index_a, int32_t index_b);

            // --------------------------
            // gb_sprite_t API
            // --------------------------
    gb_sprite_t gb_sprite_deserialize(binn* fsprite);
    binn* gb_sprite_serialize(gb_sprite_t sprite);
    gb_sprite_t gb_sprite_init(void);

            // --------------------------
            // gb_animate_sprite_t API
            // --------------------------
    void gb_animate_sprite_set(gb_animate_sprite_t* animated, const char* name);
    void gb_animate_sprite_stop(gb_animate_sprite_t* animated);
    void gb_animate_sprite_pause(gb_animate_sprite_t* animated);
    void gb_animate_sprite_play(gb_animate_sprite_t* animated);
    bool gb_animate_sprite_is_playing(gb_animate_sprite_t* animated);
    bool gb_animate_sprite_is_looping(gb_animate_sprite_t* animated);

        // --------------------------
        // WindowAPP MODULE
        // --------------------------
    gb_world_t* gb_app_init(gb_app_t* app);
    void gb_app_main(gb_world_t* world);
    void gb_app_progress(gb_world_t* world);

#ifdef __cplusplus
}
#endif

#endif

