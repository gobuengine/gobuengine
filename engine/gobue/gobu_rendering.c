#include "gobu_rendering.h"
#include "gobu_transform.h"
#include "gobu_shapes.h"
#include "gobu_text.h"
#include "gobu_sprite.h"

ECS_COMPONENT_DECLARE(GWindow);
ECS_COMPONENT_DECLARE(RenderPhases);

static void GobuRendering_CheckExitRequest(ecs_iter_t* it);
static void GobuRendering_Window(ecs_iter_t* it);
static void GobuRendering_BeginDrawing(ecs_iter_t* it);
static void GobuRendering_Drawing(ecs_iter_t* it);
static void GobuRendering_EndDrawing(ecs_iter_t* it);

void GobuRenderingImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuRendering);
    // ECS_IMPORT(world, GobuTransform);
    // ECS_IMPORT(world, GobuShapes);
    // ECS_IMPORT(world, GobuText);
    // ECS_IMPORT(world, GobuSprite);

    ECS_COMPONENT_DEFINE(world, RenderPhases);
    ECS_COMPONENT_DEFINE(world, GWindow);

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

    ECS_SYSTEM(world, GobuRendering_BeginDrawing, PreDraw, GWindow);
    // ECS_SYSTEM(world, GobuRendering_Drawing, Draw, GPosition, ? GScale, ? GRotation, ? GShapeRec, ? GSprite, ? GText);
    ECS_SYSTEM(world, GobuRendering_EndDrawing, PostDraw, GWindow);
    ECS_SYSTEM(world, GobuRendering_CheckExitRequest, PostDraw, GWindow);
}

void gobu_rendering_init(ecs_world_t* world, int width, int height, const char* title, bool viewport)
{
    ecs_singleton_set(world, GWindow, { .title = title, .width = width, .height = height, .viewport = viewport });
    // SetTargetFPS(60);
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
    GWindow* win = ecs_field(it, GWindow, 1);

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

    for (int i = 0; i < it->count; i++)
    {
        BeginDrawing();
        ClearBackground(BLACK);
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

    rlPushMatrix();
    {
        for (int i = 0; i < it->count; i++)
        {
            Vector2 origin = (Vector2){ 0.0f, 0.0f };

            rlTranslatef(post[i].x, post[i].y, 0.0f);
            rlRotatef(rota[i], 0.0f, 0.0f, 1.0f);
            rlTranslatef(-origin.x, -origin.y, 0.0f);
            rlScalef(scale[i].x, scale[i].y, 1.0f);
            {
                if (rect) {
                    Rectangle rec_d = (Rectangle){ rect[i].x, rect[i].y, rect[i].width, rect[i].height };
                    DrawRectanglePro(rec_d, origin, 0.0f, rect[i].color);
                }
            }

            // ecs_iter_t child_it = ecs_children(it->world, it->entities[i]);
            // while (ecs_children_next(&it)) {
            //     printf("it.count: %d\n",child_it.count);
            // }
        }
    }
    rlPopMatrix();
}

static void GobuRendering_EndDrawing(ecs_iter_t* it)
{
    for (int i = 0; i < it->count; i++)
    {
        // DrawFPS(10, 20);
        EndDrawing();
    }
}

