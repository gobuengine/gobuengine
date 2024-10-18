// gapp_itype_enum.h
#ifndef GAPP_ITYPE_ENUM_H
#define GAPP_ITYPE_ENUM_H

#include "../config.h"

G_BEGIN_DECLS

#define OBJECT_TYPE_IENUM (object_ienum_get_type())
G_DECLARE_FINAL_TYPE(ObjectIEnum, object_ienum, OBJECT, IENUM, GObject)

ObjectIEnum *object_ienum_new(const gchar *name, int value);

gchar *object_ienum_get_name(ObjectIEnum *self);

int object_ienum_get_value(ObjectIEnum *self);

G_END_DECLS

#endif // GAPP_ITYPE_ENUM_H

