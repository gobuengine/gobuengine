#include "bugo_camera_manager.h"

void bugo_ecs_init_camera_manager(ecs_world_t* world)
{

}

void bugo_ecs_camera_manager_set_main(Camera2D* camera)
{
    ecs_entity_t camera_t = bugo_ecs_get_camera2d_id();
    ecs_set_id(bugo_ecs_world(), camera_t, camera_t, sizeof(Camera2D), camera);
}
