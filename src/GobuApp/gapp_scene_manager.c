#include "gapp_scene_manager.h"
#include "pixio/pixio.h"
#include "gapp_common.h"
#include "gapp_whierarchy.h"
#include "gapp_widget.h"
#include "gapp.h"

// -------------------------------------------------------------
#define TSCENEITEM_TYPE_ITEM (tscene_Item_get_type())
G_DECLARE_FINAL_TYPE(TSceneItem, tscene_Item, TSCENEITEM, ITEM, GObject)

enum
{
    TSCENE_PROP_0,
    TSCENE_PROP_NAME,
    TSCENE_N_PROPS
};

struct _TSceneItem
{
    GObject parent_instance;
    ecs_entity_t entity;
    gchar *name;
};

G_DEFINE_TYPE(TSceneItem, tscene_Item, G_TYPE_OBJECT)

static void tscene_Item_finalize(GObject *object)
{
    TSceneItem *self = TSCENEITEM_ITEM(object);
    g_free(self->name);
}

static void tscene_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    TSceneItem *self = TSCENEITEM_ITEM(object);

    switch (property_id)
    {
    case TSCENE_PROP_NAME:
        g_value_set_string(value, self->name);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void tscene_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    TSceneItem *self = TSCENEITEM_ITEM(object);

    switch (property_id)
    {
    case TSCENE_PROP_NAME:
        g_free(self->name);
        self->name = g_value_dup_string(value);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void tscene_Item_class_init(TSceneItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = tscene_Item_finalize;
    object_class->get_property = tscene_get_property;
    object_class->set_property = tscene_set_property;

    GParamSpec *pspecs[TSCENE_N_PROPS] = {NULL};
    pspecs[TSCENE_PROP_NAME] = g_param_spec_string("name", "Name", "The name of the item", NULL, G_PARAM_READWRITE);
    g_object_class_install_properties(object_class, TSCENE_N_PROPS, pspecs);
}

static void tscene_Item_init(TSceneItem *self)
{
}

static TSceneItem *tscene_item_new(ecs_entity_t entity, const gchar *name)
{
    TSceneItem *self = g_object_new(TSCENEITEM_TYPE_ITEM, NULL);

    self->entity = entity;
    self->name = stringDup(name);

    return self;
}
// -------------------------------------------------------------

struct _GappSceneManager
{
    GtkBox parent_instance;
    GtkWidget *flowbox;
    GListStore *store;
    ecs_entity_t event_observer1;
};

static GListStore *gapp_scene_manager_get_scene_list(void);
static int g_list_store_sort_by_name(TSceneItem *item_a, TSceneItem *item_b, gpointer user_data);
static void gapp_scene_manager_append(GListStore *store, ecs_entity_t clone);

static void signal_grid_view_activated(GtkGridView *view, guint position, GappSceneManager *self);
static void signal_factory_setup_item(GtkListItemFactory *factory, GtkListItem *list_item, GappSceneManager *self);
static void signal_factory_bind_item(GtkListItemFactory *factory, GtkListItem *list_item, GtkSingleSelection* selection);
static void signal_create_scene(GtkWidget *widget, GappSceneManager *self);
static void signal_observer_scene_create(ecs_iter_t *it);
static void signal_gesture_menu_context_list_item(GtkGesture *gesture, guint n_press, double x, double y, GtkListItem *list_item);
static void signal_duplicate_scene(GtkWidget *button, GtkListItem *list_item);
static void signal_delete_scene(GtkWidget *button, GtkListItem *list_item);


// MARK: CLASS
G_DEFINE_TYPE(GappSceneManager, gapp_scene_manager, GTK_TYPE_WINDOW)

static void object_class_dispose(GObject *object)
{
    GappSceneManager *self = GAPP_SCENE_MANAGER(object);

    ecs_delete(GWORLD, self->event_observer1);

    G_OBJECT_CLASS(gapp_scene_manager_parent_class)->dispose(object);
}

static void gapp_scene_manager_class_init(GappSceneManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_scene_manager_init(GappSceneManager *self)
{
    GtkWidget *search_scene;

    gtk_window_set_modal(GTK_WINDOW(self), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(self), 600, 400);
    gtk_window_set_resizable(GTK_WINDOW(self), FALSE);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gapp_widget_set_margin(vbox, 5);
    gtk_window_set_child(GTK_WINDOW(self), vbox);
    {
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_box_append(GTK_BOX(vbox), hbox);
        {
            search_scene = gtk_search_entry_new();
            gtk_widget_set_hexpand(search_scene, TRUE);
            gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search_scene), "Search scene");
            gtk_box_append(GTK_BOX(hbox), search_scene);

            GtkWidget *btn_new_scene = gapp_widget_button_new_icon_with_label("document-new-symbolic", "New Scene");
            gtk_button_set_has_frame(GTK_BUTTON(btn_new_scene), TRUE);
            g_signal_connect(btn_new_scene, "clicked", G_CALLBACK(signal_create_scene), self);
            gtk_box_append(GTK_BOX(hbox), btn_new_scene);
        }

        GtkWidget *scrolled = gtk_scrolled_window_new();
        gtk_widget_set_vexpand(scrolled, TRUE);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_box_append(GTK_BOX(vbox), scrolled);
        {
            self->store = gapp_scene_manager_get_scene_list();
            g_list_store_sort(self->store, g_list_store_sort_by_name, NULL);

            GtkFilter *filter = GTK_FILTER(gtk_string_filter_new(gtk_property_expression_new(TSCENEITEM_TYPE_ITEM, NULL, "name")));
            GtkFilterListModel *filter_model = gtk_filter_list_model_new(G_LIST_MODEL(self->store), filter);
            gtk_filter_list_model_set_incremental(filter_model, TRUE);
            g_object_bind_property(search_scene, "text", filter, "search", 0);

            GtkSingleSelection* selection = gtk_single_selection_new(G_LIST_MODEL(filter_model));
            gtk_single_selection_set_can_unselect(selection, TRUE);
            gtk_single_selection_set_autoselect(selection, FALSE);

            GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_factory_setup_item), self);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_factory_bind_item), selection);

            GtkWidget *grid = gtk_grid_view_new(GTK_SELECTION_MODEL(selection), factory);
            gtk_widget_set_vexpand(grid, TRUE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), grid);
            g_signal_connect(grid, "activate", G_CALLBACK(signal_grid_view_activated), self);
        }
    }

    self->event_observer1 = pixio_observer(GWORLD, EcsPixioOnCreateScene, signal_observer_scene_create, self);
}

