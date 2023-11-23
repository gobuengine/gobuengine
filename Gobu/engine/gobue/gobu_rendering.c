#include "gobu_rendering.h"
#include "gobu_transform.h"
#include "gobu_shapes.h"
#include "gobu_text.h"
#include "gobu_sprite.h"
#include "gobu_input.h"
#include "gobu_camera.h"

#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GWindow);
ECS_COMPONENT_DECLARE(RenderPhases);

static void GobuRendering_CheckExitRequest(ecs_iter_t* it);
static void GobuRendering_Window(ecs_iter_t* it);
static void GobuRendering_BeginDrawing(ecs_iter_t* it);
static void GobuRendering_Drawing(ecs_iter_t* it);
static void GobuRendering_EndDrawing(ecs_iter_t* it);

void GobuRenderingImport(ecs_world_t* world)
{
    ECS_COMPONENT_DEFINE(world, RenderPhases);
    ECS_COMPONENT_DEFINE(world, GWindow);

    ECS_MODULE(world, GobuRendering);
    ECS_IMPORT(world, GobuTransform);
    ECS_IMPORT(world, GobuInputSystem);
    ECS_IMPORT(world, GobuCamera);
    ECS_IMPORT(world, GobuShapes);
    ECS_IMPORT(world, GobuText);
    ECS_IMPORT(world, GobuSprite);

    ecs_entity_t PreDraw = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t Background = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t Draw = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t PostDraw = ecs_new_w_id(world, EcsPhase);

    ecs_singleton_set(world, RenderPhases, { .PreDraw = PreDraw, .Draw = Draw, .PostDraw = PostDraw });

    ecs_add_pair(world, PreDraw, EcsDependsOn, EcsOnStore);
    ecs_add_pair(world, Background, EcsDependsOn, PreDraw);
    ecs_add_pair(world, Draw, EcsDependsOn, Background);
    ecs_add_pair(world, PostDraw, EcsDependsOn, Draw);

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(GWindow)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = GobuRendering_Window
    });

    ECS_SYSTEM(world, GobuRendering_BeginDrawing, PreDraw, GWindow, GCamera);
    ECS_SYSTEM(world, GobuRendering_Drawing, Draw, GPosition, ? GScale, ? GRotation, ? GShapeRec, ? GSprite, ? GText);
    ECS_SYSTEM(world, GobuRendering_EndDrawing, PostDraw, GWindow);
    ECS_SYSTEM(world, GobuRendering_CheckExitRequest, PostDraw, GWindow);
}

void gobu_rendering_init(ecs_world_t* world, GWindow* window)
{
    ecs_entity_t engine = ecs_new_entity(world, "Engine");
    ecs_set(world, engine, GWindow, {
        .title = window->title,         // Establece el título de la ventana
        .width = window->width,         // Establece el ancho de la ventana
        .height = window->height,       // Establece la altura de la ventana
        .viewport = window->viewport,   // Establece el viewport para el renderizado sin ventana
        .fps = window->fps,             // Establece los cuadros por segundo
        .show_fps = window->show_fps,   // Establece si se muestra los FPS
        .show_grid = window->show_grid  // Establece si se muestra la cuadrícula
    });
    ecs_set(world, engine, GCamera, { 0 });
    ecs_set(world, engine, GInputSystem, { 0 });
    SetTargetFPS(window->fps ? window->fps : 60);
}

void gobu_rendering_main(ecs_world_t* world)
{
    while (ecs_progress(world, GetFrameTime())) {}
}

void gobu_rendering_progress(ecs_world_t* world)
{
    ecs_progress(world, GetFrameTime());
}

static void GobuRendering_Window(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    GWindow* win = ecs_field(it, GWindow, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (event == EcsOnSet)
            InitWindow(win->width, win->height, win->title);
        else if (event == EcsOnRemove)
            CloseWindow();
    }
}

static void GobuRendering_CheckExitRequest(ecs_iter_t* it)
{
    for (int i = 0; i < it->count; i++)
    {
        if (WindowShouldClose()) {
            ecs_quit(it->world);
        }
    }
}

static void GobuRendering_BeginDrawing(ecs_iter_t* it)
{
    GWindow* win = ecs_field(it, GWindow, 1);
    GCamera* camera = ecs_field(it, GCamera, 2);

    for (int i = 0; i < it->count; i++)
    {
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera->camera);

        if (win[i].show_grid)
            DrawGrid2d(win[i].width, 48);
    }
}

static void GobuRendering_Drawing(ecs_iter_t* it)
{
    GPosition* post = ecs_field(it, GPosition, 1);
    GScale* scale = ecs_field(it, GScale, 2);
    GRotation* rota = ecs_field(it, GRotation, 3);
    // Draw
    GShapeRec* rect = ecs_field(it, GShapeRec, 4);
    GSprite* sprite = ecs_field(it, GSprite, 5);
    GText* text = ecs_field(it, GText, 6);

    for (int i = 0; i < it->count; i++)
    {
        Vector2 origin = (Vector2){ 0.0f, 0.0f };
        float nrot = (rota) ? rota[i] : 0.0f;
        GPosition npost = (post) ? post[i] : (GPosition) { 0.0f, 0.0f };
        GScale nscale = (scale) ? scale[i] : (GScale) { 1.0f, 1.0f };

        rlPushMatrix();
        {
            rlTranslatef(npost.x, npost.y, 0.0f);
            rlRotatef(nrot, 0.0f, 0.0f, 1.0f);
            rlTranslatef(-origin.x, -origin.y, 0.0f);
            rlScalef(nscale.x, nscale.y, 1.0f);
            {
                if (rect) {
                    DrawRectanglePro((Rectangle) { rect[i].x, rect[i].y, rect[i].width, rect[i].height }, origin, 0.0f, rect[i].color);
                }
                if (sprite) {
                    DrawTexturePro(sprite[i].texture, sprite[i].src, sprite[i].dst, origin, 0.0f, sprite[i].tint);
                }
                if (text) {
                    float fontSize = (text[i].size == 0) ? 20.0f : text[i].size;
                    float spacing = (text[i].spacing == 0.0f) ? (fontSize / 10) : text[i].spacing;
                    Color color = (text[i].color.a == 0) ? WHITE : text[i].color;
                    DrawTextEx(text[i].font, text[i].text, Vector2Zero(), fontSize, spacing, color);
                }
            }
        }
        rlPopMatrix();
        // ecs_iter_t child_it = ecs_children(it->world, it->entities[i]);
        // while (ecs_children_next(&it)) {
        //     printf("it.count: %d\n",child_it.count);
        // }
    }
}

static void GobuRendering_EndDrawing(ecs_iter_t* it)
{
    GWindow* win = ecs_field(it, GWindow, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (win[i].show_fps)
            DrawFPS(10, 20);

        EndMode2D();
        EndDrawing();
    }
}

