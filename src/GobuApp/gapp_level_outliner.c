#include "gapp_level_outliner.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_level_editor.h"
#include "gapp_level_inspector.h"

#include "types/type_outliner_item.h"
#include "types/type_outliner_popover_item.h"

#include "pixio/pixio.h"

// MARK: ClassGappOutliner
struct _GappOutliner
{
    GtkWidget parent;

    GtkTreeListModel *tree_model;
    GtkSingleSelection *selection;
    GtkWidget *list_view;

    GListStore *store;
    ecs_world_t *world;
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
    TOutlinerItem *oitem = OUTLINER_ITEM(item);

    GListStore *children = toutliner_item_get_children(oitem);
    GtkWidget *expander = toutliner_item_get_expander(oitem);

    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(children)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), !has_children);

    return G_LIST_MODEL(g_object_ref(children));
}

/**
 * Busca un TOutlinerItem por su entidad en un modelo de selección.
 *
 * @param selection Puntero al GtkSingleSelection que contiene los items.
 * @param entity La entidad ECS que se está buscando.
 * @return TOutlinerItem* El item encontrado, o NULL si no se encuentra.
 */
TOutlinerItem *gapp_outliner_fn_find_item_by_entity(GtkSingleSelection *selection, ecs_entity_t entity)
{
    GListModel *model = gtk_multi_selection_get_model(selection);
    guint n = g_list_model_get_n_items(model);
    for (guint i = 0; i < n; i++)
    {
        GtkTreeListRow *row = g_list_model_get_item(model, i);
        TOutlinerItem *item = gtk_tree_list_row_get_item(row);
        g_object_unref(row);
        if (toutliner_item_get_entity(item) == entity)
        {
            return item;
        }
    }

    return NULL;
}

static TOutlinerItem *gapp_outliner_fn_get_selected_item(GtkSingleSelection *selection)
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
 * Enfoca y selecciona una entidad específica en el outliner.
 *
 * Esta función busca la entidad especificada en el modelo del outliner,
 * la selecciona si la encuentra, y devuelve un valor booleano indicando
 * si la operación tuvo éxito.
 *
 * @param outliner El puntero al GappOutliner.
 * @param entity La entidad a enfocar y seleccionar.
 * @return gboolean TRUE si la entidad fue encontrada y seleccionada, FALSE en caso contrario.
 */
static gboolean gapp_outliner_fn_focus_entity(GappOutliner *outliner, ecs_entity_t entity)
{
    g_return_val_if_fail(outliner != NULL && entity != 0, FALSE);

    GtkSelectionModel *selection_model = GTK_SELECTION_MODEL(outliner->selection);
    GListModel *model = gtk_multi_selection_get_model(outliner->selection);
    guint n_items = g_list_model_get_n_items(model);

    for (guint i = 0; i < n_items; i++)
    {
        GtkTreeListRow *row = g_list_model_get_item(model, i);
        TOutlinerItem *item = gtk_tree_list_row_get_item(row);

        if (toutliner_item_get_entity(item) == entity)
        {
            gtk_selection_model_select_item(selection_model, i, TRUE);
            g_object_unref(row);
            return TRUE;
        }
        g_object_unref(row);
    }

    return FALSE;
}

/**
 * Maneja la selección de una entidad en el outliner.
 *
 * Esta función se encarga de actualizar la selección visual en el outliner
 * y actualizar el inspector con la entidad seleccionada.
 *
 * @param outliner El puntero al GappOutliner.
 * @param entity La entidad seleccionada.
 * @param selected Indica si la entidad está seleccionada (TRUE) o deseleccionada (FALSE).
 */
static void gapp_outliner_fn_selected_entity(GappOutliner *outliner, ecs_entity_t entity, gboolean selected)
{
    g_return_if_fail(outliner != NULL && entity != 0);

    if (selected)
    {
        gapp_outliner_fn_focus_entity(outliner, entity);
    }

    GtkWidget *level_editor = gtk_widget_get_ancestor(GTK_WIDGET(outliner), GOBU_TYPE_LEVEL_EDITOR);
    GtkWidget *inspector = gobu_level_editor_get_inspector(level_editor);
    gapp_inspector_set_entity(inspector, outliner->world, entity);
}

// MARK: --- SIGNAL ---

