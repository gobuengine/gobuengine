#include "type_outliner_popover_item.h"

enum
{
    PROP_0,
    PROP_NAME,
    N_PROPS
};

static GParamSpec *properties[N_PROPS];

typedef struct _TOutlinerPopoverItem
{
    GObject parent;
    gchar *name;
    gchar *icon;
    gchar *tooltip;
    gchar *component;
} TOutlinerPopoverItem;

G_DEFINE_TYPE(TOutlinerPopoverItem, toutliner_popover_item, G_TYPE_OBJECT)

static void toutliner_popover_item_finalize(GObject *object)
{
    TOutlinerPopoverItem *self = TOUTLINER_POPOVER_ITEM(object);

    g_free(self->name);
    g_free(self->icon);
    g_free(self->tooltip);
    g_free(self->component);
}

static void toutliner_popover_item_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    TOutlinerPopoverItem *self = TOUTLINER_POPOVER_ITEM(object);

    switch (property_id)
    {
    case PROP_NAME:
        g_free(self->name);
        self->name = g_value_dup_string(value);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void toutliner_popover_item_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    TOutlinerPopoverItem *self = TOUTLINER_POPOVER_ITEM(object);

    switch (property_id)
    {
    case PROP_NAME:
        g_value_set_string(value, self->name);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void toutliner_popover_item_class_init(TOutlinerPopoverItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = toutliner_popover_item_finalize;
    object_class->set_property = toutliner_popover_item_set_property;
    object_class->get_property = toutliner_popover_item_get_property;

    // Registrar las propiedades del objeto
    properties[PROP_NAME] = g_param_spec_string("name", "Name", "The name of the item", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void toutliner_popover_item_init(TOutlinerPopoverItem *self)
{
}

GListStore *toutliner_popover_new(void)
{
    return g_list_store_new(TOUTLINER_TYPE_POPOVER_ITEM);
}

TOutlinerPopoverItem *toutliner_popover_item_new(const gchar *icon, const gchar *name, const gchar *tooltip, const gchar *component)
{
    TOutlinerPopoverItem *self = g_object_new(TOUTLINER_TYPE_POPOVER_ITEM, NULL);

    self->name = g_strdup(name);
    self->icon = g_strdup(icon);
    self->tooltip = g_strdup(tooltip);
    self->component = g_strdup(component);

    return self;
}

void toutliner_popover_item_set_name(TOutlinerPopoverItem *self, const gchar *name)
{
    g_return_if_fail(TOUTLINER_IS_POPOVER_ITEM(self));
    g_object_set(self, "name", name, NULL);
}

const gchar *toutliner_popover_item_get_name(TOutlinerPopoverItem *self)
{
    g_return_val_if_fail(TOUTLINER_IS_POPOVER_ITEM(self), NULL);
    return self->name;
}

const gchar *toutliner_popover_item_get_icon_name(TOutlinerPopoverItem *self)
{
    g_return_val_if_fail(TOUTLINER_IS_POPOVER_ITEM(self), NULL);
    return self->icon;
}

const gchar *toutliner_popover_item_get_tooltip(TOutlinerPopoverItem *self)
{
    g_return_val_if_fail(TOUTLINER_IS_POPOVER_ITEM(self), NULL);
    return self->tooltip;
}

const gchar *toutliner_popover_item_get_component(TOutlinerPopoverItem *self)
{
    g_return_val_if_fail(TOUTLINER_IS_POPOVER_ITEM(self), NULL);
    return self->component;
}
