#include "actor.h"
#include "glib.h"

// typedef struct ActorWorld {
// } ActorWorld;

// typedef struct Scene {
//     GPtrArray *actors;
// } Scene;

// typedef struct Actor {
//     gint id;
//     gchar *name;
//     Actor *parent;
//     GPtrArray *children;
// } Actor;

// actor_resource_load_texture("sprite", "sprite.png");

// Stage *stage = actor_stage_new();

// Scene *scene = actor_scene_new();
// actor_stage_add_scene(stage, scene);
// {
//     Actor *bodyPhysics = actor_new_physics();
//     // actor_behaviour_set(bodyPhysics, "body_physics");
//     actor_scene_add(scene, bodyPhysics);
//     {
//         Actor *shapeCollider = actor_new_circle_collider();
//         actor_add_child(bodyPhysics, shapeCollider);

//         Actor *sprite = actor_new_sprite();
//         actor_sprite_set_texture(sprite, "sprite");
//         actor_add_child(bodyPhysics, sprite);
//     }
// }

// // -- -- -- -- --
// // serialize
// const char *json = actor_serialize(actor);

// // deserialize
// Actor *actor = actor_deserialize(json);
// actor_scene_add(scene, actor);
// actor_deserialize(actor, json);

// // instanciar actor file json al mundo
// // actor_world_add_actor(world, "actor.json");
