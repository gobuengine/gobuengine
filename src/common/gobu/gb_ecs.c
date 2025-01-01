#include "gobu.h"
#include <glib.h>

static void gobucoreImport(ecs_world_t *world);

ECS_TAG_DECLARE(gbTagScene);
ECS_TAG_DECLARE(gbOnSceneOpen);
ECS_TAG_DECLARE(gbOnSceneClose);
ECS_TAG_DECLARE(gbOnSceneLoad);
// ECS_TAG_DECLARE(gbOnSceneSave);
ECS_TAG_DECLARE(gbOnSceneReload);
ECS_TAG_DECLARE(gbOnSceneRename);
ECS_TAG_DECLARE(gbOnSceneDelete);
ECS_TAG_DECLARE(gbOnSceneCreate);

// ECS_COMPONENT_DECLARE(gbSceneActive);
ECS_COMPONENT_DECLARE(gb_origin_t);
ECS_COMPONENT_DECLARE(gb_texture_flip_t);
ECS_COMPONENT_DECLARE(gb_texture_filter_t);
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
ECS_COMPONENT_DECLARE(gb_text_t);
ECS_COMPONENT_DECLARE(gb_sprite_t);
ECS_COMPONENT_DECLARE(gb_sprite_frame_t);
ECS_COMPONENT_DECLARE(gb_shape_circle_t);
ECS_COMPONENT_DECLARE(gb_shape_rec_t);

ecs_world_t *gobu_ecs_init(void)
{
    ecs_world_t *ecs = ecs_init();
    ECS_IMPORT(ecs, gobucore);
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
    return ecs_world_from_json_file(world, filename, NULL) == NULL ? false : true;
}

// enity
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

// entity events
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

// entity scene

ecs_entity_t gobu_scene_new(ecs_world_t *world, const char *name)
{
    g_autofree gchar *new_name = gobu_util_string_format("WorldScene.%s", name);

    // Le agregamos un numero al final si ya existe
    int counter = 1;
    while (ecs_lookup(world, new_name) > 0)
        new_name = gobu_util_string_format("WorldScene.%s%d", name, counter++);

    ecs_entity_t scene = ecs_entity(world, {.name = gobu_util_string(new_name), .add = ecs_ids(gbTagScene)});
    ecs_enable(world, scene, FALSE);
    gobu_ecs_emit(world, gbOnSceneCreate, scene);

    return scene;
}

ecs_entity_t gobu_scene_clone(ecs_world_t *world, ecs_entity_t entity)
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

void gobu_scene_delete(ecs_world_t *world, ecs_entity_t entity)
{
    gobu_ecs_emit(world, gbOnSceneDelete, entity);
    ecs_delete(world, entity);
}

void gobu_scene_open(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_entity_t scene_active = gobu_scene_get_open(world);
    if (scene_active > 0)
        ecs_enable(world, scene_active, FALSE);

    ecs_enable(world, entity, TRUE);
    gobu_ecs_emit(world, gbOnSceneOpen, entity);
}

ecs_entity_t gobu_scene_get_open(ecs_world_t *world)
{
    ecs_query_t *q = ecs_query(world, {.terms = {{gbTagScene}}});
    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_entity_t scene = ecs_iter_first(&it);
    ecs_query_fini(q);
    return scene;
}

void gobu_scene_reload(ecs_world_t *world)
{
    ecs_entity_t scene_active = gobu_scene_get_open(world);
    if (scene_active > 0)
    {
        gobu_scene_open(world, scene_active);
    }
}

int gobu_scene_count(ecs_world_t *world)
{
    return ecs_count(world, gbTagScene);
}

ecs_entity_t gobu_scene_get_by_name(ecs_world_t *world, const char *name)
{
    ecs_entity_t parent = ecs_lookup(world, "WorldScene");
    return ecs_lookup_child(world, parent, name);
}

void gobu_scene_rename(ecs_world_t *world, ecs_entity_t entity, const char *name)
{
    ecs_set_name(world, entity, name);
    gobu_ecs_emit(world, gbOnSceneRename, entity);
}

// ECS MODULE BASE
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

    // ECS_COMPONENT_DEFINE(world, gbSceneActive);
    ECS_COMPONENT_DEFINE(world, gb_origin_t);
    ECS_COMPONENT_DEFINE(world, gb_texture_flip_t);
    ECS_COMPONENT_DEFINE(world, gb_texture_filter_t);
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
    ECS_COMPONENT_DEFINE(world, gb_text_t);
    ECS_COMPONENT_DEFINE(world, gb_sprite_t);
    ECS_COMPONENT_DEFINE(world, gb_sprite_frame_t);
    ECS_COMPONENT_DEFINE(world, gb_shape_circle_t);
    ECS_COMPONENT_DEFINE(world, gb_shape_rec_t);

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

    ecs_struct(world, {
        .entity = ecs_id(gb_resource_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_color_t),
        .members = {
            {.name = "r", .type = ecs_id(ecs_byte_t)},
            {.name = "g", .type = ecs_id(ecs_byte_t)},
            {.name = "b", .type = ecs_id(ecs_byte_t)},
            {.name = "a", .type = ecs_id(ecs_byte_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_size_t),
        .members = {
            {.name = "width", .type = ecs_id(ecs_f32_t)},
            {.name = "height", .type = ecs_id(ecs_f32_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_vec2_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t)},
            {.name = "y", .type = ecs_id(ecs_f32_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t)},
            {.name = "y", .type = ecs_id(ecs_f32_t)},
            {.name = "width", .type = ecs_id(ecs_f32_t)},
            {.name = "height", .type = ecs_id(ecs_f32_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_transform_t),
        .members = {
            {.name = "position", .type = ecs_id(gb_vec2_t)},
            {.name = "scale", .type = ecs_id(gb_vec2_t)},
            {.name = "rotation", .type = ecs_id(ecs_f32_t)},
            {.name = "origin", .type = ecs_id(gb_origin_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_text_t),
        .members = {
            {.name = "text", .type = ecs_id(ecs_string_t)},
            {.name = "fontSize", .type = ecs_id(ecs_u32_t)},
            {.name = "spacing", .type = ecs_id(ecs_f32_t)},
            {.name = "color", .type = ecs_id(gb_color_t)},
            {.name = "font#font", .type = ecs_id(gb_resource_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_sprite_t),
        .members = {
            {.name = "texture#texture", .type = ecs_id(gb_resource_t)},
            {.name = "filter", .type = ecs_id(gb_texture_filter_t)},
            {.name = "flip", .type = ecs_id(gb_texture_flip_t)},
            {.name = "tint", .type = ecs_id(gb_color_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_sprite_frame_t),
        .members = {
            {.name = "hframes", .type = ecs_id(ecs_u32_t)},
            {.name = "vframes", .type = ecs_id(ecs_u32_t)},
            {.name = "frame", .type = ecs_id(ecs_u32_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_circle_t),
        .members = {
            {.name = "radius", .type = ecs_id(ecs_f32_t)},
            {.name = "thickness", .type = ecs_id(ecs_f32_t), .range = {0, 20}},
            {.name = "color", .type = ecs_id(gb_color_t)},
        },
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_rec_t),
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
}
