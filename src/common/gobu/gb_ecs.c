#include "gobu.h"
#include <glib.h>

static void gobucoreImport(ecs_world_t *world);
static gb_core_scene_phases_t gbCoreScenePhases;
static ecs_entity_t projectSettings = 0;

ECS_TAG_DECLARE(gbTagScene);
ECS_TAG_DECLARE(gbOnSceneOpen);
ECS_TAG_DECLARE(gbOnSceneClose);
ECS_TAG_DECLARE(gbOnSceneLoad);
// ECS_TAG_DECLARE(gbOnSceneSave);
ECS_TAG_DECLARE(gbOnSceneReload);
ECS_TAG_DECLARE(gbOnSceneRename);
ECS_TAG_DECLARE(gbOnSceneDelete);
ECS_TAG_DECLARE(gbOnSceneCreate);

ECS_COMPONENT_DECLARE(gb_core_scene_phases_t);
ECS_COMPONENT_DECLARE(gb_core_scene_t);
ECS_COMPONENT_DECLARE(gb_core_scene_physics_t);
ECS_COMPONENT_DECLARE(gb_core_scene_grid_t);

// game project settings
ECS_COMPONENT_DECLARE(gb_core_project_settings1_t);
ECS_COMPONENT_DECLARE(gb_core_project_settings2_t);
ECS_COMPONENT_DECLARE(gb_core_project_settings3_t);

// ECS_COMPONENT_DECLARE(gbSceneActive);
ECS_COMPONENT_DECLARE(gb_origin_t);
ECS_COMPONENT_DECLARE(gb_texture_flip_t);
ECS_COMPONENT_DECLARE(gb_texture_filter_t);
ECS_COMPONENT_DECLARE(gb_scale_mode_t);
ECS_COMPONENT_DECLARE(gb_resolution_mode_t);
ECS_COMPONENT_DECLARE(gb_resource_t);
ECS_COMPONENT_DECLARE(gb_color_t);
ECS_COMPONENT_DECLARE(gb_rect_t);
ECS_COMPONENT_DECLARE(gb_vec2_t);
ECS_COMPONENT_DECLARE(gb_size_t);
ECS_COMPONENT_DECLARE(gb_boundingbox_t);
ECS_COMPONENT_DECLARE(gb_transform_t);
ECS_COMPONENT_DECLARE(gb_image_t);
ECS_COMPONENT_DECLARE(gb_texture_t);
ECS_COMPONENT_DECLARE(gb_font_t);
ECS_COMPONENT_DECLARE(gb_frame_t);
ECS_COMPONENT_DECLARE(gb_comp_text_t);
ECS_COMPONENT_DECLARE(gb_comp_sprite_t);
ECS_COMPONENT_DECLARE(gb_comp_circle_t);
ECS_COMPONENT_DECLARE(gb_comp_rectangle_t);

// -----------------
// NOTE MARK: WORLD
// -----------------

ecs_world_t *gobu_ecs_init(void)
{
    ecs_world_t *ecs = ecs_init();
    ECS_IMPORT(ecs, gobucore);

    // Esto es para crear el project settings siempre
    // esto ayuda a que siempre exista un project settings
    // con valores por defecto y si se agrega un valor nuevo
    // se actualizara en el archivo de proyecto.
    gobu_ecs_project_settings_init(ecs, "New Project");

    return ecs;
}

void gobu_ecs_free(ecs_world_t *ecs)
{
    ecs_fini(ecs);
}

void gobu_ecs_process(ecs_world_t *ecs, float deltaTime)
{
    ecs_progress(ecs, deltaTime);
}

void gobu_ecs_save_to_file(ecs_world_t *world, const char *filename)
{
    gobu_util_write_text_file(filename, ecs_world_to_json(world, NULL));
}

bool gobu_ecs_load_from_file(ecs_world_t *world, const char *filename)
{
    if (gobu_util_exist_file(filename) == false)
        return false;

    return ecs_world_from_json_file(world, filename, NULL) == NULL ? false : true;
}

