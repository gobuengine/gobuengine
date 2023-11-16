#include "gapp_level_outliner.h"
#include "gapp_main.h"
#include "gapp_widget.h"

struct _GappLevelOutliner
{
    GtkWidget parent;

    GtkWidget* colview;
    GListStore* store;
    GtkTreeListModel* tree_model;
    GtkSingleSelection* selection;

    ecs_world_t* world;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelOutliner, gapp_level_outliner, GTK_TYPE_BOX);

static void gapp_level_outliner_class_init(GappLevelOutlinerClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

//  -------------------------

extern GAPP* EditorCore;

static gboolean signal_drop(GtkDropTarget* target, const GValue* value, double x, double y, GtkListItem* list_item)
{
    if (G_VALUE_HOLDS(value, OUTLINER_TYPE_ITEM_COLUMN))
    {
        GappOutlineItemColumn* parent = gtk_list_item_get_item(list_item);
        GappOutlineItemColumn* children = g_value_get_object(value);
        // g_return_if_fail(parent);
        // g_return_if_fail(children);
        gapp_level_outliner_item_moveTo(parent, children);

        return TRUE;
    }
    return FALSE;
}

static GdkContentProvider* signal_drag_prepare(GtkDragSource* source, gdouble x, gdouble y, GtkListItem* list_item)
{
    GtkTreeListRow* row_item = gtk_list_item_get_item(list_item);
    if (row_item == NULL)
        return NULL;

    GappOutlineItemColumn* item = gtk_tree_list_row_get_item(row_item);
    return gdk_content_provider_new_typed(OUTLINER_TYPE_ITEM_COLUMN, item);
}

static void signal_factory_setup_name(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* label, * expander;
    GtkDropTarget* drop;
    GtkDragSource* drag;

    expander = gtk_tree_expander_new();
    // gtk_list_item_set_focusable(GTK_LIST_ITEM(listitem), FALSE);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), expander);

    drop = gtk_drop_target_new(OUTLINER_TYPE_ITEM_COLUMN, GDK_ACTION_MOVE);
    g_signal_connect(drop, "drop", G_CALLBACK(signal_drop), listitem);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop));

    drag = gtk_drag_source_new();
    gtk_drag_source_set_actions(drag, GDK_ACTION_MOVE);
    g_signal_connect(drag, "prepare", G_CALLBACK(signal_drag_prepare), listitem);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drag));

    label = gtk_label_new("");
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
}

static void signal_factory_bind_name(GtkSignalListItemFactory* factory, GtkListItem* list_item, gpointer data)
{
    GtkWidget* expander, * child;
    GtkTreeListRow* row_item;

    row_item = gtk_list_item_get_item(list_item);
    GappOutlineItemColumn* item = gtk_tree_list_row_get_item(row_item);

    expander = gtk_list_item_get_child(list_item);
    gtk_tree_expander_set_list_row(expander, row_item);
    gtk_tree_expander_set_hide_expander(expander, TRUE);

    GtkWidget* label = gtk_tree_expander_get_child(expander);
    gtk_label_set_label(GTK_LABEL(label), item->name);

    g_object_unref(item);
}

static void signal_factory_setup_visible(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* checkbox;
    checkbox = gtk_check_button_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), checkbox);
}

static void signal_factory_bind_visible(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* checkbox = gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
    GappOutlineItemColumn* item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), item->visible);
}

static void signal_selection_selected(GObject* object, GParamSpec* pspec, GappLevelOutliner* self)
{
    gint post = gtk_single_selection_get_selected(GTK_SINGLE_SELECTION(object));
    GappOutlineItemColumn* item = gtk_tree_list_row_get_item(gtk_single_selection_get_selected_item(GTK_SINGLE_SELECTION(object)));

    printf("Selected: %d, %s\n", post, item->name);
}

static GListModel* fn_factory_tree_list_model_create(GObject* object, gpointer data)
{
    GappOutlineItemColumn* item = OUTLINER_ITEM_COLUMN(object);

    if (item->children->len > 0) {
        GListStore* store = g_list_store_new(OUTLINER_TYPE_ITEM_COLUMN);
        for (int i = 0; i < item->children->len; i++) {
            GappOutlineItemColumn* item = g_ptr_array_index(item->children, i);
            g_list_store_append(store, item);
        }
        return G_LIST_MODEL(store);
    }

    return NULL;
}

