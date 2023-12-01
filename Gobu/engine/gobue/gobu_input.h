#ifndef __GOBU_INPUT_H__
#define __GOBU_INPUT_H__
#include <stdio.h>
#include <stdint.h>
#include "gobu_camera.h"
#include <flecs/flecs.h>
#include "raygo/raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct GInputSystem
    {
        Vector2(*get_screen_to_world)(Vector2 position, GCamera camera);
        struct
        {
            bool (*button_down)(int button);
            bool (*button_pressed)(int button);
            bool (*button_released)(int button);
            bool (*button_up)(int button);
            Vector2(*get_position)(void);
            Vector2(*get_delta)(void);
            float (*get_wheel)(void);
            void (*set_cursor)(int cursor);
            void (*set_position)(int x, int y);
            void (*set_offset)(int x, int y);
            void (*set_scale)(float scaleX, float scaleY);
            Vector2(*get_screen_to_world)(GCamera camera);
        }mouse;

        struct
        {
            bool (*down)(int key);
            bool (*pressed)(int key);
            bool (*released)(int key);
            bool (*up)(int key);
        }keyboard;

    }GInputSystem;

    extern ECS_COMPONENT_DECLARE(GInputSystem);

    void GobuInputSystemImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
