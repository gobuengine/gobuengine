#include "gapp_outliner.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_level_editor.h"
#include "gapp_inspector.h"

// #include "pixio/pixio.h"
#include "pixio/pixio_components.h"

// MARK: OutlinerPopoverItem
// - - - - - - - - - -
// BEGIN DEFINE TYPE
// - - - - - - - - - -

enum
{
    PROP_0,
    PROP_NAME,
    N_PROPS
};

static GParamSpec *properties[N_PROPS];

typedef struct _OutlinerPopoverItem
{
    GObject parent;
    gchar *name;
    gchar *icon;
    gchar *tooltip; // Nueva propiedad para el tooltip
    gchar *component;
} OutlinerPopoverItem;

#define OUTLINER_TYPE_POPOVER_ITEM (outliner_popover_item_get_type())
G_DECLARE_FINAL_TYPE(OutlinerPopoverItem, outliner_popover_item, OUTLINER, POPOVER_ITEM, GObject)
G_DEFINE_TYPE(OutlinerPopoverItem, outliner_popover_item, G_TYPE_OBJECT)

static void outliner_popover_item_finalize(GObject *object)
{
    OutlinerPopoverItem *self = OUTLINER_POPOVER_ITEM(object);

    g_free(self->name);
    g_free(self->icon);
    g_free(self->tooltip); // Liberar memoria del tooltip
    g_free(self->component);
}

static void outliner_popover_item_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    OutlinerPopoverItem *self = OUTLINER_POPOVER_ITEM(object);

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

static void outliner_popover_item_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    OutlinerPopoverItem *self = OUTLINER_POPOVER_ITEM(object);

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

