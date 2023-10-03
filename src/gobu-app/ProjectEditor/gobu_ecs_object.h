#ifndef GOBU_ECS_OBJECT_H
#define GOBU_ECS_OBJECT_H
#include <gtk/gtk.h>
#include "gobu/gobu-gobu.h"

G_BEGIN_DECLS

typedef struct _GobuEcsObjectPrivate GobuEcsObjectPrivate;

#define GOBU_TYPE_ECS_OBJECT (gobu_ecs_object_get_type())
G_DECLARE_FINAL_TYPE(GobuEcsObject, gobu_ecs_object, GOBU, ECS_OBJECT, GObject)

struct _GobuEcsObjectPrivate
{
    GtkWidget parent_instance;
};

GobuEcsObject *gobu_ecs_object_new(void);

G_END_DECLS

#endif // GOBU_ECS_OBJECT_H
