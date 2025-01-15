// gapp_itype_enum.h
#ifndef TYPE_OUTLINER_POPOVER_ITEM_H
#define TYPE_OUTLINER_POPOVER_ITEM_H

#include "../config.h"

G_BEGIN_DECLS

#define TOUTLINER_TYPE_POPOVER_ITEM (toutliner_popover_item_get_type())
G_DECLARE_FINAL_TYPE(TOutlinerPopoverItem, toutliner_popover_item, TOUTLINER, POPOVER_ITEM, GObject)

GListStore *toutliner_popover_new(void);

TOutlinerPopoverItem *toutliner_popover_item_new(const gchar *icon, const gchar *name, const gchar *tooltip, const gchar *component);

void toutliner_popover_item_set_name(TOutlinerPopoverItem *self, const gchar *name);

const gchar *toutliner_popover_item_get_name(TOutlinerPopoverItem *self);

const gchar *toutliner_popover_item_get_icon_name(TOutlinerPopoverItem *self);

const gchar *toutliner_popover_item_get_tooltip(TOutlinerPopoverItem *self);

const gchar *toutliner_popover_item_get_component(TOutlinerPopoverItem *self);

G_END_DECLS

#endif // TYPE_OUTLINER_POPOVER_ITEM_H