static void outliner_popover_item_class_init(OutlinerPopoverItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = outliner_popover_item_finalize;
    object_class->set_property = outliner_popover_item_set_property;
    object_class->get_property = outliner_popover_item_get_property;

    // Registrar las propiedades del objeto
    properties[PROP_NAME] = g_param_spec_string("name", "Name", "The name of the item", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void outliner_popover_item_init(OutlinerPopoverItem *self)
{
}

static OutlinerPopoverItem *outliner_popover_item_new(const gchar *icon, const gchar *name, const gchar *tooltip, const gchar *component)
{
    OutlinerPopoverItem *self = g_object_new(OUTLINER_TYPE_POPOVER_ITEM, NULL);

    self->name = g_strdup(name);
    self->icon = g_strdup(icon);
    self->tooltip = g_strdup(tooltip);
    self->component = g_strdup(component);

    return self;
}

// - - - - - - - - - -
// END DEFINE TYPE
// - - - - - - - - - -

// MARK: OutlinerItem
// - - - - - - - - - -
// BEGIN DEFINE TYPE
// - - - - - - - - - -

typedef struct _OutlinerItem
{
    GObject parent;

    ecs_entity_t entity;
    gchar *name;
    GListStore *children; // OutlinerItem
    GListStore *root;     // OutlinerItem
    GtkWidget *expander;  // GtkExpander
} OutlinerItem;

#define OUTLINER_TYPE_ITEM (outliner_item_get_type())
G_DECLARE_FINAL_TYPE(OutlinerItem, outliner_item, OUTLINER, ITEM, GObject)
G_DEFINE_TYPE(OutlinerItem, outliner_item, G_TYPE_OBJECT)

static void outliner_item_finalize(GObject *object)
{
    OutlinerItem *self = OUTLINER_ITEM(object);
}

static void outliner_item_class_init(OutlinerItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

static void outliner_item_init(OutlinerItem *self)
{
}

/**
 * @brief Actualiza la visualización de un ítem del esquema basado en sus hijos.
 *
 * Esta función se llama cuando cambia el número de hijos de un ítem del esquema.
 * Actualiza la visibilidad del expansor del árbol basándose en si el elemento
 * tiene hijos o no. No hace nada para el elemento raíz.
 *
 * @param store El GListStore que contiene los hijos del ítem.
 * @param pspec El GParamSpec del parámetro que ha cambiado (no utilizado).
 * @param user_data Puntero al OutlinerItem que se está actualizando.
 */
static void outliner_item_children_update(GListStore *store, GParamSpec *pspec, gpointer user_data)
{
    g_return_if_fail(G_IS_LIST_STORE(store));
    g_return_if_fail(OUTLINER_IS_ITEM(user_data));

    OutlinerItem *self = OUTLINER_ITEM(user_data);

    // No hacemos nada para el elemento raíz
    if (g_strcmp0(self->name, "Root") == 0)
        return;

    // Mostrar u ocultar el expansor basado en si el elemento tiene hijos
    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(store)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(self->expander), !has_children);

    // Opcionalmente, actualizar cualquier otra propiedad visual del ítem
    // Por ejemplo, podrías cambiar un icono o un estilo
    // gtk_widget_set_css_classes(GTK_WIDGET(self->row), has_children ? "parent-item" : "leaf-item");
}

static OutlinerItem *outliner_item_new(ecs_world_t *world, ecs_entity_t entity)
{
    OutlinerItem *self = g_object_new(OUTLINER_TYPE_ITEM, NULL);
    self->entity = entity;
    self->children = g_list_store_new(OUTLINER_TYPE_ITEM);
    self->root = NULL;
    self->name = g_strdup(ecs_get_name(world, entity));

    g_signal_connect(self->children, "notify", G_CALLBACK(outliner_item_children_update), self);

    return self;
}

// - - - - - - - - - -
// END DEFINE TYPE
// - - - - - - - - - -

// MARK: ClassGappOutliner
struct _GappOutliner
{
    GtkWidget parent;

    GtkWidget *colview;
    GtkTreeListModel *tree_model;
    GtkSingleSelection *selection;

    GListStore *store;
    ecs_world_t *world;
    ecs_entity_t root;
};

static void gapp_outliner_ui_setup(GappOutliner *self);

G_DEFINE_TYPE(GappOutliner, gapp_outliner, GTK_TYPE_BOX)

static void gapp_outliner_class_init(GappOutlinerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

static void gapp_outliner_init(GappOutliner *self)
{
    gapp_outliner_ui_setup(self);
}

// MARK: --- FUNCIONES PRIVADAS ---

/**
 * @brief Crea un modelo de lista para los hijos de una entidad en el esquema.
 *
 * Esta función toma un ítem del esquema y devuelve un modelo de lista
 * que contiene sus hijos. Si el ítem no tiene hijos, la función devuelve NULL.
 *
 * @param item Un puntero GObject al ítem del esquema.
 * @param outliner Un puntero a la estructura GappOutliner (no utilizado en esta función).
 * @return GListModel* Un nuevo modelo de lista con los hijos del ítem, o NULL si no tiene hijos.
 */
static GListModel *gapp_outliner_fn_create_list_model_entity_children(GObject *item, GappOutliner *outliner)
{
    OutlinerItem *oitem = OUTLINER_ITEM(item);

    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(oitem->children)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(oitem->expander), !has_children);

    return G_LIST_MODEL(g_object_ref(oitem->children));
}

/**
 * Busca un OutlinerItem por su entidad en un modelo de selección.
 *
 * @param selection Puntero al GtkSingleSelection que contiene los items.
 * @param entity La entidad ECS que se está buscando.
 * @return OutlinerItem* El item encontrado, o NULL si no se encuentra.
 */
static OutlinerItem *gapp_outliner_fn_find_item_by_entity(GtkSingleSelection *selection, ecs_entity_t entity)
{
    GListModel *model = gtk_multi_selection_get_model(selection);
    guint n = g_list_model_get_n_items(model);
    for (guint i = 0; i < n; i++)
    {
        GtkTreeListRow *row = g_list_model_get_item(model, i);
        OutlinerItem *item = gtk_tree_list_row_get_item(row);
        g_object_unref(row);
        if (item->entity == entity)
        {
            return item;
        }
    }

    return NULL;
}

static OutlinerItem *gapp_outliner_fn_get_selected_item(GtkSingleSelection *selection)
{
    GListModel *model = gtk_multi_selection_get_model(selection);
    guint n = g_list_model_get_n_items(model);
    for (guint i = 0; i < n; i++)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            return gtk_tree_list_row_get_item(row);
        }
    }

    return NULL;
}

/**
 * Función de callback para los hooks de ECS para manejar eventos de adición y eliminación de entidades.
 *
 * @param it Puntero al iterador ECS que contiene la información del evento.
 */
