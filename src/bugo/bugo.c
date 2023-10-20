#define SOKOL_IMPL
#include "bugo.h"
#include "external/tmp_render/raylib.h"
#include "external/tmp_render/rlgl.h"

#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>

typedef struct
{
    struct
    {
        ecs_world_t* world;
    } ecs;

    struct
    {
        char* path;
        char* name;
    } project;

    struct
    {
        lua_State* ctx;
    }script;
} CoreData;

static CoreData CORE = { 0 };

static void bugo_ecs_component_init(void);
static void ViewportSize(int width, int height);
static void bugo_script_init(void);
static void bugo_script_set_api(void);

void bugo_init(int width, int height)
{
    InitWindow(width, height, NULL);

    bugo_ecs_init();
    bugo_script_init();
}

void bugo_gfx_render_begin(int width, int height)
{
    ViewportSize(width, height);
    BeginDrawing();
}

void bugo_gfx_render_end(int width, int height)
{
    EndDrawing();
}

void bugo_gfx_render_clear_color(Color color)
{
    ClearBackground(color);
}

void bugo_gfx_begin_mode(int mode)
{
    rlBegin(mode);
}

void bugo_gfx_end_mode(void)
{
    rlEnd();
}

void bugo_gfx_color3f(float r, float g, float b)
{
    rlColor3f(r, g, b);
}

void bugo_gfx_vert2f(float x, float y)
{
    rlVertex2f(x, y);
}

void bugo_gfx_vert3f(float x, float y, float z)
{
    rlVertex3f(x, y, z);
}
//------------------------------------------------------------------------------------
// BINN, BINN JSON
//------------------------------------------------------------------------------------

const gchar* bugo_json_stringify(binn* b)
{
    return binn_serialize(b);
}

binn* bugo_json_parse(gchar* json_string)
{
    return binn_serialize_load(json_string);
}

binn* bugo_json_load_from_file(const gchar* filename)
{
    return binn_serialize_from_file(filename);
}

bool bugo_json_save_to_file(binn* b, const gchar* filename)
{
    return binn_deserialize_from_file(b, filename);
}

//------------------------------------------------------------------------------------
// PROJECT
//------------------------------------------------------------------------------------

gboolean gobu_project_load(const gchar* path)
{
    gboolean test = g_file_test(path, G_FILE_TEST_EXISTS);
    if (test == TRUE)
    {
        CORE.project.path = g_path_get_dirname(path);
        CORE.project.name = g_basename(CORE.project.path);
    }
    return test;
}

const gchar* bugo_project_get_path(void)
{
    return g_strdup(CORE.project.path);
}

const gchar* bugo_project_get_name(void)
{
    return g_strdup(CORE.project.name);
}

//------------------------------------------------------------------------------------
// COLOR
//------------------------------------------------------------------------------------

Color bugo_color_random_rgb(Color color)
{
    unsigned char fr = (unsigned char)bugo_math_random((float)color.r);
    unsigned char fg = (unsigned char)bugo_math_random((float)color.g);
    unsigned char fb = (unsigned char)bugo_math_random((float)color.b);
    return (Color) { fr, fg, fb };
}

//------------------------------------------------------------------------------------
// Mouse
//------------------------------------------------------------------------------------

int bugo_mouse_get_x(void)
{
    return GetMouseX();
}

int bugo_mouse_get_y(void)
{
    return GetMouseY();
}

Vector2 bugo_mouse_get_position(void)
{
    return GetMousePosition();
}

Vector2 bugo_mouse_get_delta(void)
{
    return GetMouseDelta();
}

float bugo_mouse_get_wheel_move(void)
{
    return GetMouseWheelMove();
}

bool bugo_mouse_button_is_pressed(int button)
{
    return IsMouseButtonPressed(button);
}

bool bugo_mouse_button_is_down(int button)
{
    return IsMouseButtonDown(button);
}

bool bugo_mouse_button_is_released(int button)
{
    return IsMouseButtonReleased(button);
}

bool bugo_mouse_button_is_up(int button)
{
    return IsMouseButtonUp(button);
}

//------------------------------------------------------------------------------------
// Math
//------------------------------------------------------------------------------------

Vector2 bugo_screen_to_world_2d(Vector2 position, ComponentCamera camera)
{
    Camera2D cam = (Camera2D){ .offset = camera.offset, .target = camera.target, .rotation = camera.rotation, .zoom = camera.zoom };
    return GetScreenToWorld2D(position, cam);
}

