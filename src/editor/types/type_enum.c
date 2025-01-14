
#include "type_enum.h"

typedef struct _ObjectIEnum
{
    GObject parent;
    char *name;
    int8_t value;
    int8_t index;
} ObjectIEnum;

G_DEFINE_TYPE(ObjectIEnum, object_ienum, G_TYPE_OBJECT)

static void object_ienum_finalize(GObject *object)
{
    ObjectIEnum *self = OBJECT_IENUM(object);
    g_free(self->name);
}

static void object_ienum_class_init(ObjectIEnumClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = object_ienum_finalize;
}

static void object_ienum_init(ObjectIEnum *self)
{
}

ObjectIEnum *object_ienum_new(const gchar *name, int8_t value, int8_t index)
{
    ObjectIEnum *self = g_object_new(OBJECT_TYPE_IENUM, NULL);
    self->name = g_strdup(name);
    self->value = value;
    self->index = index;
    return self;
}

gchar *object_ienum_get_name(ObjectIEnum *self)
{
    return self->name;
}

int8_t object_ienum_get_value(ObjectIEnum *self)
{
    return self->value;
}

int8_t object_ienum_get_index(ObjectIEnum *self)
{
    return self->index;
}
