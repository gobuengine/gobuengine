#include "gobu_ecs_object.h"

struct _GobuEcsObject
{
    GObject parent_instance;
};

G_DEFINE_FINAL_TYPE(GobuEcsObject, gobu_ecs_object, G_TYPE_OBJECT)

static void gobu_ecs_object_class_init(GobuEcsObjectClass *klass)
{
}

static void gobu_ecs_object_init(GobuEcsObject *self)
{
}

GobuEcsObject *gobu_ecs_object_new(void)
{
    return g_object_new(GOBU_TYPE_ECS_OBJECT, NULL);
}
