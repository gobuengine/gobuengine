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

void ViewportSize(int width, int height);

void bugo_init(int width, int height)
{
    InitWindow(width,height, NULL);
    CORE.ecs.world = ecs_init();
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
    return (rand()%(abs(max - min) + 1) + min);
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
