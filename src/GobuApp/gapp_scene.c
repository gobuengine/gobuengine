#include "gapp_scene.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_viewport.h"
#include "gapp_inspector.h"

#include "types/type_outliner_item.h"
#include "types/type_outliner_popover_item.h"
#include "pixio/pixio.h"
#include "gapp.h"

enum
{
    PROP_0,
    PROP_SCENENAME,
    N_PROPS
};

struct _GappScene
{
    GtkBox parent_instance;
    GtkWidget *viewport;
    ecs_entity_t root;
    ecs_world_t *world;

    struct _outliner
    {
        GtkWidget *self;
        GtkWidget *popoverBtnAdd;
        GtkSingleSelection *selection;
        GListStore *store;
    } outliner;

    GtkWidget *inspector;
    gchar *scene_name;
};

static GParamSpec *properties[N_PROPS];

// MARK:PRIVATE
static GListStore *outlinerBuildEntityComponentItem(void);
static void outlinerSetEntityName(GappScene *scene, ecs_entity_t entity, const char *name);
static TOutlinerItem *outlinerFindItemByEntity(GtkSingleSelection *selection, ecs_entity_t entity);
static TOutlinerItem *outlinerGetSelectedItem(GtkSingleSelection *selection);
static void outlinerEntitySelected(GappScene *scene, ecs_entity_t entity, gboolean selected);
static gboolean outlinerSelectItemByEntity(GappScene *scene, ecs_entity_t entity);
static ecs_entity_t outlinerCreateEntityWithComponent(GappScene *scene, const gchar *name, const gchar *componentName);
static void outlinerEcsHookCallback(ecs_iter_t *it);

static void onOutlinerPopoverItemActivated(GtkListView *self, guint position, GtkWidget *popover);
static void onOutlinerToolbarShowPopover(GtkWidget *widget, GtkWidget *popover);
static void onOutlinerToolbarDuplicate(GtkWidget *widget, GappScene *scene);
static void onOutlinerToolbarRemove(GtkWidget *widget, GappScene *scene);
static GListModel *onOutlinerCreateChildModelForTreeRow(GObject *item, GappScene *scene);
static void onOutlinerSetupItemFactory(GtkSignalListItemFactory *factory, GtkListItem *listitem, GappScene *scene);
static void onOutlinerBindItemFactory(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappScene *scene);
static gboolean onOutlinerReceiveBrowserFileDrop(GtkDropTarget *target, const GValue *value, double x, double y, GappScene *scene);
static void onOutlinerListViewSelectionChanged(GtkMultiSelection *selection, guint position, guint n_items, GappScene *scene);

static GtkWidget *outlinerToolbarSetupInterfacePopoverMenu(GtkWidget *parent, GappScene *scene);
static void gappSceneConfigureInterface(GappScene *scene);
static GtkWidget *setupOutlinerInterface(GappScene *scene);
static GtkWidget *setupInspectorInterface(GappScene *scene);
static GtkWidget *setupViewportInterface(GappScene *scene);

// MARK:BASE CLASS
G_DEFINE_TYPE(GappScene, gapp_scene, GTK_TYPE_BOX)