/**
 * gapp_outliner_s_toolbar_add_clicked:
 * @widget: (transfer none): El widget que activó la señal de clic.
 * @popover: (transfer none): El #GtkPopover que se mostrará.
 *
 * Maneja el evento de clic en el botón "Agregar" de la barra de herramientas del esquema.
 *
 * Esta función se activa cuando el usuario hace clic en el botón "Agregar" en la
 * barra de herramientas del esquema. Su única acción es mostrar un popover que
 * presumiblemente contiene opciones para agregar nuevos elementos al esquema.
 *
 * Nota: Esta función asume que @popover es un widget válido de tipo GtkPopover.
 *
 * Since: 1.0
 */
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
    gboolean isRemove = FALSE;

    GListModel *model = gtk_multi_selection_get_model(outliner->selection);
    guint n = g_list_model_get_n_items(model);

    for (gint i = n - 1; i >= 0; i--)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(outliner->selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *item = gtk_tree_list_row_get_item(row);

            if (g_strcmp0(toutliner_item_get_name(item), GAPP_ROOT_STR) != 0)
            {
                pixio_delete(outliner->world, toutliner_item_get_entity(item));
                isRemove = TRUE;
            }

            g_object_unref(row);
            g_object_unref(item);
        }
    }

    if (isRemove)
        gapp_outliner_fn_selected_entity(outliner, ecs_lookup(outliner->world, GAPP_ROOT_STR), TRUE);
}

/**
 * Maneja el evento de clic en el botón "Duplicar" de la barra de herramientas del Outliner.
 * Esta función duplica los elementos seleccionados en el outliner, excluyendo el elemento raíz.
 *
 * @param widget El widget que generó el evento (no utilizado en esta función).
 * @param outliner Puntero a la estructura GappOutliner que contiene el estado del Outliner.
 */
static void gapp_outliner_s_toolbar_duplicate_clicked(GtkWidget *widget, GappOutliner *outliner)
{
    GListModel *model = gtk_multi_selection_get_model(outliner->selection);
    guint n = g_list_model_get_n_items(model);

    for (gint i = n - 1; i >= 0; i--)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(outliner->selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *item = gtk_tree_list_row_get_item(row);

            if (g_strcmp0(toutliner_item_get_name(item), GAPP_ROOT_STR) != 0)
            {
                ecs_entity_t eclone = pixio_clone(outliner->world, toutliner_item_get_entity(item));
                gapp_outliner_fn_selected_entity(outliner, eclone, TRUE);
            }

            g_object_unref(row);
            g_object_unref(item);
        }
    }
}

/**
 * Maneja la activación de elementos en la vista de lista del outliner.
 *
 * Esta función se llama cuando se activan elementos en la vista de lista.
 * Recorre los elementos seleccionados y llama a la función de selección
 * de entidad para cada uno de ellos.
 *
 * @param selection El modelo de selección múltiple.
 * @param position La posición del primer elemento activado.
 * @param n_items El número de elementos activados.
 * @param outliner El puntero al GappOutliner.
 */
static void gapp_outliner_s_list_view_activated(GtkMultiSelection *selection, guint position, guint n_items, GappOutliner *outliner)
{
    g_return_if_fail(selection != NULL && outliner != NULL);

    GListModel *model = gtk_multi_selection_get_model(selection);
    GtkSelectionModel *selection_model = GTK_SELECTION_MODEL(selection);
    guint total_items = g_list_model_get_n_items(model);

    for (guint i = 0; i < total_items; i++)
    {
        if (gtk_selection_model_is_selected(selection_model, i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *item = gtk_tree_list_row_get_item(row);

            gapp_outliner_fn_selected_entity(outliner, toutliner_item_get_entity(item), TRUE);

            g_object_unref(row);
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

/**
 * column_factory_s_bind_name:
 * @factory: El #GtkSignalListItemFactory que llamó a esta función
 * @list_item: El #GtkListItem que representa el elemento en la columna
 * @outliner: El #GappOutliner al que pertenece el elemento
 *
 * Función auxiliar llamada por el #GtkSignalListItemFactory para configurar
 * la apariencia de cada elemento en la columna "Nombre" del Outliner.
 *
 * Esta función obtiene el #TOutlinerItem asociado al listitem, configura
 * el widget expander, establece el icono y la etiqueta con la información
 * del elemento, y maneja el caso especial del elemento raíz.
 *
 * Since: 1.0
 */
static void column_factory_s_bind_name(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappOutliner *outliner)
{
    GtkTreeListRow *row_item = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_TREE_LIST_ROW(row_item));

    TOutlinerItem *item = gtk_tree_list_row_get_item(row_item);
    GtkWidget *expander = toutliner_item_get_expander(item);

    expander = gtk_list_item_get_child(list_item);
    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(expander), row_item);

    // Root expander is always expanded
    if (g_strcmp0(toutliner_item_get_name(item), GAPP_ROOT_STR) == 0)
    {
        gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), TRUE);
        gtk_tree_list_row_set_expanded(row_item, TRUE);
    }

    GtkWidget *box = gtk_tree_expander_get_child(expander);
    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_last_child(box);

    gtk_image_set_from_icon_name(GTK_IMAGE(icon), "edit-select-all-symbolic");
    gtk_label_set_label(GTK_LABEL(label), toutliner_item_get_name(item));

    g_object_unref(item);
}

