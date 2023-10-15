#define SOKOL_IMPL
#include "bugo.h"
#include "external/tmp_render/raylib.h"
#include "external/tmp_render/rlgl.h"

typedef struct
{
    struct
    {
        ecs_world_t *world;
    } ecs;

    struct
    {
        char *path;
        char *name;
    } project;
} CoreData;

static CoreData CORE = {0};

static void bugo_ecs_component_init(void);
static void ViewportSize(int width, int height);

void bugo_init(int width, int height)
{
    InitWindow(width, height, NULL);

    bugo_ecs_init();
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

const gchar *bugo_json_stringify(binn *b)
{
    return binn_serialize(b);
}

binn *bugo_json_parse(gchar *json_string)
{
    return binn_serialize_load(json_string);
}

binn *bugo_json_load_from_file(const gchar *filename)
{
    return binn_serialize_from_file(filename);
}

bool bugo_json_save_to_file(binn *b, const gchar *filename)
{
    return binn_deserialize_from_file(b, filename);
}

//------------------------------------------------------------------------------------
// PROJECT
//------------------------------------------------------------------------------------

gboolean gobu_project_load(const gchar *path)
{
    gboolean test = g_file_test(path, G_FILE_TEST_EXISTS);
    if (test == TRUE)
    {
        CORE.project.path = g_path_get_dirname(path);
        CORE.project.name = g_basename(CORE.project.path);
    }
    return test;
}

const gchar *bugo_project_get_path(void)
{
    return g_strdup(CORE.project.path);
}

const gchar *bugo_project_get_name(void)
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
    return (Color){fr, fg, fb};
}

//------------------------------------------------------------------------------------
// Math
//------------------------------------------------------------------------------------

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
    return (Vector2){0.0f, 0.0f};
}

Vector2 bugo_math_vector2_one(void)
{
    return (Vector2){1.0f, 1.0f};
}

//------------------------------------------------------------------------------------
// File
//------------------------------------------------------------------------------------

const gchar *bugo_file_get_name(const gchar *filepath)
{
    return g_basename(filepath);
}

const gchar *bugo_file_get_name_without_ext(const gchar *filepath)
{
    gchar **name_split = g_strsplit(g_basename(filepath), ".", -1);
    gchar *out = g_strdup(name_split[0]);
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

void bugo_draw_text(const char *text, int x, int y, int size, Color color)
{
    DrawText(text, x, y, size, color);
}

//------------------------------------------------------------------------------------
// Load any
//------------------------------------------------------------------------------------

Texture2D bugo_load_texture(const char *filename)
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

void bugo_ecs_init(void)
{
    CORE.ecs.world = ecs_init();
    bugo_ecs_component_init();
    printf("INFO: ECS initialize\n");
}

ecs_world_t *bugo_ecs_world(void)
{
    return CORE.ecs.world;
}

int32_t bugo_ecs_world_count(void)
{
    return ecs_count_id(CORE.ecs.world, bugo_ecs_get_renderer_id());
}

static void bugo_ecs_component_init(void)
{
    ecs_world_t *world = CORE.ecs.world;

    bugo_ecs_init_color(world);
    bugo_ecs_init_vector2(world);
    bugo_ecs_init_rectangle(world);
    bugo_ecs_init_camera2d(world);
    // contienen systems
    bugo_ecs_init_renderer(world);
    bugo_ecs_init_shape_rect(world);
    bugo_ecs_init_sprite(world);
}
