#ifndef GB_INPUT_H
#define GB_INPUT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "binn/binn_json.h"
#include "gfx/gb_gfx.h"

#include "gobu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // global component

    // API::Resource
    bool input_is_mouse_button_down(int button);

    bool input_is_mouse_button_pressed(int button);

    bool input_is_mouse_button_released(int button);

    bool input_is_key_down(int key);

    bool input_is_key_pressed(int key);

    bool input_is_key_released(int key);

    gb_vec2_t input_mouse_position(void);

    gb_vec2_t input_mouse_delta(void);

    float input_mouse_wheel(void);

    void input_mouse_position_set(gb_vec2_t position);

    void input_mouse_cursor_set(int cursor);

    gb_vec2_t screen_to_world(gb_camera_t camera, gb_vec2_t position);

    gb_vec2_t world_to_screen(gb_camera_t camera, gb_vec2_t position);

    // module import

#ifdef __cplusplus
}
#endif

#endif // GB_INPUT_H
