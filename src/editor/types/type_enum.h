// type_enum.h
#ifndef TYPE_ENUM_H
#define TYPE_ENUM_H

#include "../config.h"
#include <stdint.h>

G_BEGIN_DECLS

#define OBJECT_TYPE_IENUM (object_ienum_get_type())
G_DECLARE_FINAL_TYPE(ObjectIEnum, object_ienum, OBJECT, IENUM, GObject)

ObjectIEnum *object_ienum_new(const gchar *name, int8_t value, int8_t index);

gchar *object_ienum_get_name(ObjectIEnum *self);

int8_t object_ienum_get_value(ObjectIEnum *self);

int8_t object_ienum_get_index(ObjectIEnum *self);

G_END_DECLS

#endif // TYPE_ENUM_H