static void set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GappScene *self = GAPP_SCENE(object);

    switch (property_id)
    {
    case PROP_SCENENAME:
        g_free(self->scene_name);
        self->scene_name = g_value_dup_string(value);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GappScene *self = GAPP_SCENE(object);

    switch (property_id)
    {
    case PROP_SCENENAME:
        g_value_set_string(value, self->scene_name);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void dispose(GObject *object)
{
    GappScene *scene = GAPP_SCENE(object);
    gtk_widget_unparent(scene->outliner.popoverBtnAdd);
    pixio_free(scene->world);
    G_OBJECT_CLASS(gapp_scene_parent_class)->dispose(object);
}

static void gapp_scene_class_init(GappSceneClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = dispose;
    object_class->set_property = set_property;
    object_class->get_property = get_property;

    // Registrar las propiedades del objeto
    properties[PROP_SCENENAME] = g_param_spec_string("scenename", "sceneName", "The file name of the item", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void gapp_scene_init(GappScene *scene)
{
    g_return_if_fail(GAPP_IS_SCENE(scene));
    gappSceneConfigureInterface(scene);

    // Por el momento cada escena o prefab tiene su propio mundo.
    // En un futuro me gustaria hacer que una escena y prefab sean solo una entidad en un mundo global.
    scene->world = pixio_world_init();
    ecs_set_hooks(scene->world, pixio_transform_t, {.on_add = outlinerEcsHookCallback, .on_remove = outlinerEcsHookCallback, .ctx = scene});
    scene->root = pixio_new_root(scene->world);
}

GappScene *gapp_scene_new(const gchar *scene_name)
{
    return g_object_new(GAPP_TYPE_SCENE, "orientation", GTK_ORIENTATION_VERTICAL, "scenename", stringDup(scene_name), NULL);
}

// MARK: PRIVATE
static GListStore *outlinerBuildEntityComponentItem(void)
{
    GListStore *store = toutliner_popover_new();

    const struct
    {
        const char *icon;
        const char *label;
        const char *type;
    } items[] = {
        {"list-add-symbolic", "Entity", GAPP_COMPS_EMPTY},
        {"emblem-photos-symbolic", "Sprite", GAPP_COMPS_SPRITE_RENDER},
        {"emblem-photos-symbolic", "Tiling Sprite", GAPP_COMPS_EMPTY},
        {"emblem-photos-symbolic", "Tilemap", GAPP_COMPS_EMPTY},
        {"preferences-desktop-wallpaper-symbolic", "Animated Sprite", GAPP_COMPS_EMPTY},
        {"microphone-sensitivity-high-symbolic", "Audio Listener", GAPP_COMPS_EMPTY},
        {"audio-volume-medium-symbolic", "Sound", GAPP_COMPS_EMPTY},
        {"camera-photo-symbolic", "Camera", GAPP_COMPS_EMPTY},
        {"list-add-symbolic", "Light", GAPP_COMPS_EMPTY},
        {"input-dialpad-symbolic", "Particle System", GAPP_COMPS_EMPTY},
        {"face-monkey", "Text", GAPP_COMPS_GUI_TEXT},
        {"face-monkey", "Rectangle", GAPP_COMPS_GUI_SHAPE_RECTANGLE},
    };

    for (size_t i = 0; i < G_N_ELEMENTS(items); i++)
    {
        g_list_store_append(store, toutliner_popover_item_new(items[i].icon, items[i].label, "", items[i].type));
    }

    return store;
}

static void outlinerSetEntityName(GappScene *scene, ecs_entity_t entity, const char *name)
{
    TOutlinerItem *oitem = outlinerFindItemByEntity(scene->outliner.selection, entity);
    GtkWidget *expander = toutliner_item_get_expander(oitem);

    toutliner_item_set_name(oitem, name);

    GtkWidget *label = gtk_widget_get_last_child(gtk_tree_expander_get_child(expander));
    gtk_label_set_label(GTK_LABEL(label), name);
}

static TOutlinerItem *outlinerFindItemByEntity(GtkSingleSelection *selection, ecs_entity_t entity)
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

static TOutlinerItem *outlinerGetSelectedItem(GtkSingleSelection *selection)
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

static void outlinerEntitySelected(GappScene *scene, ecs_entity_t entity, gboolean selected)
{
    g_return_if_fail(entity != 0);

    if (selected)
    {
        outlinerSelectItemByEntity(scene, entity);
    }

    inspectorSetEntity(scene->inspector, scene->world, entity);
}

static gboolean outlinerSelectItemByEntity(GappScene *scene, ecs_entity_t entity)
{
    g_return_val_if_fail(entity != 0, FALSE);

    GtkSelectionModel *selection_model = GTK_SELECTION_MODEL(scene->outliner.selection);
    GListModel *model = gtk_multi_selection_get_model(scene->outliner.selection);
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

static ecs_entity_t outlinerCreateEntityWithComponent(GappScene *scene, const gchar *name, const gchar *componentName)
{
    TOutlinerItem *itemSelected = outlinerGetSelectedItem(scene->outliner.selection);

    ecs_entity_t parent = itemSelected ? toutliner_item_get_entity(itemSelected) : pixio_get_root(scene->world);
    ecs_entity_t entity = pixio_new(scene->world, parent, name);

    if (strcmp(componentName, "entity.empty") != 0 && strlen(componentName) > 0)
        pixio_set_component_by_name(scene->world, entity, componentName);

    return entity;
}

static void outlinerEcsHookCallback(ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;
    GappScene *scene = it->ctx;
    GtkSingleSelection *outlinerSelect = scene->outliner.selection;

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];

        if (event == EcsOnAdd)
        {
            TOutlinerItem *oitem = toutliner_item_new(world, entity);
            GListStore *store = scene->outliner.store;

            if (pixio_has_parent(world, entity))
            {
                ecs_entity_t parent = pixio_get_parent(world, entity);
                TOutlinerItem *item_find = outlinerFindItemByEntity(outlinerSelect, parent);
                if (item_find != NULL)
                {
                    store = toutliner_item_get_children(item_find);
                }
            }

            toutliner_item_set_root(oitem, store);
            g_list_store_append(store, oitem);
        }
        else if (event == EcsOnRemove)
        {
            guint position;
            TOutlinerItem *oitem = outlinerFindItemByEntity(outlinerSelect, entity);

            if (oitem && g_list_store_find(toutliner_item_get_root(oitem), oitem, &position))
                g_list_store_remove(toutliner_item_get_root(oitem), position);
        }
    }
}

// MARK: SIGNAL
static void onOutlinerPopoverItemActivated(GtkListView *self, guint position, GtkWidget *popover)
{
    GtkSelectionModel *selection = gtk_list_view_get_model(self);
    TOutlinerPopoverItem *popoverItem = TOUTLINER_POPOVER_ITEM(g_object_ref(g_list_model_get_item(G_LIST_MODEL(selection), position)));

    GappScene *scene = g_object_get_data(G_OBJECT(popover), "scene");

    const gchar *name = toutliner_popover_item_get_name(popoverItem);
    const gchar *component = toutliner_popover_item_get_component(popoverItem);
    ecs_entity_t entity = outlinerCreateEntityWithComponent(scene, name, component);

    outlinerSelectItemByEntity(scene, entity);
    gtk_popover_popdown(GTK_POPOVER(popover));
    g_object_unref(popoverItem);
}

static void onOutlinerToolbarShowPopover(GtkWidget *widget, GtkWidget *popover)
{
    gtk_popover_popup(GTK_POPOVER(popover));
}

static void onOutlinerToolbarDuplicate(GtkWidget *widget, GappScene *scene)
{
    GListModel *model = gtk_multi_selection_get_model(scene->outliner.selection);
    guint n = g_list_model_get_n_items(model);
    ecs_world_t *world = (scene->world);

    for (gint i = n - 1; i >= 0; i--)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(scene->outliner.selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

            if (g_strcmp0(toutliner_item_get_name(oitem), GAPP_ROOT_STR) != 0)
            {
                ecs_entity_t eclone = pixio_clone(world, toutliner_item_get_entity(oitem));
                // outlinerEntitySelected(outliner, eclone, TRUE);
            }

            g_object_unref(row);
            g_object_unref(oitem);
        }
    }
}

static void onOutlinerToolbarRemove(GtkWidget *widget, GappScene *scene)
{
    gboolean isRemove = FALSE;

    GListModel *model = gtk_multi_selection_get_model(scene->outliner.selection);
    guint n = g_list_model_get_n_items(model);

    for (gint i = n - 1; i >= 0; i--)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(scene->outliner.selection), i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

            if (g_strcmp0(toutliner_item_get_name(oitem), GAPP_ROOT_STR) != 0)
            {
                pixio_delete(scene->world, toutliner_item_get_entity(oitem));
                isRemove = TRUE;
            }

            g_object_unref(row);
            g_object_unref(oitem);
        }
    }

    if (isRemove)
        outlinerEntitySelected(scene, ecs_lookup(scene->world, GAPP_ROOT_STR), TRUE);
}

