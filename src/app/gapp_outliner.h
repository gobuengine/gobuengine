// gobu_outliner.h
#ifndef GAPP_OUTLINER_H
#define GAPP_OUTLINER_H

#include "config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define GAPP_TYPE_OUTLINER (gapp_outliner_get_type())
G_DECLARE_FINAL_TYPE(GappOutliner, gapp_outliner, GAPP, OUTLINER, GtkBox)

GappOutliner *gapp_outliner_new(void);

void gapp_outliner_start_process(GappOutliner *outliner, ecs_world_t *world);

G_END_DECLS

#endif // GAPP_OUTLINER_H