/**
 * s_setup_outliner_popover_entity_item_factory:
 * @factory: El #GtkSignalListItemFactory que llamó a esta función
 * @listitem: El #GtkListItem que representa el elemento en el listview
 * @data: Datos adicionales (no utilizados aquí)
 *
 * Función auxiliar llamada por el #GtkSignalListItemFactory para configurar
 * la estructura visual de cada elemento en el listview del popover de entidades.
 *
 * Esta función crea un contenedor horizontal con un icono y una etiqueta,
 * y los establece como el widget hijo del listitem.
 *
 * Since: 1.0
 */
static void s_setup_outliner_popover_entity_item_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gapp_widget_set_margin(hbox_parent, 8);

    gtk_box_append(GTK_BOX(hbox_parent), gtk_image_new());
    gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(""));

    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), hbox_parent);
}

/**
 * s_bind_outliner_popover_entity_item_factory:
 * @factory: El #GtkSignalListItemFactory que llamó a esta función
 * @listitem: El #GtkListItem que representa el elemento en el listview
 * @data: Datos adicionales (no utilizados aquí)
 *
 * Función auxiliar llamada por el #GtkSignalListItemFactory para configurar
 * la apariencia de cada elemento en el listview del popover de entidades.
 *
 * Esta función obtiene el #TOutlinerPopoverItem asociado al listitem,
 * y actualiza los widgets hijos (icono y etiqueta) con la información
 * del elemento del popover.
 *
 * Since: 1.0
 */
static void s_bind_outliner_popover_entity_item_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    TOutlinerPopoverItem *popoverItem = gtk_list_item_get_item(listitem);

    GtkWidget *hbox_parent = gtk_list_item_get_child(listitem);
    GtkWidget *icon = gtk_widget_get_first_child(hbox_parent);
    GtkWidget *label = gtk_widget_get_last_child(hbox_parent);

    /* Actualizar el icono y la etiqueta con la información del elemento */
    gtk_image_set_from_icon_name(GTK_IMAGE(icon), toutliner_popover_item_get_icon_name(popoverItem));
    gtk_label_set_label(GTK_LABEL(label), toutliner_popover_item_get_name(popoverItem));
}

/**
 * s_outliner_popover_entity_item_activated:
 * @self: El #GtkListView donde se activó el elemento
 * @position: La posición del elemento activado en el listview
 * @popover: El #GtkPopover que contiene el listview
 *
 * Función auxiliar llamada cuando se activa un elemento en el listview
 * del popover de entidades. Esta función se encarga de crear una nueva
 * entidad en el mundo ECS y enfocarla en el esquema del Outliner.
 *
 * El elemento activado en el listview contiene información sobre el
 * tipo de entidad a crear, como su nombre y los componentes a asignar.
 *
 * Since: 1.0
 */