// -----------------
// NOTE MARK: project settings
// -----------------
void gobu_ecs_project_settings_init(ecs_world_t *ecs, const char *name)
{
    projectSettings = ecs_new_low_id(ecs);
    // ecs_set_name(ecs, projectSettings, gobu_util_string("ProjectSettings"));

    ecs_set(ecs, projectSettings, gb_core_project_settings1_t, {
        .name = gobu_util_string(name),
        .description = gobu_util_string(""),
        .author = gobu_util_string("")
    });

    ecs_set(ecs, projectSettings, gb_core_project_settings2_t, {
        .name = gobu_util_string("com.example.game"),
        .version = gobu_util_string("1.0.0"),
        .publisher = gobu_util_string("[Publisher]")
    });

    ecs_set(ecs, projectSettings, gb_core_project_settings3_t, {
        .resolution = {1280, 720},
        .targetFps = 60,
        .resolutionMode = GB_RESIZE_MODE_NO_CHANGE,
        .scaleMode = GB_SCALE_MODE_LINEAR
    });
}

ecs_entity_t gobu_ecs_project_settings(void)
{
    return projectSettings;
}

// -----------------
// NOTE MARK: Entity
// -----------------
ecs_entity_t gobu_ecs_entity_new_low(ecs_world_t *world, ecs_entity_t parent)
{
    ecs_entity_t entity = ecs_new_low_id(world);
    if (parent > 0)
        gobu_ecs_set_parent(world, entity, parent);

    return entity;
}

ecs_entity_t gobu_ecs_entity_new(ecs_world_t *world, ecs_entity_t parent, const char *name)
{
    ecs_entity_t entity = gobu_ecs_entity_new_low(world, parent);
    g_autofree gchar *name_entity = gobu_util_string_format("%s%ld", name, entity);
    ecs_set_name(world, entity, name_entity);
    ecs_set(world, entity, gb_transform_t, {.position = {0, 0}, .scale = {1, 1}, .rotation = 0, .origin = GB_ORIGIN_CENTER});

    return entity;
}

void gobu_ecs_set_parent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}

ecs_entity_t gobu_ecs_get_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_get_target(world, entity, EcsChildOf, 0);
}

bool gobu_ecs_has_parent(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_has_id(world, entity, ecs_pair(EcsChildOf, EcsWildcard));
}

ecs_entity_t gobu_ecs_clone(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t clone = ecs_clone(world, 0, entity, TRUE);
    ecs_set_name(world, clone, gobu_util_string_format("%s%ld", ecs_get_name(world, entity), clone));
    gobu_ecs_set_parent(world, clone, gobu_ecs_get_parent(world, entity));

    ecs_iter_t it = ecs_children(world, entity);
    while (ecs_children_next(&it))
        for (int i = 0; i < it.count; i++)
            if (ecs_is_alive(world, it.entities[i]))
                gobu_ecs_set_parent(world, gobu_ecs_clone(world, it.entities[i]), clone);

    return clone;
}

bool gobu_ecs_is_enabled(ecs_world_t *world, ecs_entity_t entity)
{
    return !ecs_has_id(world, entity, EcsDisabled);
}

void gobu_ecs_add_component_name(ecs_world_t *world, ecs_entity_t entity, const char *component)
{
    ecs_add_id(world, entity, ecs_lookup(world, component));
}

// -----------------
// NOTE MARK: Entity Events
// -----------------
ecs_entity_t gobu_ecs_observer(ecs_world_t *world, ecs_entity_t event, ecs_iter_action_t callback, void *ctx)
{
    return ecs_observer(world, {
        .query.terms = {{.id = EcsAny}, {EcsDisabled, .oper = EcsOptional}},
        .events = {event},
        .callback = callback,
        .ctx = ctx,
    });
}

void gobu_ecs_emit(ecs_world_t *world, ecs_entity_t event, ecs_entity_t entity)
{
    ecs_emit(world, &(ecs_event_desc_t){.entity = entity, .event = event});
}

// -----------------
// NOTE MARK: Entity SCENE
// -----------------
ecs_entity_t gobu_ecs_scene_new(ecs_world_t *world, const char *name)
{
    g_autofree gchar *new_name = gobu_util_string_format("WorldScene.%s", name);

    // Le agregamos un numero al final si ya existe
    int counter = 1;
    while (ecs_lookup(world, new_name) > 0)
        new_name = gobu_util_string_format("WorldScene.%s%d", name, counter++);

    ecs_entity_t scene = ecs_entity(world, {.name = gobu_util_string(new_name), .add = ecs_ids(gbTagScene)});
    ecs_set(world, scene, gb_core_scene_t, {.color = GOBUWHITE });
    ecs_set(world, scene, gb_core_scene_physics_t, {.enabled = TRUE, .debug = FALSE, .gravity = 980, .gravityDirection = {0, -1}});
    ecs_set(world, scene, gb_core_scene_grid_t, {.size = 32, .enabled = TRUE});
    ecs_enable(world, scene, FALSE);
    gobu_ecs_emit(world, gbOnSceneCreate, scene);

    return scene;
}

