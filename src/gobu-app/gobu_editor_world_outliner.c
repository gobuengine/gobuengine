#include "gobu_editor_world_outliner.h"
#include "gobu_editor_world_outliner_item_column.h"

struct _GobuEditorWorldOutliner
{
    GtkWidget parent_instance;
    GtkWidget *colview;
    GListStore *store_root;
    GobuEditorWorldOutliner *item_root;
};

G_DEFINE_FINAL_TYPE(GobuEditorWorldOutliner, gobu_editor_world_outliner, GTK_TYPE_WIDGET)

static void gobu_editor_world_outliner_class_init(GobuEditorWorldOutlinerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void gobu_signal_factory_setup_column_name(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *label;
    GtkWidget *expander;

    label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);

    expander = gtk_tree_expander_new();
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);

    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), expander);
}

static void gobu_signal_factory_bind_column_name(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *label;
    GtkWidget *expander;
    GtkTreeListRow *row;
    GobuEditorWorldOutliner *itemm;

    expander = GTK_TREE_EXPANDER(gtk_list_item_get_child(listitem));
    row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(listitem));

    itemm = gtk_tree_list_row_get_item(row);

    g_assert(GTK_IS_TREE_EXPANDER(expander));
    g_assert(GTK_IS_TREE_LIST_ROW(row));
    g_assert(WORLD_OUTLINER_IS_ITEM_COLUMN(itemm));

    gtk_tree_expander_set_list_row(expander, row);

    const gchar *name = gobu_world_outline_item_column_get_name(itemm);

    label = gtk_tree_expander_get_child(expander);
    gtk_label_set_label(GTK_LABEL(label), strcmp(name, "Root") == 0 ? g_strdup_printf("<b>%s</b>", name) : name);
}

static void gobu_signal_factory_setup_column_visible(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *checkbox;

    checkbox = gtk_check_button_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), checkbox);
}

static void gobu_signal_factory_bind_column_visible(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *checkbox;
    GObject *item;

    checkbox = gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
    item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), gobu_world_outline_item_column_get_visible(item));
}

static GListModel *gobu_fn_factory_tree_list_model_create_fn(GobuWorldOutlineItemColumn *item, gpointer user_data)
{
    return G_LIST_MODEL(item->children);
}

static void gobu_fn_create_tree_list_model(GobuEditorWorldOutliner *self)
{
    GtkTreeListModel *tree_model;
    GtkSingleSelection *selection_model;

    self->store_root = g_list_store_new(WORLD_OUTLINER_TYPE_ITEM_COLUMN);
    self->item_root = gobu_world_outline_item_column_new("Root", TRUE);
    g_list_store_append(self->store_root, self->item_root);

    tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->store_root), FALSE, TRUE,
                                         (GtkTreeListModelCreateModelFunc)gobu_fn_factory_tree_list_model_create_fn,
                                         NULL, NULL);

    selection_model = gtk_single_selection_new(G_LIST_MODEL(tree_model));

    gtk_column_view_set_model(GTK_COLUMN_VIEW(self->colview),
                              GTK_SELECTION_MODEL(selection_model));
}

static void gobu_editor_world_outliner_init(GobuEditorWorldOutliner *self)
{
    GtkWidget *scroll;
    GtkListItemFactory *factory;
    GtkColumnViewColumn *column;

    scroll = gtk_scrolled_window_new();
    gtk_widget_set_parent(scroll, GTK_WIDGET(self));
    {
        self->colview = gtk_column_view_new(NULL);
        gtk_column_view_set_reorderable(GTK_COLUMN_VIEW(self->colview), FALSE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->colview);
        // gtk_widget_add_css_class(self->colview, "data-table");
        {
            factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(gobu_signal_factory_setup_column_name), NULL);
            g_signal_connect(factory, "bind", G_CALLBACK(gobu_signal_factory_bind_column_name), NULL);
            {
                column = gtk_column_view_column_new("Name", factory);
                gtk_column_view_column_set_expand(column, TRUE);
                gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                g_object_unref(column);
            }

            factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(gobu_signal_factory_setup_column_visible), NULL);
            g_signal_connect(factory, "bind", G_CALLBACK(gobu_signal_factory_bind_column_visible), NULL);
            {
                column = gtk_column_view_column_new("Visible", factory);
                gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                g_object_unref(column);
            }
        }

        gobu_fn_create_tree_list_model(self);
    }
}

GobuEditorWorldOutliner *gobu_editor_world_outliner_new(void)
{
    return g_object_new(GOBU_EDITOR_TYPE_WORLD_OUTLINER, NULL);
}

void gobu_editor_world_outliner_append(GObject *parent, GObject *child)
{
    GobuWorldOutlineItemColumn *parentt = WORLD_OUTLINER_ITEM_COLUMN(parent);

    if (parentt->children == NULL)
        parentt->children = gobu_world_outline_item_column_new_children();

    g_list_store_append(parentt->children, WORLD_OUTLINER_ITEM_COLUMN(child));
}