static GListModel *onOutlinerCreateChildModelForTreeRow(GObject *item, GappScene *scene)
{
    TOutlinerItem *oitem = TOUTLINER_ITEM(item);

    GListStore *children = toutliner_item_get_children(oitem);
    GtkWidget *expander = toutliner_item_get_expander(oitem);

    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(children)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), !has_children);

    return G_LIST_MODEL(g_object_ref(children));
}

static void onOutlinerSetupItemFactory(GtkSignalListItemFactory *factory, GtkListItem *listitem, GappScene *scene)
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

static void onOutlinerBindItemFactory(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappScene *scene)
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

static gboolean onOutlinerReceiveBrowserFileDrop(GtkDropTarget *target, const GValue *value, double x, double y, GappScene *scene)
{
    if (G_VALUE_HOLDS(value, G_TYPE_FILE_INFO))
    {
        GFileInfo *file_info = g_value_get_object(value);
        GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

        gchar *pathfile = g_file_get_path(file);
        const gchar *filename = g_file_info_get_name(file_info);

        if (fsIsExtension(filename, BROWSER_FILE_IMAGE))
        {
            outlinerCreateEntityWithComponent(scene, fsGetName(filename, TRUE), GAPP_COMPS_SPRITE_RENDER);
        }

        return TRUE;
    }

    return FALSE;
}