ecs_entity_t gobu_ecs_scene_clone(ecs_world_t *world, ecs_entity_t entity)
{
    const char *orig_name = ecs_get_name(world, entity);

    g_autofree gchar *new_name = gobu_util_string(orig_name);
    ecs_entity_t parent = ecs_lookup(world, "WorldScene");

    // Le agregamos un numero al final si ya existe
    int counter = 1;
    while (ecs_lookup_child(world, parent, new_name) > 0)
        new_name = gobu_util_string_format("%s%d", orig_name, counter++);

    ecs_entity_t scene_clone = gobu_ecs_clone(world, entity);
    ecs_set_name(world, scene_clone, new_name);
    ecs_enable(world, scene_clone, FALSE);

    return scene_clone;
}

void gobu_ecs_scene_delete(ecs_world_t *world, ecs_entity_t entity)
{
    gobu_ecs_emit(world, gbOnSceneDelete, entity);
    ecs_delete(world, entity);
}

void gobu_ecs_scene_open(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t scene_active = gobu_ecs_scene_get_open(world);
    if (scene_active > 0)
        ecs_enable(world, scene_active, FALSE);

    ecs_enable(world, entity, TRUE);
    gobu_ecs_emit(world, gbOnSceneOpen, entity);
}

ecs_entity_t gobu_ecs_scene_get_open(ecs_world_t *world)
{
    ecs_query_t *q = ecs_query(world, {.terms = {{gbTagScene}}});
    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_entity_t scene = ecs_iter_first(&it);
    ecs_query_fini(q);
    return scene;
}

void gobu_ecs_scene_reload(ecs_world_t *world)
{
    ecs_entity_t scene_active = gobu_ecs_scene_get_open(world);
    if (scene_active > 0)
    {
        gobu_ecs_scene_open(world, scene_active);
    }
}

int gobu_ecs_scene_count(ecs_world_t *world)
{
    return ecs_count(world, gbTagScene);
}

ecs_entity_t gobu_ecs_scene_get_by_name(ecs_world_t *world, const char *name)
{
    ecs_entity_t parent = ecs_lookup(world, "WorldScene");
    return ecs_lookup_child(world, parent, name);
}

void gobu_ecs_scene_rename(ecs_world_t *world, ecs_entity_t entity, const char *name)
{
    ecs_set_name(world, entity, name);
    gobu_ecs_emit(world, gbOnSceneRename, entity);
}

bool gobu_ecs_scene_has(ecs_world_t *world, ecs_entity_t entity)
{
    return ecs_has(world, entity, gbTagScene);
}

void gobu_ecs_scene_process(ecs_world_t *world, ecs_entity_t root, float delta)
{
    // // process
    // gb_transform_t *transform = ecs_get(world, root, gb_transform_t);
    // if (transform)
    // {
    //     float px = transform->position.x;
    //     float py = transform->position.y;
    //     float sx = transform->scale.x;
    //     float sy = transform->scale.y;
    //     float angle = transform->rotation;
    //     gb_origin_t origin = transform->origin;

    //     gb_comp_rectangle_t *shape = ecs_get(world, root, gb_comp_rectangle_t);
    //     if (shape)
    //         gobu_draw_rect(px, py, shape->width, shape->height, shape->color, shape->lineColor, shape->lineWidth, 0);
    // }
    // // children root
    // ecs_iter_t it = ecs_children(world, root);
    // while (ecs_children_next(&it))
    // {
    //     for (int i = 0; i < it.count; i++)
    //     {
    //         ecs_entity_t entity = it.entities[i];
    //         gobu_ecs_scene_process(world, entity, delta);
    //     }
    // }
}

// -----------------
// NOTE MARK: MODULE BASE
// -----------------
static void gobu_core_scene_pre_update(ecs_iter_t *it)
{
    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t e = it->entities[i];
    }
}

static void gobu_core_scene_render_draw(ecs_iter_t *it)
{
    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t e = it->entities[i];
    }
}

