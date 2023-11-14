#include "gapp_level_outliner.h"
#include "gapp_main.h"
#include "gapp_widget.h"

struct _GappLevelOutliner
{
    GtkWidget parent;

    GtkWidget* colview;
    GListStore* store;
    GtkTreeListModel* tree_model;

    ecs_world_t* world;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelOutliner, gapp_level_outliner, GTK_TYPE_BOX);

static void gapp_level_outliner_class_init(GappLevelOutlinerClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

//  -------------------------

extern GAPP* EditorCore;

static void gapp_level_outliner_signal_factory_setup_column_name(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* label;
    GtkWidget* expander;

    label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);

    expander = gtk_tree_expander_new();
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);
    gtk_list_item_set_focusable(GTK_LIST_ITEM(listitem), FALSE);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), expander);
}

static void gapp_level_outliner_signal_factory_bind_column_name(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* label;
    GtkWidget* expander;
    GtkTreeListRow* row;
    GappOutlineItemColumn* itemm;

    expander = GTK_TREE_EXPANDER(gtk_list_item_get_child(listitem));

    row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(listitem));
    gtk_tree_expander_set_list_row(expander, row);

    itemm = OUTLINER_ITEM_COLUMN(gtk_tree_list_row_get_item(row));

    const gchar* name = gapp_outline_item_column_get_name(itemm);

    label = gtk_tree_expander_get_child(expander);
    gtk_label_set_label(GTK_LABEL(label), name);

    gint children_count = GPOINTER_TO_INT(g_object_get_data(itemm->children, "n-items"));

    gtk_tree_expander_set_hide_expander(expander, children_count == 0 && strcmp(name, "Root") != 0 ? TRUE : FALSE);
}

static void gapp_level_outliner_signal_factory_setup_column_visible(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* checkbox;

    checkbox = gtk_check_button_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), checkbox);
}

static void gapp_level_outliner_signal_factory_bind_column_visible(GtkSignalListItemFactory* factory, GtkListItem* listitem, gpointer data)
{
    GtkWidget* checkbox;
    GObject* item;

    checkbox = gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
    item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), gapp_outline_item_column_get_visible(item));
}

static GListModel* gapp_level_outliner_fn_factory_tree_list_model_create(gpointer* item, GappLevelOutliner* self)
{
    GappOutlineItemColumn* itemm = OUTLINER_ITEM_COLUMN(item);

    if (itemm->children)
        return g_object_ref(G_LIST_MODEL(itemm->children));

    return NULL;
}

static void gapp_level_outliner_fn_create_tree_list_model(GappLevelOutliner* self)
{
    GtkTreeListModel* tree_model;
    GtkSingleSelection* selection_model;

    self->store = g_list_store_new(OUTLINER_TYPE_ITEM_COLUMN);

    self->tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->store), FALSE, TRUE,
                                         gapp_level_outliner_fn_factory_tree_list_model_create,
                                         self, NULL);

    selection_model = gtk_single_selection_new(G_LIST_MODEL(self->tree_model));

    gtk_column_view_set_model(GTK_COLUMN_VIEW(self->colview), GTK_SELECTION_MODEL(selection_model));
}

static void assasa(GtkWidget* widget, GappLevelOutliner* self)
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
        g_signal_connect(item, "clicked", G_CALLBACK(assasa), self);
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
            gtk_column_view_set_reorderable(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_column_view_set_show_column_separators(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_column_view_set_show_row_separators(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->colview);
            gtk_widget_add_css_class(self->colview, "data-table");
            // gtk_widget_add_css_class(self->colview, "rich-list");
            {
                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(gapp_level_outliner_signal_factory_setup_column_name), NULL);
                g_signal_connect(factory, "bind", G_CALLBACK(gapp_level_outliner_signal_factory_bind_column_name), NULL);
                {
                    column = gtk_column_view_column_new("Name", factory);
                    gtk_column_view_column_set_expand(column, TRUE);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                    g_object_unref(column);
                }

                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(gapp_level_outliner_signal_factory_setup_column_visible), NULL);
                g_signal_connect(factory, "bind", G_CALLBACK(gapp_level_outliner_signal_factory_bind_column_visible), NULL);
                {
                    column = gtk_column_view_column_new("Visible", factory);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                    g_object_unref(column);
                }
            }

            gapp_level_outliner_fn_create_tree_list_model(self);
        }
    }
}

GappLevelOutliner* gapp_level_outliner_new(void)
{
    GappLevelOutliner* outliner = g_object_new(GAPP_LEVEL_TYPE_OUTLINER, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
    return outliner;
}

void gapp_level_outliner_append(GappLevelOutliner* outliner, const gchar* name, uint64_t entity, bool visible)
{
    GappOutlineItemColumn* item = gapp_outline_item_column_new(name, entity, visible);
    g_list_store_append(outliner->store, item);

    debug_print(CONSOLE_INFO, TF("Outliner append entity %lld %s", entity, name));
}

void gapp_level_outliner_remove(GappLevelOutliner* outliner, uint64_t entity)
{
    for (int i = 0; i < g_list_model_get_n_items(outliner->store); i++)
    {
        GappOutlineItemColumn* item = g_list_model_get_object(outliner->store, i);
        if (item->entity == entity) {
            g_list_store_remove(outliner->store, i);
            debug_print(CONSOLE_INFO, TF("Outliner remove entity %lld", entity));
            break;
        }
    }
}