Vector2 bugo_world_to_screen_2d(Vector2 position, ComponentCamera camera)
{
    Camera2D cam = (Camera2D){ .offset = camera.offset, .target = camera.target, .rotation = camera.rotation, .zoom = camera.zoom };
    return GetWorldToScreen2D(position, cam);
}

float bugo_math_random(float number)
{
    return (((float)rand() / (float)(RAND_MAX)) * number);
}

int bugo_math_random_range(int min, int max)
{
    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }
    return (rand() % (abs(max - min) + 1) + min);
}

Vector2 bugo_math_vector2_zero(void)
{
    return (Vector2) { 0.0f, 0.0f };
}

Vector2 bugo_math_vector2_one(void)
{
    return (Vector2) { 1.0f, 1.0f };
}

Vector2 bugo_math_vector2_add(Vector2 v1, Vector2 v2)
{
    return (Vector2) { v1.x + v2.x, v1.y + v2.y };
}

Vector2 bugo_math_vector2_scale(Vector2 v, float scale)
{
    return (Vector2) { v.x* scale, v.y* scale };
}

//------------------------------------------------------------------------------------
// File
//------------------------------------------------------------------------------------

const gchar* bugo_file_get_name(const gchar* filepath)
{
    return g_basename(filepath);
}

const gchar* bugo_file_get_name_without_ext(const gchar* filepath)
{
    gchar** name_split = g_strsplit(g_basename(filepath), ".", -1);
    gchar* out = g_strdup(name_split[0]);
    g_strfreev(name_split);
    return out;
}

//------------------------------------------------------------------------------------
// Shape
//------------------------------------------------------------------------------------

void bugo_draw_shape_rect(Rectangle rec, Vector2 origin, float rotation, Color color)
{
    DrawRectanglePro(rec, origin, rotation, color);
}

void bugo_draw_shape_grid(int slices, float spacing)
{
    int halfSlices = slices / 2;

    bugo_gfx_begin_mode(GOBU_MODE_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        if (i == 0)
        {
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
        }
        else
        {
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
        }

        bugo_gfx_vert2f((float)i * spacing, (float)-halfSlices * spacing);
        bugo_gfx_vert2f((float)i * spacing, (float)halfSlices * spacing);

        bugo_gfx_vert2f((float)-halfSlices * spacing, (float)i * spacing);
        bugo_gfx_vert2f((float)halfSlices * spacing, (float)i * spacing);
    }
    bugo_gfx_end_mode();
}

//------------------------------------------------------------------------------------
// Texture
//------------------------------------------------------------------------------------

void bugo_draw_texture(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint)
{
    DrawTexturePro(texture, source, dest, origin, rotation, tint);
}

//------------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------------

void bugo_draw_text(const char* text, int x, int y, int size, Color color)
{
    DrawText(text, x, y, size, color);
}

//------------------------------------------------------------------------------------
// Load any
//------------------------------------------------------------------------------------

Texture2D bugo_load_texture(const char* filename)
{
    return LoadTexture(filename);
}

//------------------------------------------------------------------------------------
// ECS & COMPONENT DEFAULT
//------------------------------------------------------------------------------------

void bugo_ecs_progress(float delta)
{
    ecs_progress(CORE.ecs.world, delta);
}

ecs_entity_t bugo_ecs_entity_new(Vector2 position, Vector2 scale, float rotation, Vector2 origin)
{
    ecs_entity_t e = ecs_new_id(CORE.ecs.world);
    bugo_ecs_set_renderer(e, &(ComponentRenderer){
        .active = true, .visible = true,
            .position = position, .scale = scale, .rotation = rotation, .origin = origin,
            .zindex = 0});
    return e;
}

void bugo_ecs_entity_delete(ecs_entity_t entity)
{
    ecs_delete(CORE.ecs.world, entity);
}

const void* bugo_ecs_get(ecs_entity_t entity, ecs_entity_t component)
{
    return ecs_get_id(CORE.ecs.world, entity, component);
}

void bugo_ecs_init(void)
{
    CORE.ecs.world = ecs_init();
    bugo_ecs_component_init();
    printf("INFO: ECS initialize\n");
}

ecs_world_t* bugo_ecs_world(void)
{
    return CORE.ecs.world;
}

int32_t bugo_ecs_world_count(void)
{
    return ecs_count_id(CORE.ecs.world, bugo_ecs_get_renderer_id());
}