static void gobu_core_scene_render_pre_draw(ecs_iter_t *it)
{
    gb_core_scene_t *scene = ecs_field(it, gb_core_scene_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t e = it->entities[i];

        // gfxb_viewport_begin(scene[i].context);
        // gfxb_viewport_color(scene[i].context, scene[i].clear_color.r, scene[i].clear_color.g, scene[i].clear_color.b);
        // gobu_draw_rect(0, 0, scene[i].size.width, scene[i].size.height, BLANK, scene[i].color, 2.0f, 0);
    }
}

static void gobu_core_scene_render_post_draw(ecs_iter_t *it)
{
    gb_core_scene_t *scene = ecs_field(it, gb_core_scene_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t e = it->entities[i];

        // gfxb_viewport_end(scene[i].context);
    }
}

static void gobucoreImport(ecs_world_t *world)
{
    ECS_MODULE(world, gobucore);

    ECS_TAG_DEFINE(world, gbTagScene);
    ECS_TAG_DEFINE(world, gbOnSceneOpen);
    ECS_TAG_DEFINE(world, gbOnSceneClose);
    ECS_TAG_DEFINE(world, gbOnSceneLoad);
    // ECS_TAG_DEFINE(world, gbOnSceneSave);
    ECS_TAG_DEFINE(world, gbOnSceneReload);
    ECS_TAG_DEFINE(world, gbOnSceneRename);
    ECS_TAG_DEFINE(world, gbOnSceneDelete);
    ECS_TAG_DEFINE(world, gbOnSceneCreate);

    ECS_COMPONENT_DEFINE(world, gb_core_scene_phases_t);
    ECS_COMPONENT_DEFINE(world, gb_core_scene_t);
    ECS_COMPONENT_DEFINE(world, gb_core_scene_physics_t);
    ECS_COMPONENT_DEFINE(world, gb_core_scene_grid_t);
    // game project settings
    ECS_COMPONENT_DEFINE(world, gb_core_project_settings1_t);
    ECS_COMPONENT_DEFINE(world, gb_core_project_settings2_t);
    ECS_COMPONENT_DEFINE(world, gb_core_project_settings3_t);

    // ECS_COMPONENT_DEFINE(world, gbSceneActive);
    ECS_COMPONENT_DEFINE(world, gb_origin_t);
    ECS_COMPONENT_DEFINE(world, gb_texture_flip_t);
    ECS_COMPONENT_DEFINE(world, gb_texture_filter_t);
    ECS_COMPONENT_DEFINE(world, gb_scale_mode_t);
    ECS_COMPONENT_DEFINE(world, gb_resolution_mode_t);
    ECS_COMPONENT_DEFINE(world, gb_resource_t);
    ECS_COMPONENT_DEFINE(world, gb_color_t);
    ECS_COMPONENT_DEFINE(world, gb_rect_t);
    ECS_COMPONENT_DEFINE(world, gb_vec2_t);
    ECS_COMPONENT_DEFINE(world, gb_size_t);
    ECS_COMPONENT_DEFINE(world, gb_boundingbox_t);
    ECS_COMPONENT_DEFINE(world, gb_transform_t);
    ECS_COMPONENT_DEFINE(world, gb_image_t);
    ECS_COMPONENT_DEFINE(world, gb_texture_t);
    ECS_COMPONENT_DEFINE(world, gb_font_t);
    ECS_COMPONENT_DEFINE(world, gb_comp_text_t);
    ECS_COMPONENT_DEFINE(world, gb_comp_sprite_t);
    ECS_COMPONENT_DEFINE(world, gb_frame_t);
    ECS_COMPONENT_DEFINE(world, gb_comp_circle_t);
    ECS_COMPONENT_DEFINE(world, gb_comp_rectangle_t);

// MARK: ENUM
    ecs_enum(world, {
        .entity = ecs_id(gb_origin_t),
        .constants = {
            {.name = "TopLeft", .value = GB_ORIGIN_TOP_LEFT},
            {.name = "TopCenter", .value = GB_ORIGIN_TOP_CENTER},
            {.name = "TopRight", .value = GB_ORIGIN_TOP_RIGHT},
            {.name = "CenterLeft", .value = GB_ORIGIN_CENTER_LEFT},
            {.name = "Center", .value = GB_ORIGIN_CENTER},
            {.name = "CenterRight", .value = GB_ORIGIN_CENTER_RIGHT},
            {.name = "BottomLeft", .value = GB_ORIGIN_BOTTOM_LEFT},
            {.name = "BottomCenter", .value = GB_ORIGIN_BOTTOM_CENTER},
            {.name = "BottomRight", .value = GB_ORIGIN_BOTTOM_RIGHT},
        },
    });

    ecs_enum(world, {
        .entity = ecs_id(gb_texture_flip_t),
        .constants = {
            {.name = "None", .value = GB_NO_FLIP},
            {.name = "Vertical", .value = GB_FLIP_VERTICAL},
            {.name = "Horizontal", .value = GB_FLIP_HORIZONTAL},
            {.name = "Both", .value = GB_FLIP_BOTH},
        },
    });

    ecs_enum(world, {
        .entity = ecs_id(gb_texture_filter_t),
        .constants = {
            {.name = "Point", .value = GB_FILTER_POINT},
            {.name = "Bilinear", .value = GB_FILTER_BILINEAR},
            {.name = "Trilinear", .value = GB_FILTER_TRILINEAR},
        },
    });

    ecs_enum(world, {
        .entity = ecs_id(gb_scale_mode_t),
        .constants = {
            {.name = "scale_nearest", .value = GB_SCALE_MODE_NEAREST},
            {.name = "scale_linear", .value = GB_SCALE_MODE_LINEAR},
        },
    });

    ecs_enum(world, {
        .entity = ecs_id(gb_resolution_mode_t),
        .constants = {
            {.name = "resize_no_change", .value = GB_RESIZE_MODE_NO_CHANGE},
            {.name = "resize_fill_screen", .value = GB_RESIZE_MODE_FILL_SCREEN},
            {.name = "resize_adjust_width", .value = GB_RESIZE_MODE_ADJUST_WIDTH},
            {.name = "resize_adjust_height", .value = GB_RESIZE_MODE_ADJUST_HEIGHT},
        },
    });

// MARK: DATA-COMPONENT
    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_resource_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_color_t),
        .members = {
            {.name = "r", .type = ecs_id(ecs_byte_t)},
            {.name = "g", .type = ecs_id(ecs_byte_t)},
            {.name = "b", .type = ecs_id(ecs_byte_t)},
            {.name = "a", .type = ecs_id(ecs_byte_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_size_t),
        .members = {
            {.name = "width", .type = ecs_id(ecs_f32_t)},
            {.name = "height", .type = ecs_id(ecs_f32_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_vec2_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t)},
            {.name = "y", .type = ecs_id(ecs_f32_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t)},
            {.name = "y", .type = ecs_id(ecs_f32_t)},
            {.name = "width", .type = ecs_id(ecs_f32_t)},
            {.name = "height", .type = ecs_id(ecs_f32_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_transform_t),
        .members = {
            {.name = "position", .type = ecs_id(gb_vec2_t)},
            {.name = "scale", .type = ecs_id(gb_vec2_t)},
            {.name = "rotation", .type = ecs_id(ecs_f32_t)},
            {.name = "origin", .type = ecs_id(gb_origin_t)},
        },
    });

// MARK: COMPONENT DRAW
    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_comp_text_t),
        .members = {
            {.name = "text", .type = ecs_id(ecs_string_t)},
            {.name = "fontSize", .type = ecs_id(ecs_u32_t)},
            {.name = "spacing", .type = ecs_id(ecs_f32_t)},
            {.name = "color", .type = ecs_id(gb_color_t)},
            {.name = "font#font", .type = ecs_id(gb_resource_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_comp_sprite_t),
        .members = {
            {.name = "texture#texture", .type = ecs_id(gb_resource_t)},
            {.name = "filter", .type = ecs_id(gb_texture_filter_t)},
            {.name = "flip", .type = ecs_id(gb_texture_flip_t)},
            {.name = "tint", .type = ecs_id(gb_color_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_frame_t),
        .members = {
            {.name = "hframes", .type = ecs_id(ecs_u32_t)},
            {.name = "vframes", .type = ecs_id(ecs_u32_t)},
            {.name = "frame", .type = ecs_id(ecs_u32_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_comp_circle_t),
        .members = {
            {.name = "radius", .type = ecs_id(ecs_f32_t)},
            {.name = "thickness", .type = ecs_id(ecs_f32_t), .range = {0, 20}},
            {.name = "color", .type = ecs_id(gb_color_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_comp_rectangle_t),
        .members = {
            {.name = "width", .type = ecs_id(ecs_f32_t)},
            {.name = "height", .type = ecs_id(ecs_f32_t)},
            {.name = "roundness", .type = ecs_id(ecs_f32_t), .range = {0.0, 1.0}},
            // {.name = "segments", .type = ecs_id(ecs_f32_t), .range = {0, 60}},
            {.name = "color", .type = ecs_id(gb_color_t)},
            {.name = "lineWidth", .type = ecs_id(ecs_f32_t), .range = {0, 20}},
            {.name = "lineColor", .type = ecs_id(gb_color_t)},
        },
    });

// MARK: CORE SCENE 
    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_core_scene_grid_t),
        .members = {
            {.name = "enabled", .type = ecs_id(ecs_bool_t)},
            {.name = "size", .type = ecs_id(ecs_u32_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_core_scene_t),
        .members = {
            {.name = "color", .type = ecs_id(gb_color_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_core_scene_physics_t),
        .members = {
            {.name = "gravity", .type = ecs_id(ecs_u32_t)},
            {.name = "gravityDirection", .type = ecs_id(gb_vec2_t)},
        },
    });

// MARK: PROJECT SETTINGS
    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_core_project_settings1_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t)},
            {.name = gobu_prop_inspector({.name = "description"}), .type = ecs_id(ecs_string_t)},
            {.name = "author", .type = ecs_id(ecs_string_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_core_project_settings2_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t)},
            {.name = "version", .type = ecs_id(ecs_string_t)},
            {.name = "publisher", .type = ecs_id(ecs_string_t)},
        },
    });

    gobu_ecs_struct(world, {
        .entity = ecs_id(gb_core_project_settings3_t),
        .members = {
            {.name = "resolution", .type = ecs_id(gb_size_t)},
            {.name = "targetFps", .type = ecs_id(ecs_u32_t)},
            {.name = "resolutionMode", .type = ecs_id(gb_resolution_mode_t)},
            {.name = "scaleMode", .type = ecs_id(gb_scale_mode_t)},
        },
    });

// MARK: System and pipelines
    gbCoreScenePhases.PreDraw = ecs_new_w_id(world, EcsPhase);
    gbCoreScenePhases.Background = ecs_new_w_id(world, EcsPhase);
    gbCoreScenePhases.Draw = ecs_new_w_id(world, EcsPhase);
    gbCoreScenePhases.PostDraw = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, gbCoreScenePhases.PreDraw, EcsDependsOn, EcsOnStore);
    ecs_add_pair(world, gbCoreScenePhases.Background, EcsDependsOn, gbCoreScenePhases.PreDraw);
    ecs_add_pair(world, gbCoreScenePhases.Draw, EcsDependsOn, gbCoreScenePhases.Background);
    ecs_add_pair(world, gbCoreScenePhases.PostDraw, EcsDependsOn, gbCoreScenePhases.Draw);

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_dependson(EcsPreUpdate)) }),
        .query.terms = {
            {ecs_id(gb_transform_t)}, 
            {ecs_id(gb_comp_text_t), .oper = EcsOptional},
            {ecs_id(gb_comp_circle_t), .oper = EcsOptional},
            {ecs_id(gb_comp_rectangle_t), .oper = EcsOptional},
            {ecs_id(gb_comp_sprite_t), .oper = EcsOptional},
        },
        .callback = gobu_core_scene_pre_update,
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_dependson(gbCoreScenePhases.Draw)) }),
        .query.terms = {
            {ecs_id(gb_transform_t)}, 
            {ecs_id(gb_comp_text_t), .oper = EcsOptional},
            {ecs_id(gb_comp_circle_t), .oper = EcsOptional},
            {ecs_id(gb_comp_rectangle_t), .oper = EcsOptional},
            {ecs_id(gb_comp_sprite_t), .oper = EcsOptional},
        },
        .callback = gobu_core_scene_render_draw,
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_dependson(gbCoreScenePhases.PreDraw)) }),
        .query.terms = {
            {ecs_id(gb_core_scene_t)}, 
        },
        .callback = gobu_core_scene_render_pre_draw,
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_dependson(gbCoreScenePhases.PostDraw)) }),
        .query.terms = {
            {ecs_id(gb_core_scene_t)}, 
        },
        .callback = gobu_core_scene_render_post_draw,
    });
}
