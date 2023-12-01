#include "gobu_input.h"

ECS_COMPONENT_DECLARE(GInputSystem);

static void GobuInputSystem_Add(ecs_iter_t* it);

void GobuInputSystemImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuInputSystem);

    ECS_COMPONENT_DEFINE(world, GInputSystem);

    ECS_OBSERVER(world, GobuInputSystem_Add, EcsOnSet, GInputSystem);
}

static Vector2 get_screen_to_world(Vector2 position, GCamera camera)
{
    return GetScreenToWorld2D(position, camera.camera);
}

static Vector2 get_screen_to_world_mouse(GCamera camera)
{
    return GetScreenToWorld2D(GetMousePosition(), camera.camera);
}

static void GobuInputSystem_Add(ecs_iter_t* it)
{
    GInputSystem* input = ecs_field(it, GInputSystem, 1);

    for (int i = 0; i < it->count; i++)
    {
        input[i].mouse.button_down = IsMouseButtonDown;
        input[i].mouse.button_pressed = IsMouseButtonPressed;
        input[i].mouse.button_released = IsMouseButtonReleased;
        input[i].mouse.button_up = IsMouseButtonUp;
        input[i].mouse.get_position = GetMousePosition;
        input[i].mouse.get_delta = GetMouseDelta;
        input[i].mouse.get_wheel = GetMouseWheelMove;
        input[i].mouse.set_cursor = SetMouseCursor;
        input[i].mouse.set_position = SetMousePosition;
        input[i].mouse.set_offset = SetMouseOffset;
        input[i].mouse.set_scale = SetMouseScale;
        input[i].mouse.get_screen_to_world = get_screen_to_world_mouse;

        input[i].keyboard.down = IsKeyDown;
        input[i].keyboard.pressed = IsKeyPressed;
        input[i].keyboard.released = IsKeyReleased;
        input[i].keyboard.up = IsKeyUp;

        input[i].get_screen_to_world = get_screen_to_world;
    }
}

