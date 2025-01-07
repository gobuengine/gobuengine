#ifndef GOBU_ECS_H
#define GOBU_ECS_H
#include <stdio.h>
#include "gb_type.h"

// MARK: ECS
extern ECS_TAG_DECLARE(gbTagScene);
extern ECS_TAG_DECLARE(gbOnSceneOpen);
extern ECS_TAG_DECLARE(gbOnSceneClose);
extern ECS_TAG_DECLARE(gbOnSceneLoad);
// extern ECS_TAG_DECLARE(gbOnSceneSave);
extern ECS_TAG_DECLARE(gbOnSceneReload);
extern ECS_TAG_DECLARE(gbOnSceneRename);
extern ECS_TAG_DECLARE(gbOnSceneDelete);
extern ECS_TAG_DECLARE(gbOnSceneCreate);
// property inspector
extern ECS_COMPONENT_DECLARE(gb_property_t);
// pipeline
extern ECS_COMPONENT_DECLARE(gb_core_scene_phases_t);
extern ECS_COMPONENT_DECLARE(gb_core_scene_t);
extern ECS_COMPONENT_DECLARE(gb_core_scene_physics_t);
extern ECS_COMPONENT_DECLARE(gb_core_scene_grid_t);
// game project settings
extern ECS_COMPONENT_DECLARE(gb_core_project_settings1_t);
extern ECS_COMPONENT_DECLARE(gb_core_project_settings2_t);
extern ECS_COMPONENT_DECLARE(gb_core_project_settings3_t);

// extern ECS_COMPONENT_DECLARE(gbSceneActive);
extern ECS_COMPONENT_DECLARE(gb_origin_t);
extern ECS_COMPONENT_DECLARE(gb_texture_flip_t);
extern ECS_COMPONENT_DECLARE(gb_texture_filter_t);
extern ECS_COMPONENT_DECLARE(gb_scale_mode_t);
extern ECS_COMPONENT_DECLARE(gb_resolution_mode_t);
extern ECS_COMPONENT_DECLARE(gb_resource_t);
extern ECS_COMPONENT_DECLARE(gb_color_t);
extern ECS_COMPONENT_DECLARE(gb_rect_t);
extern ECS_COMPONENT_DECLARE(gb_vec2_t);
extern ECS_COMPONENT_DECLARE(gb_size_t);
extern ECS_COMPONENT_DECLARE(gb_boundingbox_t);
extern ECS_COMPONENT_DECLARE(gb_transform_t);
extern ECS_COMPONENT_DECLARE(gb_image_t);
extern ECS_COMPONENT_DECLARE(gb_texture_t);
extern ECS_COMPONENT_DECLARE(gb_font_t);
extern ECS_COMPONENT_DECLARE(gb_frame_t);

extern ECS_COMPONENT_DECLARE(gb_comp_text_t);
extern ECS_COMPONENT_DECLARE(gb_comp_sprite_t);
extern ECS_COMPONENT_DECLARE(gb_comp_circle_t);
extern ECS_COMPONENT_DECLARE(gb_comp_rectangle_t);

void gobu_ecs_init(void);
ecs_world_t *gobu_ecs_world(void);
void gobu_ecs_free(void);
void gobu_ecs_process(float deltaTime);
void gobu_ecs_save_to_file(const char *filename);
bool gobu_ecs_load_from_file(const char *filename);

void gobu_ecs_project_settings_init(const char *name);
ecs_entity_t gobu_ecs_project_settings(void);

ecs_entity_t gobu_ecs_entity_new_low(ecs_entity_t parent);
ecs_entity_t gobu_ecs_entity_new(ecs_entity_t parent, const char *name);
void gobu_ecs_set_parent(ecs_entity_t entity, ecs_entity_t parent);
ecs_entity_t gobu_ecs_get_parent(ecs_entity_t entity);
bool gobu_ecs_has_parent(ecs_entity_t entity);
ecs_entity_t gobu_ecs_clone(ecs_entity_t entity);
bool gobu_ecs_is_enabled(ecs_entity_t entity);
void gobu_ecs_add_component_name(ecs_entity_t entity, const char *component);
const char *gobu_ecs_name(ecs_entity_t entity);
void gobu_ecs_set_name(ecs_entity_t entity, const char *name);
void gobu_ecs_enable(ecs_entity_t entity, bool enable);

ecs_entity_t gobu_ecs_observer(ecs_entity_t event, ecs_iter_action_t callback, void *ctx);
void gobu_ecs_emit(ecs_entity_t event, ecs_entity_t entity);

ecs_entity_t gobu_ecs_scene_new(const char *name);
ecs_entity_t gobu_ecs_scene_clone(ecs_entity_t entity);
void gobu_ecs_scene_delete(ecs_entity_t entity);
void gobu_ecs_scene_open(ecs_entity_t entity);
ecs_entity_t gobu_ecs_scene_get_open(void);
void gobu_ecs_scene_reload(void);
int gobu_ecs_scene_count(void);
ecs_entity_t gobu_ecs_scene_get_by_name(const char *name);
void gobu_ecs_scene_rename(ecs_entity_t entity, const char *name);
bool gobu_ecs_scene_has(ecs_entity_t entity);
void gobu_ecs_scene_process(ecs_entity_t root, float delta);

#endif // GOBU_ECS_H