static void fn_create_tree_list_model(GappLevelOutliner* self)
{
    self->store = g_list_store_new(OUTLINER_TYPE_ITEM_COLUMN);
    self->tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->store), FALSE, TRUE, fn_factory_tree_list_model_create, NULL, NULL);
    self->selection = gtk_single_selection_new(G_LIST_MODEL(self->tree_model));
    gtk_column_view_set_model(GTK_COLUMN_VIEW(self->colview), GTK_SELECTION_MODEL(self->selection));
}

static void signal_clicked_add_prefab(GtkWidget* widget, GappLevelOutliner* self)
{

}

static void gapp_level_outliner_init(GappLevelOutliner* self)
{
    GtkWidget* scroll, * toolbar, * item;
    GtkListItemFactory* factory;
    GtkColumnViewColumn* column;

    {
        toolbar = gapp_widget_toolbar_new();
        gtk_box_append(self, toolbar);

        item = gapp_widget_button_new_icon_with_label("list-add-symbolic", NULL);
        gtk_button_set_has_frame(item, FALSE);
        gtk_box_append(GTK_BOX(toolbar), item);
        g_signal_connect(item, "clicked", G_CALLBACK(signal_clicked_add_prefab), self);
        // gapp_widget_toolbar_separator_new(toolbar);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    {
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(self, scroll);
        {
            self->colview = gtk_column_view_new(NULL);
            fn_create_tree_list_model(self);

            gtk_column_view_set_reorderable(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_column_view_set_show_column_separators(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_column_view_set_show_row_separators(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_column_view_set_enable_rubberband(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->colview);
            // g_signal_connect(self->selection, "notify::selected", signal_selection_selected, self);
            {
                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(signal_factory_setup_name), NULL);
                g_signal_connect(factory, "bind", G_CALLBACK(signal_factory_bind_name), NULL);
                {
                    column = gtk_column_view_column_new("Name", factory);
                    gtk_column_view_column_set_expand(column, TRUE);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                    g_object_unref(column);
                }

                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(signal_factory_setup_visible), NULL);
                g_signal_connect(factory, "bind", G_CALLBACK(signal_factory_bind_visible), NULL);
                {
                    column = gtk_column_view_column_new("Visible", factory);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                    g_object_unref(column);
                }
            }
        }
    }
}

GappLevelOutliner* gapp_level_outliner_new(void)
{
    GappLevelOutliner* outliner = g_object_new(GAPP_LEVEL_TYPE_OUTLINER, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
    return outliner;
}

void gapp_level_outliner_append(GappLevelOutliner* outliner, ecs_world_t* world, const gchar* name, uint64_t entity, bool visible)
{
    GappOutlineItemColumn* item = gapp_outline_item_column_new(name, entity, visible);
    // item->world = world; // experimental
    // item->parent = outliner->store;
    g_list_store_append(outliner->store, item);
    debug_print(CONSOLE_INFO, TF("Outliner append entity %lld %s", entity, name));
}

void gapp_level_outliner_item_moveTo(GappOutlineItemColumn* parent, GappOutlineItemColumn* item)
{
    if (parent->entity != item->entity)
    {
        // item->parent = parent->parent;
        // g_ptr_array_add(parent->children, item);
        // g_list_store_append(parent->parent, item);
        // g_list_store_remove(item->parent, position);
        // item->parent = parent->children;
        // g_object_unref(item);

        // ecs_add_pair(parent->world, children->entity, EcsChildOf, parent->entity);
        debug_print(CONSOLE_INFO, TF("Move entity %s to entity %s", item->name, parent->name));
    }
}

void gapp_level_outliner_remove(GappLevelOutliner* outliner, uint64_t entity)
{
    // for (int i = 0; i < g_list_model_get_n_items(outliner->store); i++)
    // {
    //     GappOutlineItemColumn* item = g_list_model_get_object(outliner->store, i);
    //     if (item->entity == entity) {
    //         g_list_store_remove(outliner->store, i);
    //         debug_print(CONSOLE_INFO, TF("Outliner remove entity %lld", entity));
    //         break;
    //     }
    // }
}