static void fn_hooks_callback(ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;
    GappOutliner *outliner = it->ctx;

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t e = it->entities[i];

        if (event == EcsOnAdd)
        {
            OutlinerItem *item = outliner_item_new(world, e);
            item->root = outliner->store;

            // Verificamos si la entidad tiene padre
            if (ecs_has_id(world, e, ecs_pair(EcsChildOf, EcsWildcard)))
            {
                // Obtenemos el padre
                ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
                OutlinerItem *item_find = gapp_outliner_fn_find_item_by_entity(outliner->selection, parent);
                if (item_find != NULL)
                {
                    item->root = item_find->children;
                    g_list_store_append(item_find->children, item);
                    continue;
                }
            }

            // La entidad no tiene padre, se agrega al almacén raíz
            g_list_store_append(outliner->store, item);
        }
        else if (event == EcsOnRemove)
        {
            guint position;
            OutlinerItem *item = gapp_outliner_fn_find_item_by_entity(outliner->selection, e);
            if (item && g_list_store_find(item->root, item, &position))
                g_list_store_remove(item->root, position);
        }
    }
}

// MARK: --- SIGNAL ---
static void gapp_outliner_s_toolbar_add_clicked(GtkWidget *widget, GtkWidget *popover)
{
    gtk_popover_popup(GTK_POPOVER(popover));
}

/**
 * @brief Maneja el evento de clic en el botón de eliminación de la barra de herramientas del esquema.
 *
 * Esta función elimina los elementos seleccionados del esquema, excluyendo el elemento raíz.
 * Itera sobre los elementos seleccionados en orden inverso para evitar problemas de índice
 * durante la eliminación.
 *
 * @param widget El widget que activó el evento (no utilizado en esta función).
 * @param outliner Puntero a la estructura GappOutliner que contiene el estado del esquema.
 */
static void gapp_outliner_s_toolbar_remove_clicked(GtkWidget *widget, GappOutliner *outliner)
{
    GListModel *model = gtk_multi_selection_get_model(outliner->selection);
    guint n = g_list_model_get_n_items(model);

    for (gint i = n - 1; i >= 0; i--)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(outliner->selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            OutlinerItem *item = gtk_tree_list_row_get_item(row);

            // Evitar la eliminación del elemento raíz
            if (g_strcmp0(item->name, "Root") == 0)
            {
                continue;
            }

            // Eliminar la entidad del mundo
            ecs_delete(outliner->world, item->entity);

            g_object_unref(row);
            g_free(item);
        }
    }
}

static void gapp_outliner_s_list_view_activated(GtkMultiSelection *selection, guint position, guint n_items, GappOutliner *outliner)
{
    GListModel *model = gtk_multi_selection_get_model(selection);
    guint n = g_list_model_get_n_items(model);
    for (guint i = 0; i < n; i++)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            OutlinerItem *item = gtk_tree_list_row_get_item(row);
            gapp_inspector_set_entity(gobu_level_editor_get_inspector(gtk_widget_get_parent(gtk_widget_get_parent(outliner))), outliner->world, item->entity);
        }
    }
}

static void column_factory_s_setup_name(GtkSignalListItemFactory *factory, GtkListItem *listitem, GappOutliner *outliner)
{
    // GtkDropTarget *drop;
    // GtkDragSource *drag;

    GtkWidget *expander = gtk_tree_expander_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), expander);
    // gtk_tree_expander_set_indent_for_depth(GTK_TREE_EXPANDER(expander), FALSE);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), TRUE);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gapp_widget_set_margin(box, 6);
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), box);

    // drop = gtk_drop_target_new(OBJECT_TYPE_OUTLINER_ITEM, GDK_ACTION_MOVE);
    // g_signal_connect(drop, "drop", G_CALLBACK(signal_drop), listitem);
    // gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop));

    // drag = gtk_drag_source_new();
    // gtk_drag_source_set_actions(drag, GDK_ACTION_MOVE);
    // g_signal_connect(drag, "prepare", G_CALLBACK(signal_drag_prepare), listitem);
    // gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drag));

    GtkWidget *icon = gtk_image_new();
    gtk_box_append(GTK_BOX(box), icon);

    GtkWidget *label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    // gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(label, 5);
    gtk_widget_set_margin_end(label, 5);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(box), label);
}

static void column_factory_s_bind_name(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappOutliner *outliner)
{
    GtkTreeListRow *row_item = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_TREE_LIST_ROW(row_item));

    OutlinerItem *item = gtk_tree_list_row_get_item(row_item);

    item->expander = gtk_list_item_get_child(list_item);
    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(item->expander), row_item);

    // Root expander is always expanded
    if (strcmp(item->name, "Root") == 0)
    {
        gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(item->expander), TRUE);
        gtk_tree_list_row_set_expanded(row_item, TRUE);
    }

    GtkWidget *box = gtk_tree_expander_get_child(item->expander);

    GtkWidget *icon = gtk_widget_get_first_child(box);
    gtk_image_set_from_icon_name(GTK_IMAGE(icon), "edit-select-all-symbolic");

    GtkWidget *label = gtk_widget_get_last_child(box);
    gtk_label_set_label(GTK_LABEL(label), item->name);

    g_object_unref(item);
}