static void s_outliner_popover_entity_item_activated(GtkListView *self, guint position, GtkWidget *popover)
{
    GtkSelectionModel *selection = gtk_list_view_get_model(self);
    TOutlinerPopoverItem *popoverItem = TOUTLINER_POPOVER_ITEM(g_object_ref(g_list_model_get_item(G_LIST_MODEL(selection), position)));

    GappOutliner *outliner = g_object_get_data(G_OBJECT(popover), "outliner");

    TOutlinerItem *itemSelected = gapp_outliner_fn_get_selected_item(outliner->selection);
    ecs_entity_t parent = itemSelected ? toutliner_item_get_entity(itemSelected) : pixio_get_root(outliner->world);
    ecs_entity_t entity = pixio_new(outliner->world, parent, toutliner_popover_item_get_name(popoverItem));

    const char *componentName = toutliner_popover_item_get_component(popoverItem);
    if (strcmp(componentName, "entity.empty") != 0)
        pixio_set_component_by_name(outliner->world, entity, componentName);

    gapp_outliner_fn_focus_entity(outliner, entity);
    gtk_popover_popdown(GTK_POPOVER(popover));
    g_object_unref(popoverItem);
}

// MARK: --- UI ---

/**
 * Crea y devuelve un nuevo GListStore con elementos para un menú desplegable del outliner.
 *
 * Esta función inicializa un GListStore con varios TOutlinerPopoverItem, cada uno
 * representando una entidad o elemento de interfaz de usuario diferente que se puede
 * agregar al outliner. Los elementos incluyen varias entidades gráficas, de audio,
 * y de interfaz de usuario.
 *
 * @return GListStore* Un puntero al nuevo GListStore creado y poblado con elementos.
 */
static GListStore *gapp_outliner_ui_popover_new(void)
{
    GListStore *store = toutliner_popover_new();

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
        g_list_store_append(store, toutliner_popover_item_new(items[i].icon, items[i].label, "", items[i].type));
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

        filter = GTK_FILTER(gtk_string_filter_new(gtk_property_expression_new(TOUTLINER_TYPE_POPOVER_ITEM, NULL, "name")));
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
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_outliner_s_toolbar_duplicate_clicked), self);

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
            self->store = g_list_store_new(TOUTLINER_TYPE_ITEM);

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

            self->list_view = gtk_list_view_new(self->selection, factory);
            // gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(list_view), TRUE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->list_view);

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
 * gapp_outliner_set_name_entity:
 * @outliner: (transfer none): El #GappOutliner que contiene la entidad.
 * @entity: La entidad cuyo nombre se va a cambiar.
 * @name: (transfer none): El nuevo nombre para la entidad.
 *
 * Establece un nuevo nombre para una entidad específica en el esquema y actualiza la interfaz de usuario.
 *
 * Esta función realiza las siguientes operaciones:
 * - Busca el ítem correspondiente a la entidad en la selección del esquema.
 * - Actualiza el nombre del ítem utilizando la función toutliner_item_set_name().
 * - Actualiza la etiqueta en la interfaz de usuario para reflejar el nuevo nombre.
 *
 * Nota: El nuevo nombre se copia internamente, por lo que el llamador mantiene
 * la propiedad de la cadena original.
 *
 * Advertencia: Esta función asume que la entidad existe en el esquema y que
 * la estructura del widget es la esperada. Si la entidad no se encuentra o
 * la estructura del widget ha cambiado, el comportamiento puede ser indefinido.
 *
 * Since: 1.0
 */
void gapp_outliner_set_name_entity(GappOutliner *outliner, ecs_entity_t entity, const char *name)
{
    // Buscar el ítem correspondiente a la entidad
    TOutlinerItem *item = gapp_outliner_fn_find_item_by_entity(outliner->selection, entity);

    // Actualizar el nombre del ítem
    toutliner_item_set_name(item, name);

    // Actualizar la etiqueta en la interfaz de usuario
    GtkWidget *label = gtk_widget_get_last_child(gtk_tree_expander_get_child(toutliner_item_get_expander(item)));
    gtk_label_set_label(GTK_LABEL(label), toutliner_item_get_name(item));
}

void gapp_outliner_append_item_root(GappOutliner *outliner, TOutlinerItem *item)
{
    g_list_store_append(outliner->store, item);
}

GListStore *gapp_outliner_get_store(GappOutliner *outliner)
{
    return outliner->store;
}

void gapp_outliner_append_item_children(GappOutliner *outliner, TOutlinerItem *item)
{
    g_list_store_append(outliner->store, item);
}

GtkSingleSelection *gapp_outliner_get_selection(GappOutliner *outliner)
{
    return outliner->selection;
}

// --- END API ---