// -----------------
// MARK:PRIVATE
// -----------------

static GListStore *gapp_scene_manager_get_scene_list(void)
{
    GListStore *store = g_list_store_new(TSCENEITEM_TYPE_ITEM);

    ecs_query_t *q = ecs_query(GWORLD, {
        .terms = {{EcsPixioTagScene}, {EcsDisabled, .oper = EcsOptional}},
        .cache_kind = EcsQueryCacheAuto,
    });
    ecs_iter_t it = ecs_query_iter(GWORLD, q);

    while (ecs_query_next(&it))
    {
        for (int i = 0; i < it.count; i++)
        {
            ecs_entity_t entity = it.entities[i];
            gapp_scene_manager_append(store, entity);
        }
    }

    ecs_query_fini(q);

    return store;
}

static int g_list_store_sort_by_name(TSceneItem *item_a, TSceneItem *item_b, gpointer user_data)
{
    return item_a->entity - item_b->entity;
}

static void gapp_scene_manager_append(GListStore *store, ecs_entity_t clone)
{
    const gchar *name = ecs_get_name(GWORLD, clone);
    TSceneItem *item_clone = tscene_item_new(clone, name);
    g_list_store_append(store, item_clone);
}

// -----------------
// MARK:SIGNAL
// -----------------

static void signal_grid_view_activated(GtkGridView *view, guint position, GappSceneManager *self)
{
    GtkSelectionModel *selection = gtk_grid_view_get_model(view);
    TSceneItem *item = TSCENEITEM_ITEM(g_object_ref(g_list_model_get_item(G_LIST_MODEL(selection), position)));

    pixio_scene_open(GWORLD, item->entity);
    gtk_window_close(GTK_WINDOW(self));
}

static void signal_factory_setup_item(GtkListItemFactory *factory, GtkListItem *list_item, GappSceneManager *self)
{
    g_return_if_fail(GTK_IS_LIST_ITEM_FACTORY(factory));
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));
    g_object_set_data(G_OBJECT(list_item), "GappSceneManager", self);

    GtkWidget *box, *imagen, *label;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_list_item_set_child(list_item, box);
    gtk_widget_set_size_request(box, 80, 80);

    imagen = gtk_image_new_from_icon_name(GAPP_PROJECT_MANAGER_ICON);
    gtk_image_set_icon_size(GTK_IMAGE(imagen), GTK_ICON_SIZE_LARGE);
    gtk_widget_set_vexpand(imagen, TRUE);
    gtk_box_append(GTK_BOX(box), imagen);

    label = gtk_label_new(NULL);
    gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_CHAR);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(box), label);

    GtkGesture *gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), GDK_BUTTON_SECONDARY);
    gtk_widget_add_controller(box, GTK_EVENT_CONTROLLER(gesture));
    g_signal_connect(gesture, "pressed", G_CALLBACK(signal_gesture_menu_context_list_item), list_item);
}