static void onOutlinerListViewSelectionChanged(GtkMultiSelection *selection, guint position, guint n_items, GappScene *scene)
{
    g_return_if_fail(selection != NULL);

    GListModel *model = gtk_multi_selection_get_model(selection);
    GtkSelectionModel *selection_model = GTK_SELECTION_MODEL(selection);
    guint total_items = g_list_model_get_n_items(model);

    for (guint i = 0; i < total_items; i++)
    {
        if (gtk_selection_model_is_selected(selection_model, i))
        {
            GtkTreeListRow *row = g_list_model_get_item(model, i);
            TOutlinerItem *oitem = gtk_tree_list_row_get_item(row);

            outlinerEntitySelected(scene, toutliner_item_get_entity(oitem), TRUE);

            g_object_unref(row);
        }
    }
}

static void onOutlinerSetupPopoverMenuItemFactory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gapp_widget_set_margin(hbox_parent, 8);

    gtk_box_append(GTK_BOX(hbox_parent), gtk_image_new());
    gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(""));

    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), hbox_parent);
}

static void onOutlinerBindPopoverMenuItemFactory(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    TOutlinerPopoverItem *popoverItem = gtk_list_item_get_item(listitem);

    GtkWidget *hbox_parent = gtk_list_item_get_child(listitem);
    GtkWidget *icon = gtk_widget_get_first_child(hbox_parent);
    GtkWidget *label = gtk_widget_get_last_child(hbox_parent);

    /* Actualizar el icono y la etiqueta con la información del elemento */
    gtk_image_set_from_icon_name(GTK_IMAGE(icon), toutliner_popover_item_get_icon_name(popoverItem));
    gtk_label_set_label(GTK_LABEL(label), toutliner_popover_item_get_name(popoverItem));
}

// MARK: UI
static GtkWidget *outlinerToolbarSetupInterfacePopoverMenu(GtkWidget *parent, GappScene *scene)
{
    GtkWidget *box, *item, *icon, *search, *scroll;
    GtkFilter *filter;
    GtkFilterListModel *filter_model;

    GtkWidget *popover = gtk_popover_new();
    gtk_widget_set_parent(popover, parent);
    g_object_set_data(G_OBJECT(popover), "scene", scene);
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
        GListStore *store = outlinerBuildEntityComponentItem();

        filter = GTK_FILTER(gtk_string_filter_new(gtk_property_expression_new(TOUTLINER_TYPE_POPOVER_ITEM, NULL, "name")));
        filter_model = gtk_filter_list_model_new(G_LIST_MODEL(store), filter);
        gtk_filter_list_model_set_incremental(filter_model, TRUE);
        g_object_bind_property(search, "text", filter, "search", 0);

        GtkSingleSelection *selection = gtk_no_selection_new(G_LIST_MODEL(filter_model));

        GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(onOutlinerSetupPopoverMenuItemFactory), NULL);
        g_signal_connect(factory, "bind", G_CALLBACK(onOutlinerBindPopoverMenuItemFactory), NULL);

        GtkWidget *list_view = gtk_list_view_new(selection, factory);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);
        // gtk_list_view_set_show_separators(GTK_LIST_VIEW(list_view), TRUE);
        gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(list_view), TRUE);
        g_signal_connect(list_view, "activate", G_CALLBACK(onOutlinerPopoverItemActivated), popover);
    }

    return popover;
}

