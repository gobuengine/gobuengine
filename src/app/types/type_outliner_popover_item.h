// gapp_itype_enum.h
#ifndef GAPP_OUTLINER_ITEM_H
#define GAPP_OUTLINER_ITEM_H

#include "../config.h"
#include "pixio/pixio.h"

G_BEGIN_DECLS

#define TOUTLINER_TYPE_POPOVER_ITEM (toutliner_popover_item_get_type())
G_DECLARE_FINAL_TYPE(TOutlinerPopoverItem, toutliner_popover_item, OUTLINER, POPOVER_ITEM, GObject)

TOutlinerPopoverItem *toutliner_popover_item_new(const gchar *icon, const gchar *name, const gchar *tooltip, const gchar *component);

void toutliner_popover_item_set_name(TOutlinerPopoverItem *self, const gchar *name);

const gchar *toutliner_popover_item_get_name(TOutlinerPopoverItem *self);

GtkWidget *toutliner_popover_item_get_icon(TOutlinerPopoverItem *self);

GtkWidget *toutliner_popover_item_get_tooltip(TOutlinerPopoverItem *self);

const gchar *toutliner_popover_item_get_component(TOutlinerPopoverItem *self);

G_END_DECLS

#endif // GAPP_OUTLINER_ITEM_H