static void s_setup_outliner_popover_entity_item_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gapp_widget_set_margin(hbox_parent, 8);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), hbox_parent);
    gtk_box_append(GTK_BOX(hbox_parent), gtk_image_new());
    gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(""));
}

static void s_bind_outliner_popover_entity_item_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    OutlinerPopoverItem *popoverItem = gtk_list_item_get_item(listitem);

    GtkWidget *hbox_parent = gtk_list_item_get_child(listitem);
    GtkWidget *icon = gtk_widget_get_first_child(hbox_parent);
    gtk_image_set_from_icon_name(GTK_IMAGE(icon), popoverItem->icon);

    GtkWidget *label = gtk_widget_get_last_child(hbox_parent);
    gtk_label_set_label(GTK_LABEL(label), popoverItem->name);
}

static void s_outliner_popover_entity_item_activated(GtkListView *self, guint position, GtkWidget *popover)
{
    GtkSelectionModel *selection = gtk_list_view_get_model(self);
    OutlinerPopoverItem *popoverItem = OUTLINER_POPOVER_ITEM(g_object_ref(g_list_model_get_item(G_LIST_MODEL(selection), position)));

    GappOutliner *outliner = g_object_get_data(G_OBJECT(popover), "outliner");

    {
        // create a new entity and add it to the world
        OutlinerItem *item_selected = gapp_outliner_fn_get_selected_item(outliner->selection);
        ecs_entity_t parent = item_selected != NULL ? item_selected->entity : ecs_lookup(outliner->world, "Root");
        // create entity
        ecs_entity_t entity = pixio_entity_new(outliner->world, parent, popoverItem->name);
        // add components
        if (strcmp(popoverItem->component, "entity.empty") != 0)
            ecs_add_id(outliner->world, entity, ecs_lookup(outliner->world, popoverItem->component));
    }

    gtk_popover_popdown(GTK_POPOVER(popover));
}

// MARK: --- UI ---

/**
 * Crea y devuelve un nuevo GListStore con elementos para un menú desplegable del outliner.
 *
 * Esta función inicializa un GListStore con varios OutlinerPopoverItem, cada uno
 * representando una entidad o elemento de interfaz de usuario diferente que se puede
 * agregar al outliner. Los elementos incluyen varias entidades gráficas, de audio,
 * y de interfaz de usuario.
 *
 * @return GListStore* Un puntero al nuevo GListStore creado y poblado con elementos.
 */
static GListStore *gapp_outliner_ui_popover_new(void)
{
    GListStore *store = g_list_store_new(OUTLINER_TYPE_POPOVER_ITEM);

    const struct
    {
        const char *icon;
        const char *label;
        const char *type;
    } items[] = {
        {"list-add-symbolic", "Entity", "entity.empty"},
        {"emblem-photos-symbolic", "Sprite", "pixio_type_module.pixio_sprite_t"},
        {"emblem-photos-symbolic", "Tiling Sprite", "entity.empty"},
        {"emblem-photos-symbolic", "Tilemap", "entity.empty"},
        {"preferences-desktop-wallpaper-symbolic", "Animated Sprite", "entity.empty"},
        {"microphone-sensitivity-high-symbolic", "Audio Listener", "entity.empty"},
        {"audio-volume-medium-symbolic", "Sound", "entity.empty"},
        {"camera-photo-symbolic", "Camera", "entity.empty"},
        {"list-add-symbolic", "Light", "entity.empty"},
        {"input-dialpad-symbolic", "Particle System", "entity.empty"},
        {"face-monkey-symbolic", "Panel", "entity.empty"},
        {"face-monkey", "Text", "pixio_type_module.pixio_text_t"},
        {"face-monkey", "Shape Rectangle", "pixio_type_module.pixio_shape_rec_t"},
    };

    for (size_t i = 0; i < G_N_ELEMENTS(items); i++)
    {
        g_list_store_append(store, outliner_popover_item_new(items[i].icon, items[i].label, "", items[i].type));
    }

    return store;
}