static GtkWidget *setupOutlinerInterface(GappScene *scene)
{
    GtkWidget *item, *list_view;
    GtkListItemFactory *factory;
    GtkTreeListModel *tree_model;

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *toolbar = gapp_widget_toolbar_new();
    gtk_box_append(box, toolbar);

    item = gapp_widget_button_new_icon_with_label("list-add-symbolic", NULL);
    gtk_widget_set_size_request(item, -1, 20);
    gtk_box_append(GTK_BOX(toolbar), item);
    {
        scene->outliner.popoverBtnAdd = outlinerToolbarSetupInterfacePopoverMenu(item, scene);
        g_signal_connect(item, "clicked", G_CALLBACK(onOutlinerToolbarShowPopover), scene->outliner.popoverBtnAdd);
    }

    item = gapp_widget_button_new_icon_with_label("edit-copy-symbolic", NULL);
    gtk_widget_set_size_request(item, -1, 20);
    gtk_box_append(GTK_BOX(toolbar), item);
    g_signal_connect(item, "clicked", G_CALLBACK(onOutlinerToolbarDuplicate), scene);

    item = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
    gtk_widget_set_size_request(item, -1, 20);
    gtk_box_append(GTK_BOX(toolbar), item);
    g_signal_connect(item, "clicked", G_CALLBACK(onOutlinerToolbarRemove), scene);

    gtk_box_append(box, gapp_widget_separator_h());

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(box, scroll);

    scene->outliner.store = g_list_store_new(TOUTLINER_TYPE_ITEM);

    tree_model = gtk_tree_list_model_new(G_LIST_MODEL(scene->outliner.store),
                                         FALSE,
                                         FALSE,
                                         onOutlinerCreateChildModelForTreeRow,
                                         scene,
                                         NULL);

    scene->outliner.selection = gtk_multi_selection_new(G_LIST_MODEL(tree_model));

    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(onOutlinerSetupItemFactory), scene);
    g_signal_connect(factory, "bind", G_CALLBACK(onOutlinerBindItemFactory), scene);

    list_view = gtk_list_view_new(scene->outliner.selection, factory);
    // gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(list_view), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);

    GtkDropTarget *drop = gtk_drop_target_new(G_TYPE_FILE_INFO, GDK_ACTION_MOVE);
    g_signal_connect(drop, "drop", G_CALLBACK(onOutlinerReceiveBrowserFileDrop), scene);
    gtk_widget_add_controller(list_view, GTK_EVENT_CONTROLLER(drop));

    g_signal_connect(scene->outliner.selection, "selection-changed", G_CALLBACK(onOutlinerListViewSelectionChanged), scene);

    return box;
}

static GtkWidget *setupInspectorInterface(GappScene *scene)
{
    GtkWidget *inspector = gapp_inspector_new();
    inspectorSetEmpty(inspector, "Select an entity to view its properties");
    return inspector;
}

static GtkWidget *setupViewportInterface(GappScene *scene)
{
    GtkWidget *viewport = gapp_viewport_new();

    // g_signal_connect(viewport, "viewport-ready", G_CALLBACK(gapp_level_viewport_s_ready), self);
    // g_signal_connect(viewport, "viewport-render", G_CALLBACK(gapp_level_viewport_s_render), self);

    return viewport;
}

static void gappSceneConfigureInterface(GappScene *scene)
{
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(paned), 220);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    gtk_box_append(GTK_BOX(scene), paned);
    {
        scene->outliner.self = setupOutlinerInterface(scene);
        gtk_widget_set_size_request(scene->outliner.self, 220, -1);
        gtk_paned_set_start_child(GTK_PANED(paned), scene->outliner.self);

        GtkWidget *paned_b = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_paned_set_resize_end_child(GTK_PANED(paned_b), FALSE);
        gtk_paned_set_shrink_end_child(GTK_PANED(paned_b), FALSE);
        gtk_paned_set_end_child(GTK_PANED(paned), paned_b);
        {
            scene->viewport = setupViewportInterface(scene);
            gapp_viewport_set_custom_render(scene->viewport, FALSE);
            gtk_paned_set_start_child(GTK_PANED(paned_b), scene->viewport);

            scene->inspector = setupInspectorInterface(scene);
            gtk_widget_set_size_request(scene->inspector, 300, -1);
            gtk_paned_set_end_child(GTK_PANED(paned_b), scene->inspector);
        }
    }
}

// MARK: PUBLIC

const gchar *sceneGetName(GappScene *scene)
{
    g_return_val_if_fail(GAPP_IS_SCENE(scene), NULL);
    return scene->scene_name;
}
