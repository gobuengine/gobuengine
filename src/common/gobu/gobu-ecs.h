#ifndef GOBU_ECS_H
#define GOBU_ECS_H
#include "gobu-type.h"

// MARK: ECS
go_extern ECS_TAG_DECLARE(gbTagScene);
go_extern ECS_TAG_DECLARE(gbOnSceneOpen);
go_extern ECS_TAG_DECLARE(gbOnSceneClose);
go_extern ECS_TAG_DECLARE(gbOnSceneLoad);
// go_extern ECS_TAG_DECLARE(gbOnSceneSave);
go_extern ECS_TAG_DECLARE(gbOnSceneReload);
go_extern ECS_TAG_DECLARE(gbOnSceneRename);
go_extern ECS_TAG_DECLARE(gbOnSceneDelete);
go_extern ECS_TAG_DECLARE(gbOnSceneCreate);
// property inspector
go_extern ECS_COMPONENT_DECLARE(go_property_t);
// pipeline
go_extern ECS_COMPONENT_DECLARE(go_core_scene_phases_t);
go_extern ECS_COMPONENT_DECLARE(go_core_scene_t);
go_extern ECS_COMPONENT_DECLARE(go_core_scene_physics_t);
go_extern ECS_COMPONENT_DECLARE(go_core_scene_grid_t);
// game project settings
go_extern ECS_COMPONENT_DECLARE(go_core_project_settings1_t);
go_extern ECS_COMPONENT_DECLARE(go_core_project_settings2_t);
go_extern ECS_COMPONENT_DECLARE(go_core_project_settings3_t);

// go_extern ECS_COMPONENT_DECLARE(gbSceneActive);
go_extern ECS_COMPONENT_DECLARE(go_origin_t);
go_extern ECS_COMPONENT_DECLARE(go_texture_flip_t);
go_extern ECS_COMPONENT_DECLARE(go_texture_filter_t);
go_extern ECS_COMPONENT_DECLARE(go_scale_mode_t);
go_extern ECS_COMPONENT_DECLARE(go_resolution_mode_t);
go_extern ECS_COMPONENT_DECLARE(go_resource_t);
go_extern ECS_COMPONENT_DECLARE(go_color_t);
go_extern ECS_COMPONENT_DECLARE(go_rect_t);
go_extern ECS_COMPONENT_DECLARE(go_vec2_t);
go_extern ECS_COMPONENT_DECLARE(go_size_t);
go_extern ECS_COMPONENT_DECLARE(go_boundingbox_t);
go_extern ECS_COMPONENT_DECLARE(go_transform_t);
go_extern ECS_COMPONENT_DECLARE(go_image_t);
go_extern ECS_COMPONENT_DECLARE(go_texture_t);
go_extern ECS_COMPONENT_DECLARE(go_font_t);
go_extern ECS_COMPONENT_DECLARE(go_frame_t);

go_extern ECS_COMPONENT_DECLARE(go_comp_text_t);
go_extern ECS_COMPONENT_DECLARE(go_comp_sprite_t);
go_extern ECS_COMPONENT_DECLARE(go_comp_circle_t);
go_extern ECS_COMPONENT_DECLARE(go_comp_rectangle_t);

go_public void go_ecs_init(void);
go_public ecs_world_t *go_ecs_world(void);
go_public void go_ecs_free(void);
go_public void go_ecs_process(float deltaTime);
go_public void go_ecs_save_to_file(const char *filename);
bool go_ecs_load_from_file(const char *filename);

go_public void go_ecs_project_settings_init(const char *name);
go_public ecs_entity_t go_ecs_project_settings(void);

go_public ecs_entity_t go_ecs_entity_new_low(ecs_entity_t parent);
go_public ecs_entity_t go_ecs_entity_new(ecs_entity_t parent, const char *name);
go_public void go_ecs_set_parent(ecs_entity_t entity, ecs_entity_t parent);
go_public ecs_entity_t go_ecs_get_parent(ecs_entity_t entity);
go_public bool go_ecs_has_parent(ecs_entity_t entity);
go_public ecs_entity_t go_ecs_clone(ecs_entity_t entity);
go_public bool go_ecs_is_enabled(ecs_entity_t entity);
go_public void go_ecs_add_component_name(ecs_entity_t entity, const char *component);
go_public const char *go_ecs_name(ecs_entity_t entity);
go_public void go_ecs_set_name(ecs_entity_t entity, const char *name);
go_public void go_ecs_enable(ecs_entity_t entity, bool enable);

go_public ecs_entity_t go_ecs_observer(ecs_entity_t event, ecs_iter_action_t callback, void *ctx);
go_public void go_ecs_emit(ecs_entity_t event, ecs_entity_t entity);

go_public ecs_entity_t go_ecs_scene_new(const char *name);
go_public ecs_entity_t go_ecs_scene_clone(ecs_entity_t entity);
go_public void go_ecs_scene_delete(ecs_entity_t entity);
go_public void go_ecs_scene_open(ecs_entity_t entity);
go_public ecs_entity_t go_ecs_scene_get_open(void);
go_public void go_ecs_scene_reload(void);
go_public int go_ecs_scene_count(void);
go_public ecs_entity_t go_ecs_scene_get_by_name(const char *name);
go_public void go_ecs_scene_rename(ecs_entity_t entity, const char *name);
go_public bool go_ecs_scene_has(ecs_entity_t entity);
go_public void go_ecs_scene_process(ecs_entity_t root, float delta);

#endif // GOBU_ECS_H
