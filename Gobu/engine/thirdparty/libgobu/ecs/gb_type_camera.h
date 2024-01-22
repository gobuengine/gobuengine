#ifndef GB_TYPE_CAMERA_H
#define GB_TYPE_CAMERA_H

#include <stdio.h>
#include <stdint.h>
#include "thirdparty/flecs/flecs.h"
#include "gb_gfx.h"

#include "gb_type_vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        GB_CAMERA_NONE = 0,
        GB_CAMERA_EDITOR,
        GB_CAMERA_FOLLOWING,
    }eCameraMode;

    typedef struct gb_camera_t
    {
        gb_vec2_t offset;       // Camera offset (displacement from target)
        gb_vec2_t target;       // Camera target (rotation and zoom origin)
        float rotation;         // Camera rotation in degrees
        float zoom;             // Camera zoom (scaling), should be 1.0f by default
        eCameraMode mode;
        // RenderTexture render;   // Camera unique render texture
    }gb_camera_t;

    // global component
    extern ECS_COMPONENT_DECLARE(eCameraMode);
    extern ECS_COMPONENT_DECLARE(gb_camera_t);

    // module import
    void gb_type_camera_moduleImport(ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // GB_TYPE_CAMERA_H
