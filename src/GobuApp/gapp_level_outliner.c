#include "gapp_level_outliner.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_level_editor.h"
#include "gapp_level_inspector.h"
#include "gapp_browser.h"

#include "types/type_outliner_item.h"
#include "types/type_outliner_popover_item.h"

#include "pixio/pixio.h"

#include "gapp.h"

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

static GListModel *gapp_outliner_fn_create_list_model_entity_children(GObject *item, GappOutliner *outliner)
{
    TOutlinerItem *oitem = TOUTLINER_ITEM(item);

    GListStore *children = toutliner_item_get_children(oitem);
    GtkWidget *expander = toutliner_item_get_expander(oitem);

    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(children)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), !has_children);

    return G_LIST_MODEL(g_object_ref(children));
}

TOutlinerItem *gapp_outliner_fn_find_item_by_entity(GtkSingleSelection *selection, ecs_entity_t entity)
{
    GListModel *model = gtk_multi_selection_get_model(selection);
    guint n = g_list_model_get_n_items(model);
    for (guint i = 0; i < n; i++)
    {
        GtkTreeListRow *row = g_list_model_get_item(model, i);
        TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

        g_object_unref(row);
        if (toutliner_item_get_entity(oitem) == entity)
        {
            return oitem;
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

static gboolean gapp_outliner_fn_focus_entity(GappOutliner *outliner, ecs_entity_t entity)
{
    g_return_val_if_fail(outliner != NULL && entity != 0, FALSE);

    GtkSelectionModel *selection_model = GTK_SELECTION_MODEL(outliner->selection);
    GListModel *model = gtk_multi_selection_get_model(outliner->selection);
    guint n_items = g_list_model_get_n_items(model);

    for (guint i = 0; i < n_items; i++)
    {
        GtkTreeListRow *row = g_list_model_get_item(model, i);
        TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

        if (toutliner_item_get_entity(oitem) == entity)
        {
            gtk_selection_model_select_item(selection_model, i, TRUE);
            g_object_unref(row);
            return TRUE;
        }
        g_object_unref(row);
    }

    return FALSE;
}

static void gapp_outliner_fn_selected_entity(GappOutliner *outliner, ecs_entity_t entity, gboolean selected)
{
    g_return_if_fail(outliner != NULL && entity != 0);

    if (selected)
    {
        gapp_outliner_fn_focus_entity(outliner, entity);
    }

    GtkWidget *level_editor = gtk_widget_get_ancestor(GTK_WIDGET(outliner), GOBU_TYPE_LEVEL_EDITOR);
    GtkWidget *inspector = gobu_level_editor_get_inspector(level_editor);
    gapp_inspector_set_entity(inspector, GAPP_ECS_WORLD, entity);
}

static ecs_entity_t gapp_outliner_fn_create_entity(GappOutliner *outliner, const gchar *name, const gchar *componentName)
{
    TOutlinerItem *itemSelected = gapp_outliner_fn_get_selected_item(outliner->selection);

    ecs_entity_t parent = itemSelected ? toutliner_item_get_entity(itemSelected) : pixio_get_root(GAPP_ECS_WORLD);
    ecs_entity_t entity = pixio_new(GAPP_ECS_WORLD, parent, name);

    if (strcmp(componentName, "entity.empty") != 0 && strlen(componentName) > 0)
        pixio_set_component_by_name(GAPP_ECS_WORLD, entity, componentName);

    return entity;
}

// MARK: --- SIGNAL ---

static void gapp_outliner_s_toolbar_add_clicked(GtkWidget *widget, GtkWidget *popover)
{
    gtk_popover_popup(GTK_POPOVER(popover));
}

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
            TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

            if (g_strcmp0(toutliner_item_get_name(oitem), GAPP_ROOT_STR) != 0)
            {
                pixio_delete(GAPP_ECS_WORLD, toutliner_item_get_entity(oitem));
                isRemove = TRUE;
            }

            g_object_unref(row);
            g_object_unref(oitem);
        }
    }

    if (isRemove)
        gapp_outliner_fn_selected_entity(outliner, ecs_lookup(GAPP_ECS_WORLD, GAPP_ROOT_STR), TRUE);
}

static void gapp_outliner_s_toolbar_duplicate_clicked(GtkWidget *widget, GappOutliner *outliner)
{
    GListModel *model = gtk_multi_selection_get_model(outliner->selection);
    guint n = g_list_model_get_n_items(model);
    ecs_world_t *world = (GAPP_ECS_WORLD);

    for (gint i = n - 1; i >= 0; i--)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(outliner->selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

            if (g_strcmp0(toutliner_item_get_name(oitem), GAPP_ROOT_STR) != 0)
            {
                ecs_entity_t eclone = pixio_clone(world, toutliner_item_get_entity(oitem));
                gapp_outliner_fn_selected_entity(outliner, eclone, TRUE);
            }

            g_object_unref(row);
            g_object_unref(oitem);
        }
    }
}

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
            TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

            gapp_outliner_fn_selected_entity(outliner, toutliner_item_get_entity(oitem), TRUE);

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

