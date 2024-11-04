#include "type_outliner_item.h"

struct _TOutlinerItem
{
    GObject parent_instance;

    ecs_entity_t entity;
    gchar *name;
    GListStore *children; // TOutlinerItem
    GListStore *root;     // TOutlinerItem
    GtkWidget *expander;  // GtkExpander
};

G_DEFINE_TYPE(TOutlinerItem, toutliner_item, G_TYPE_OBJECT)

static void toutliner_item_finalize(GObject *object)
{
    TOutlinerItem *self = TOUTLINER_ITEM(object);
}

static void toutliner_item_class_init(TOutlinerItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

static void toutliner_item_init(TOutlinerItem *self)
{
}

static void notify_signal_toutliner_item(GListStore *store, GParamSpec *pspec, gpointer user_data)
{
    g_return_if_fail(G_IS_LIST_STORE(store));
    g_return_if_fail(TOUTLINER_IS_ITEM(user_data));

    TOutlinerItem *self = TOUTLINER_ITEM(user_data);

    // No hacemos nada para el elemento raíz
    if (g_strcmp0(self->name, GAPP_ROOT_STR) == 0)
        return;

    // Mostrar u ocultar el expansor basado en si el elemento tiene hijos
    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(store)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(self->expander), !has_children);

    // Opcionalmente, actualizar cualquier otra propiedad visual del ítem
    // Por ejemplo, podrías cambiar un icono o un estilo
    // gtk_widget_set_css_classes(GTK_WIDGET(self->row), has_children ? "parent-item" : "leaf-item");
}

TOutlinerItem *toutliner_item_new(ecs_world_t *world, ecs_entity_t entity)
{
    TOutlinerItem *self = g_object_new(TOUTLINER_TYPE_ITEM, NULL);

    self->entity = entity;
    self->name = g_strdup(pixio_get_name(world, entity));
    self->children = g_list_store_new(TOUTLINER_TYPE_ITEM);
    self->root = NULL;
    self->expander = NULL;

    g_signal_connect(self->children, "notify", G_CALLBACK(notify_signal_toutliner_item), self);

    return self;
}

void toutliner_item_set_entity(TOutlinerItem *self, ecs_entity_t entity)
{
    self->entity = entity;
}

ecs_entity_t toutliner_item_get_entity(TOutlinerItem *self)
{
    return self->entity;
}

void toutliner_item_set_name(TOutlinerItem *self, const gchar *name)
{
    g_free(self->name);
    self->name = g_strdup(name);
}

gchar *toutliner_item_get_name(TOutlinerItem *self)
{
    return self->name;
}

void toutliner_item_set_expander(TOutlinerItem *self, GtkWidget *expander)
{
    self->expander = expander;
}

GtkWidget *toutliner_item_get_expander(TOutlinerItem *self)
{
    return self->expander;
}

void toutliner_item_set_children(TOutlinerItem *self, GListStore *children)
{
    self->children = children;
}

GListStore *toutliner_item_get_children(TOutlinerItem *self)
{
    return self->children;
}

void toutliner_item_set_root(TOutlinerItem *self, GListStore *root)
{
    self->root = root;
}

GListStore *toutliner_item_get_root(TOutlinerItem *self)
{
    return self->root;
}