static GtkWidget *gapp_outliner_ui_popover_menu_toolbar_add(GtkWidget *parent, GappOutliner *self)
{
    GtkWidget *box, *item, *icon, *search, *scroll;
    GtkFilter *filter;
    GtkFilterListModel *filter_model;

    GtkWidget *popover = gtk_popover_new();
    gtk_widget_set_parent(popover, parent);
    g_object_set_data(G_OBJECT(popover), "outliner", self);
    gtk_widget_add_css_class(popover, "popover");

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_set_child(GTK_POPOVER(popover), box);

    search = gtk_search_entry_new();
    gapp_widget_set_margin(GTK_SEARCH_ENTRY(search), 4);
    gtk_box_append(GTK_BOX(box), search);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    // gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scroll), TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(scroll, 220, 420);
    gtk_box_append(GTK_BOX(box), scroll);
    {
        GListStore *store = gapp_outliner_ui_popover_new();

        filter = GTK_FILTER(gtk_string_filter_new(gtk_property_expression_new(OUTLINER_TYPE_POPOVER_ITEM, NULL, "name")));
        filter_model = gtk_filter_list_model_new(G_LIST_MODEL(store), filter);
        gtk_filter_list_model_set_incremental(filter_model, TRUE);
        g_object_bind_property(search, "text", filter, "search", 0);

        GtkSingleSelection *selection = gtk_no_selection_new(G_LIST_MODEL(filter_model));

        GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(s_setup_outliner_popover_entity_item_factory), NULL);
        g_signal_connect(factory, "bind", G_CALLBACK(s_bind_outliner_popover_entity_item_factory), NULL);

        GtkWidget *list_view = gtk_list_view_new(selection, factory);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);
        // gtk_list_view_set_show_separators(GTK_LIST_VIEW(list_view), TRUE);
        gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(list_view), TRUE);
        g_signal_connect(list_view, "activate", G_CALLBACK(s_outliner_popover_entity_item_activated), popover);
    }

    return popover;
}

static void gapp_outliner_ui_setup(GappOutliner *self)
{
    GtkWidget *item;
    GtkListItemFactory *factory;
    // GtkColumnViewColumn *column;

    { // toolbar
        GtkWidget *toolbar = gapp_widget_toolbar_new();
        gtk_box_append(self, toolbar);

        item = gapp_widget_button_new_icon_with_label("list-add-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        GtkWidget *menu_popover = gapp_outliner_ui_popover_menu_toolbar_add(item, self);
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_outliner_s_toolbar_add_clicked), menu_popover);

        item = gapp_widget_button_new_icon_with_label("edit-copy-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);

        item = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_outliner_s_toolbar_remove_clicked), self);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    { //
        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(self, scroll);
        {
            self->store = g_list_store_new(OUTLINER_TYPE_ITEM);

            GtkTreeListModel *tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->store),
                                                                   FALSE,
                                                                   FALSE,
                                                                   gapp_outliner_fn_create_list_model_entity_children,
                                                                   self,
                                                                   NULL);

            self->selection = gtk_multi_selection_new(G_LIST_MODEL(tree_model));

            factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(column_factory_s_setup_name), self);
            g_signal_connect(factory, "bind", G_CALLBACK(column_factory_s_bind_name), self);

            GtkWidget *list_view = gtk_list_view_new(self->selection, factory);
            // gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(list_view), TRUE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);

            g_signal_connect(self->selection, "selection-changed", G_CALLBACK(gapp_outliner_s_list_view_activated), self);
        }
    }
}

// MARK: --- API ---

/**
 * Crea y inicializa una nueva instancia de GappOutliner.
 *
 * @return Puntero a la nueva instancia de GappOutliner.
 */
GappOutliner *gapp_outliner_new(void)
{
    return g_object_new(GAPP_TYPE_OUTLINER,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        NULL);
}

/**
 * Inicia el proceso del outliner asociándolo con un mundo ECS y configurando los hooks.
 *
 * @param outliner Puntero al GappOutliner que se va a iniciar.
 * @param world Puntero al mundo ECS que se asociará con el outliner.
 */
void gapp_outliner_start_process(GappOutliner *outliner, ecs_world_t *world)
{
    // Verifica que tanto el outliner como el mundo no sean NULL
    g_return_if_fail(outliner != NULL && world != NULL);

    // Asocia el mundo ECS con el outliner
    outliner->world = world;

    // Configura los hooks para el componente pixio_transform_t
    ecs_set_hooks(world, pixio_transform_t, {.on_add = fn_hooks_callback, .on_remove = fn_hooks_callback, .ctx = outliner});

    g_debug("Outliner process started and hooks set for pixio_transform_t");
}

// --- END API ---