static void column_factory_s_bind_name(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappOutliner *outliner)
{
    GtkTreeListRow *row_item = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_TREE_LIST_ROW(row_item));

    TOutlinerItem *oitem = gtk_tree_list_row_get_item(row_item);

    GtkWidget *expander = gtk_list_item_get_child(list_item);
    toutliner_item_set_expander(oitem, expander);

    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(expander), row_item);

    // Root expander is always expanded
    if (g_strcmp0(toutliner_item_get_name(oitem), GAPP_ROOT_STR) == 0)
    {
        gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), TRUE);
        gtk_tree_list_row_set_expanded(row_item, TRUE);
    }

    GtkWidget *box = gtk_tree_expander_get_child(expander);
    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_last_child(box);

    gtk_image_set_from_icon_name(GTK_IMAGE(icon), "edit-select-all-symbolic");
    gtk_label_set_label(GTK_LABEL(label), toutliner_item_get_name(oitem));

    g_object_unref(oitem);
}

static void s_setup_outliner_popover_entity_item_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gapp_widget_set_margin(hbox_parent, 8);

    gtk_box_append(GTK_BOX(hbox_parent), gtk_image_new());
    gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(""));

    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), hbox_parent);
}

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

static void s_outliner_popover_entity_item_activated(GtkListView *self, guint position, GtkWidget *popover)
{
    GtkSelectionModel *selection = gtk_list_view_get_model(self);
    TOutlinerPopoverItem *popoverItem = TOUTLINER_POPOVER_ITEM(g_object_ref(g_list_model_get_item(G_LIST_MODEL(selection), position)));

    GappOutliner *outliner = g_object_get_data(G_OBJECT(popover), "outliner");

    const gchar *name = toutliner_popover_item_get_name(popoverItem);
    const gchar *component = toutliner_popover_item_get_component(popoverItem);
    ecs_entity_t entity = gapp_outliner_fn_create_entity(outliner, name, component);

    gapp_outliner_fn_focus_entity(outliner, entity);
    gtk_popover_popdown(GTK_POPOVER(popover));
    g_object_unref(popoverItem);
}

static gboolean s_outliner_drop_file_browser(GtkDropTarget *target, const GValue *value, double x, double y, GappOutliner *outliner)
{
    if (G_VALUE_HOLDS(value, G_TYPE_FILE_INFO))
    {
        GFileInfo *file_info = g_value_get_object(value);
        GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

        gchar *pathfile = g_file_get_path(file);
        gchar *filename = g_file_info_get_name(file_info);

        if (gobu_fs_is_extension(filename, BROWSER_FILE_IMAGE))
        {
            gapp_outliner_fn_create_entity(outliner, gobu_fs_get_name(filename, TRUE), "pixio_type_module.pixio_sprite_t");
        }

        return TRUE;
    }

    return FALSE;
}

// MARK: --- UI ---

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

            GtkDropTarget *drop = gtk_drop_target_new(G_TYPE_FILE_INFO, GDK_ACTION_MOVE);
            g_signal_connect(drop, "drop", G_CALLBACK(s_outliner_drop_file_browser), self);
            gtk_widget_add_controller(self->list_view, GTK_EVENT_CONTROLLER(drop));

            g_signal_connect(self->selection, "selection-changed", G_CALLBACK(gapp_outliner_s_list_view_activated), self);
        }
    }
}

// MARK: --- API ---

GappOutliner *gapp_outliner_new(void)
{
    return g_object_new(GAPP_TYPE_OUTLINER,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        NULL);
}

void gapp_outliner_set_name_entity(GappOutliner *outliner, ecs_entity_t entity, const char *name)
{
    // Buscar el ítem correspondiente a la entidad
    TOutlinerItem *oitem = gapp_outliner_fn_find_item_by_entity(outliner->selection, entity);
    GtkWidget *expander = toutliner_item_get_expander(oitem);

    // Actualizar el nombre del ítem
    toutliner_item_set_name(oitem, name);

    // Actualizar la etiqueta en la interfaz de usuario
    GtkWidget *label = gtk_widget_get_last_child(gtk_tree_expander_get_child(expander));
    gtk_label_set_label(GTK_LABEL(label), name);
}

GListStore *gapp_outliner_get_store(GappOutliner *outliner)
{
    return outliner->store;
}

GtkSingleSelection *gapp_outliner_get_selection(GappOutliner *outliner)
{
    return outliner->selection;
}

// --- END API ---
