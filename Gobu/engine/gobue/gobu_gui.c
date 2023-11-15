#include "gobu_gui.h"
#include "gobu_transform.h"
#include "gobu_rendering.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

// #define RAYGUI_IMPLEMENTATION
// #include "raygo/raygui.h"

ECS_COMPONENT_DECLARE(GUIWindow);

static void GobuGui_Set(ecs_iter_t* it);
static void GobuGui_Draw(ecs_iter_t* it);
static void GobuGui_Update(ecs_iter_t* it);

void GobuGuiImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuGui);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GUIWindow);

    const RenderPhases* phases = ecs_singleton_get(world, RenderPhases);

    ECS_OBSERVER(world, GobuGui_Set, EcsOnSet, GUIWindow);

    ECS_SYSTEM(world, GobuGui_Draw, phases->PostDraw, GUIWindow);
    ECS_SYSTEM(world, GobuGui_Update, EcsOnUpdate, GUIWindow);
}

static void GobuGui_Set(ecs_iter_t* it)
{
    GUIWindow* window = ecs_field(it, GUIWindow, 1);

    for (int i = 0; i < it->count; i++)
    {
        window[i].position = GetWindowPosition();
    }
}

static void GobuGui_Update(ecs_iter_t* it)
{
    static Vector2 mouse_start;

    GUIWindow* window = ecs_field(it, GUIWindow, 1);

    for (int i = 0; i < it->count; i++)
    {
        Vector2 mouse = GetMousePosition();
        Vector2 mouse_delta = GetMouseDelta();
        int width = GetScreenWidth();
        int height = GetScreenHeight();

        Rectangle bounds = (Rectangle){ 0.0f, 0.0f, width, height };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && window[i].movable == false)
        {
            if (CheckCollisionPointRec(mouse, (Rectangle) { 0.0f, 0.0f, width, 20 })) {
                window[i].movable = true;
                mouse_start = mouse;
            }
        }

        if (window[i].movable)
        {
            window[i].position.x += (mouse.x - mouse_start.x) * 60.0f * it->delta_time;
            window[i].position.y += (mouse.y - mouse_start.y) * 60.0f * it->delta_time;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) window[i].movable = false;
        }
    }
}

static void GobuGui_Draw(ecs_iter_t* it)
{
    GUIWindow* window = ecs_field(it, GUIWindow, 1);

    for (int i = 0; i < it->count; i++)
    {
        window[i].handle(it->world, &window[i], it->delta_time);
    }
}

