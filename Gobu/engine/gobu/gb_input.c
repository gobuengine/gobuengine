#include "gb_input.h"

bool input_is_mouse_button_down(int button)
{
    return IsMouseButtonDown(button);
}

bool input_is_mouse_button_pressed(int button)
{
    return IsMouseButtonPressed(button);
}

bool input_is_mouse_button_released(int button)
{
    return IsMouseButtonReleased(button);
}

bool input_is_key_down(int key)
{
    return IsKeyDown(key);
}

bool input_is_key_pressed(int key)
{
    return IsKeyPressed(key);
}

bool input_is_key_released(int key)
{
    return IsKeyReleased(key);
}

gb_vec2_t input_mouse_position(void)
{
    return GetMousePosition();
}

gb_vec2_t input_mouse_delta(void)
{
    return GetMouseDelta();
}

float input_mouse_wheel(void)
{
    return GetMouseWheelMove();
}

void input_mouse_position_set(gb_vec2_t position)
{
    SetMousePosition(position.x, position.y);
}

void input_mouse_cursor_set(int cursor)
{
    SetMouseCursor(cursor);
}

gb_vec2_t screen_to_world(gb_camera_t camera, gb_vec2_t position)
{
    Camera2D cam = {
        .offset = (gb_vec2_t){camera.offset.x, camera.offset.y},
        .target = (gb_vec2_t){camera.target.x, camera.target.y},
        .rotation = camera.rotation,
        .zoom = camera.zoom,
    };
    return GetScreenToWorld2D(position, cam);
}

gb_vec2_t world_to_screen(gb_camera_t camera, gb_vec2_t position)
{
    Camera2D cam = {
        .offset = (gb_vec2_t){camera.offset.x, camera.offset.y},
        .target = (gb_vec2_t){camera.target.x, camera.target.y},
        .rotation = camera.rotation,
        .zoom = camera.zoom,
    };
    return GetWorldToScreen2D(position, cam);
}