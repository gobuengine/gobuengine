#include "gapp_scene_hierarchy.h"
#include "gobu/gobu.h"
#include "gapp_common.h"
#include "gapp_scene_manager.h"
#include "gapp_whierarchy.h"
#include "gapp_widget.h"
#include "gapp.h"

#include "types/type_outliner_item.h"

struct _GappSceneHierarchy
{
    GtkBox parent_instance;

    GtkWidget *hierarchy;
    GtkWidget *listbox;
    GtkWidget *scene_label;
    // hierarchy
    GtkSingleSelection *selection;
    GListStore *store;
};

static void signal_open_scene_manager(GtkWidget *widget, GappSceneHierarchy *self);
static void signal_observer_scene_changed(ecs_iter_t *it);
static void signal_hierarchy_list_setup_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, GappSceneHierarchy *self);
static void signal_hierarchy_list_bind_factory(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappSceneHierarchy *self);
static GListModel *signal_hierarchy_list_createchildmodelfortreerow(GObject *item, GappSceneHierarchy *self);

// MARK: CLASS
G_DEFINE_TYPE(GappSceneHierarchy, gapp_scene_hierarchy, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappSceneHierarchy *self = GAPP_SCENE_HIERARCHY(object);
    G_OBJECT_CLASS(gapp_scene_hierarchy_parent_class)->dispose(object);
}

static void gapp_scene_hierarchy_class_init(GappSceneHierarchyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_scene_hierarchy_init(GappSceneHierarchy *self)
{
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_start(GTK_WIDGET(hbox), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(hbox), 5);
    gtk_box_append(self, hbox);
    {
        GtkWidget *icon_scene = gtk_image_new_from_icon_name("view-pin-symbolic");
        gtk_widget_set_margin_end(GTK_WIDGET(icon_scene), 3);
        gtk_box_append(GTK_BOX(hbox), icon_scene);

        self->scene_label = gtk_label_new("<b>Scene</b>");
        gtk_label_set_use_markup(GTK_LABEL(self->scene_label), TRUE);
        gtk_widget_set_halign(self->scene_label, GTK_ALIGN_START);
        gtk_widget_set_hexpand(self->scene_label, TRUE);
        gtk_box_append(GTK_BOX(hbox), self->scene_label);

        GtkWidget *btn_open_scene_manager = gtk_button_new_from_icon_name("view-grid-symbolic");
        gtk_button_set_has_frame(GTK_BUTTON(btn_open_scene_manager), FALSE);
        gtk_widget_add_css_class(btn_open_scene_manager, "expander_button");
        g_signal_connect(btn_open_scene_manager, "clicked", G_CALLBACK(signal_open_scene_manager), self);
        gtk_box_append(GTK_BOX(hbox), btn_open_scene_manager);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    {
        GtkWidget *box_hierarchy = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_append(GTK_BOX(self), box_hierarchy);

        GtkWidget *search = gtk_search_entry_new();
        gtk_search_entry_set_placeholder_text(GTK_SEARCH_ENTRY(search), "Search");
        gtk_widget_set_margin_start(GTK_WIDGET(search), 5);
        gtk_widget_set_margin_end(GTK_WIDGET(search), 5);
        gtk_widget_set_hexpand(search, TRUE);
        gtk_box_append(GTK_BOX(box_hierarchy), search);

        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(box_hierarchy, scroll);
        {
            GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_hierarchy_list_setup_factory), self);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_hierarchy_list_bind_factory), self);

            self->store = g_list_store_new(TOUTLINER_TYPE_ITEM);
            self->selection = gtk_multi_selection_new(G_LIST_MODEL(gtk_tree_list_model_new(G_LIST_MODEL(self->store), FALSE, FALSE, signal_hierarchy_list_createchildmodelfortreerow, self, NULL)));

            GtkWidget *list_view = gtk_list_view_new(self->selection, factory);
            gtk_list_view_set_single_click_activate(GTK_LIST_VIEW(list_view), TRUE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);
        }
    }

    go_ecs_observer(gbOnSceneOpen, signal_observer_scene_changed, self);
    go_ecs_observer(gbOnSceneRename, signal_observer_scene_changed, self);
}

// -----------------
// MARK:PRIVATE
// -----------------

// -----------------
// MARK:SIGNAL
// -----------------

static void signal_open_scene_manager(GtkWidget *widget, GappSceneHierarchy *self)
{
    g_return_if_fail(self != NULL);

    GappSceneManager *scene_manager = gapp_scene_manager_new();
    gtk_window_set_transient_for(GTK_WINDOW(scene_manager), GTK_WINDOW(gapp_get_window_instance()));
    gtk_window_present(GTK_WINDOW(scene_manager));
}

static void signal_observer_scene_changed(ecs_iter_t *it)
{
    GappSceneHierarchy *self = it->ctx;

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];
        if (go_ecs_is_enabled(entity))
        {
            const gchar *name = go_ecs_name(entity);
            gtk_label_set_text(GTK_LABEL(self->scene_label), g_strdup_printf("<b>Scene</b> (%s)", name));
            gtk_label_set_use_markup(GTK_LABEL(self->scene_label), TRUE);
        }
    }
}

static void signal_hierarchy_list_setup_factory(GtkSignalListItemFactory *factory, GtkListItem *listitem, GappSceneHierarchy *self)
{
    GtkTreeExpander *expander = gtk_tree_expander_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), expander);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gapp_widget_set_margin(box, 6);
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), box);

    GtkWidget *icon = gtk_image_new();
    gtk_box_append(GTK_BOX(box), icon);

    GtkWidget *label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_widget_set_margin_start(label, 5);
    gtk_widget_set_margin_end(label, 5);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(box), label);
}

static void signal_hierarchy_list_bind_factory(GtkSignalListItemFactory *factory, GtkListItem *list_item, GappSceneHierarchy *self)
{
    GtkTreeListRow *row_item = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_TREE_LIST_ROW(row_item));

    TOutlinerItem *oitem = gtk_tree_list_row_get_item(row_item);

    GtkTreeExpander *expander = gtk_list_item_get_child(list_item);
    toutliner_item_set_expander(oitem, expander);

    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(expander), row_item);

    GtkWidget *box = gtk_tree_expander_get_child(expander);
    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_last_child(box);

    gtk_image_set_from_icon_name(GTK_IMAGE(icon), "edit-select-all-symbolic");
    gtk_label_set_label(GTK_LABEL(label), toutliner_item_get_name(oitem));

    g_object_unref(oitem);
}

static GListModel *signal_hierarchy_list_createchildmodelfortreerow(GObject *item, GappSceneHierarchy *self)
{
    TOutlinerItem *oitem = TOUTLINER_ITEM(item);

    GListStore *children = toutliner_item_get_children(oitem);
    GtkTreeExpander *expander = toutliner_item_get_expander(oitem);

    gboolean has_children = (g_list_model_get_n_items(G_LIST_MODEL(children)) > 0);
    gtk_tree_expander_set_hide_expander(GTK_TREE_EXPANDER(expander), !has_children);

    return G_LIST_MODEL(g_object_ref(children));
}

// -----------------
// MARK:UI
// -----------------

// -----------------
// MARK: PUBLIC
// -----------------

GappSceneHierarchy *gapp_scene_hierarchy_new(void)
{
    return g_object_new(GAPP_SCENE_TYPE_HIERARCHY, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 5, NULL);
}