static void bugo_ecs_component_init(void)
{
    ecs_world_t* world = CORE.ecs.world;

    bugo_ecs_init_phaser(world);
    bugo_ecs_init_color(world);
    bugo_ecs_init_vector2(world);
    bugo_ecs_init_rectangle(world);
    bugo_ecs_init_camera2d(world);
    // bugo_ecs_init_stage(world);
    // bugo_ecs_init_camera_manager(world);
    // contienen systems
    bugo_ecs_init_renderer(world);
    bugo_ecs_init_shape_rect(world);
    bugo_ecs_init_sprite(world);

    bugo_ecs_init_lua(world);
}

//------------------------------------------------------------------------------------
// LUA SCRIPT
//------------------------------------------------------------------------------------

static void bugo_script_init(void)
{
    CORE.script.ctx = luaL_newstate();
    luaL_openlibs(CORE.script.ctx);
    bugo_script_set_api();
}

static void bugo_system_on_lua_call(ecs_iter_t* it)
{
    lua_rawgeti(CORE.script.ctx, LUA_REGISTRYINDEX, it->ctx);
    lua_pushlightuserdata(CORE.script.ctx, it);

    if (lua_pcall(CORE.script.ctx, 1, 0, 0) == LUA_OK)
        lua_pop(CORE.script.ctx, lua_gettop(CORE.script.ctx));
}

static int iter_count(lua_State* L) {
    ecs_iter_t* it = lua_touserdata(L, 1);
    lua_pushinteger(L, it->count);
    return 1;
}

static int wrap_ecs_field(lua_State* L) {
    ecs_iter_t* it = lua_touserdata(L, 1);
    int column = lua_tointeger(L, 2);
    ComponentRenderer* r = ecs_field(it, ComponentRenderer, column);
    lua_pushlightuserdata(L, r);
    return 1;
}

static int test_set_rotate(lua_State* L) {
    ComponentRenderer* r = lua_touserdata(L, 1);
    int column = lua_tointeger(L, 2);
    float value = lua_tonumber(L, 3);
    r[column].rotation = value;
    return 0;
}

static int test_get_rotate(lua_State* L) {
    ComponentRenderer* r = lua_touserdata(L, 1);
    int column = lua_tointeger(L, 2);
    lua_pushnumber(L, r[column].rotation);
    return 1;
}

ecs_entity_t systems[100];
int idx_sys = 0;

void bugo_script_reset(void)
{
    for (int i = 0; i < idx_sys; i++) {
        ecs_world_t* world = bugo_ecs_world();
        ecs_delete(world, systems[i]);
    }
}

static int Engine_System(lua_State* L)
{
    BugoEcsPhaser phaser_n;
    ecs_world_t* world = bugo_ecs_world();

    // parametros func
    phaser_n = luaL_checkinteger(L, 1);
    if (phaser_n == PHASER_NONE)return 0;

    lua_pushvalue(L, 2);
    int func = luaL_ref(L, LUA_REGISTRYINDEX);

    // Create system
    systems[idx_sys] = ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = {ecs_dependson(bugo_ecs_get_phaser(phaser_n))},
        }),
        .query.filter.terms = {
            {.id = bugo_ecs_get_renderer_id()},
        },
        .callback = bugo_system_on_lua_call,
        .ctx = func,
    });

    idx_sys++;

    printf("INFO: Create System\n");

    return 0;
}

static void bugo_script_set_api(void)
{
    const struct luaL_Reg reg_fun[] = {
        {"System", Engine_System},
        {"get_entitys", iter_count},
        {"get_component", wrap_ecs_field},
        {"set_rotate", test_set_rotate},
        {"get_rotate", test_get_rotate},
        {NULL, NULL}
    };

    lua_newtable(CORE.script.ctx);
    luaL_setfuncs(CORE.script.ctx, &reg_fun, 0);
    lua_setglobal(CORE.script.ctx, "Engine");
}

bool bugo_script_string_load(const char* str)
{
    printf("INFO: Script load\n");
    if (luaL_loadstring(CORE.script.ctx, str) == LUA_OK) {
        if (lua_pcall(CORE.script.ctx, 0, 0, 0) == LUA_OK)
            lua_pop(CORE.script.ctx, lua_gettop(CORE.script.ctx));
        else return false;
    }else return false;
    return true;
}

char *bugo_script_traceback(void)
{
    return lua_tostring(CORE.script.ctx, -1);
}
