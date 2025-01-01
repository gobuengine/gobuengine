// gapp_prefab.h
#ifndef GAPP_PREFAB_H
#define GAPP_PREFAB_H

#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_PREFAB (gapp_prefab_get_type())
G_DECLARE_FINAL_TYPE(GappPrefab, gapp_prefab, GAPP, PREFAB, GtkBox)

GappPrefab *gapp_prefab_new(void);

G_END_DECLS

#endif // GAPP_PREFAB_H