static void signal_factory_bind_item(GtkListItemFactory *factory, GtkListItem *list_item, GtkSingleSelection* selection)
{
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkWidget *box, *image, *label;

    box = gtk_list_item_get_child(list_item);
    g_return_if_fail(GTK_IS_BOX(box));

    image = gtk_widget_get_first_child(box);
    g_return_if_fail(GTK_IS_IMAGE(image));

    label = gtk_widget_get_next_sibling(image);
    g_return_if_fail(GTK_IS_LABEL(label));

    TSceneItem *item = gtk_list_item_get_item(list_item);
    g_return_if_fail(TSCENEITEM_IS_ITEM(item));

    g_autofree gchar *thumbnail_path = pathJoin(gapp_get_project_path(), "saved", stringDups("scene_thumbnail_%s.jpg", item->name), NULL);
    if (fsExist(thumbnail_path))
        gtk_image_set_from_file(image, thumbnail_path);
    // g_free(thumbnail_path);

    gtk_label_set_text(GTK_LABEL(label), item->name);

    // Seleccionamos el item si esta activo
    if (pixio_is_enabled(GWORLD, item->entity))
    {
        guint position = gtk_list_item_get_position(list_item);
        gtk_single_selection_set_selected(selection, position);
    }
}

static void signal_create_scene_response(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *name = gapp_widget_entry_get_text(GTK_ENTRY(dialog->entry));
    if (pixio_scene_new(GWORLD, name) == 0)
        gtk_widget_add_css_class(dialog->entry, "error");
    else
        gapp_widget_dialog_entry_text_destroy(dialog);
}

static void signal_create_scene(GtkWidget *widget, GappSceneManager *self)
{
    GappWidgetDialogEntry *dialog = gapp_widget_dialog_new_entry_text(gtk_widget_get_root(widget), "New Scene", "Name");
    gtk_entry_set_max_length(GTK_ENTRY(dialog->entry), 15);
    gtk_entry_set_input_purpose(GTK_ENTRY(dialog->entry), GTK_INPUT_PURPOSE_ALPHA);
    gtk_entry_set_input_hints(GTK_ENTRY(dialog->entry), GTK_INPUT_HINT_LOWERCASE | GTK_INPUT_HINT_SPELLCHECK);
    g_signal_connect(dialog->button_ok, "clicked", G_CALLBACK(signal_create_scene_response), dialog);
}

static void signal_observer_scene_create(ecs_iter_t *it)
{
    GappSceneManager *self = it->ctx;

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];
        gapp_scene_manager_append(self->store, entity);
    }
}

static void signal_gesture_menu_context_list_item(GtkGesture *gesture, guint n_press, double x, double y, GtkListItem *list_item)
{
    GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    GtkWidget *child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);
    child = gtk_widget_get_ancestor(child, GTK_TYPE_BOX);

    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    {
        GtkWidget *menu = gtk_popover_new();
        gtk_widget_set_parent(menu, widget);
        gtk_popover_set_has_arrow(GTK_POPOVER(menu), FALSE);
        gtk_popover_set_pointing_to(GTK_POPOVER(menu), &(GdkRectangle){x, y, 1, 1});
        gtk_popover_set_cascade_popdown(GTK_POPOVER(menu), TRUE);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_popover_set_child(GTK_POPOVER(menu), vbox);
        {
            GtkWidget *btn_duplicate = gapp_widget_button_new_icon_with_label("edit-copy-symbolic", "Duplicate");
            gtk_box_append(GTK_BOX(vbox), btn_duplicate);
            g_signal_connect(btn_duplicate, "clicked", G_CALLBACK(signal_duplicate_scene), list_item);

            GtkWidget *btn_rename = gapp_widget_button_new_icon_with_label("preferences-desktop-keyboard-symbolic", "Rename");
            gtk_box_append(GTK_BOX(vbox), btn_rename);
            // g_signal_connect(btn_rename, "clicked", G_CALLBACK(signal_rename_scene), item);

            gtk_box_append(GTK_BOX(vbox), gapp_widget_separator_h());

            GtkWidget *btn_delete = gapp_widget_button_new_icon_with_label("user-trash-full-symbolic", "Delete");
            gtk_box_append(GTK_BOX(vbox), btn_delete);
            g_signal_connect(btn_delete, "clicked", G_CALLBACK(signal_delete_scene), list_item);
        }

        gtk_popover_popup(GTK_POPOVER(menu));
    }
}

static void signal_duplicate_scene(GtkWidget *button, GtkListItem *list_item)
{
    TSceneItem *item = gtk_list_item_get_item(list_item);
    GappSceneManager *self = g_object_get_data(G_OBJECT(list_item), "GappSceneManager");

    ecs_entity_t clone = pixio_scene_clone(GWORLD, item->entity);
    gapp_scene_manager_append(self->store, clone);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

static void signal_delete_scene(GtkWidget *button, GtkListItem *list_item)
{
    TSceneItem *item = gtk_list_item_get_item(list_item);
    GappSceneManager *self = g_object_get_data(G_OBJECT(list_item), "GappSceneManager");
    guint position = gtk_list_item_get_position(list_item);

    pixio_scene_delete(GWORLD, item->entity);
    g_list_store_remove(self->store, position);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

// -----------------
// MARK:UI
// -----------------

// -----------------
// MARK: PUBLIC
// -----------------

GappSceneManager *gapp_scene_manager_new(void)
{
    return g_object_new(GAPP_SCENE_TYPE_MANAGER, "title", "Scene Manager", NULL);
}

// MARK: PUBLIC
