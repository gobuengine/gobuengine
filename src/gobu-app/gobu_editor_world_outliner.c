#include "gobu_editor_world_outliner.h"
#include "gobu_widget.h"

struct _GobuEditorWorldOutliner
{
    GtkWidget parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuEditorWorldOutliner, gobu_editor_world_outliner, GTK_TYPE_WIDGET)

static void gobu_editor_world_outliner_class_init(GobuEditorWorldOutlinerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

/**
 * Configura el nombre de la columna en una fábrica de elementos de lista de señales en Gobu.
 *
 * @param factory   La fábrica de elementos de lista de señales en la que se realiza la configuración.
 * @param listitem  El elemento de lista al que se aplica la configuración del nombre de la columna.
 * @param data      Datos de usuario opcionales para la configuración del nombre de la columna.
 */
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

/**
 * Vincula el nombre de la columna en una fábrica de elementos de lista de señales en Gobu.
 *
 * @param factory   La fábrica de elementos de lista de señales en la que se realiza la vinculación.
 * @param listitem  El elemento de lista al que se vincula el nombre de la columna.
 * @param data      Datos de usuario opcionales para la vinculación del nombre de la columna.
 */
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

/**
 * Configura la visibilidad de la columna en una fábrica de elementos de lista de señales en Gobu.
 *
 * @param factory   La fábrica de elementos de lista de señales en la que se realiza la configuración.
 * @param listitem  El elemento de lista al que se aplica la configuración de la visibilidad de la columna.
 * @param data      Datos de usuario opcionales para la configuración de la visibilidad de la columna.
 */
static void gobu_signal_factory_setup_column_visible(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *checkbox;

    checkbox = gtk_check_button_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), checkbox);
}

/**
 * Vincula la visibilidad de la columna en una fábrica de elementos de lista de señales en Gobu.
 *
 * @param factory   La fábrica de elementos de lista de señales en la que se realiza la vinculación.
 * @param listitem  El elemento de lista al que se vincula la visibilidad de la columna.
 * @param data      Datos de usuario opcionales para la vinculación de la visibilidad de la columna.
 */
static void gobu_signal_factory_bind_column_visible(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *checkbox;
    GObject *item;

    checkbox = gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
    item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), gobu_world_outline_item_column_get_visible(item));
}

/**
 * Crea un modelo de lista para un árbol en Gobu utilizando una función de creación.
 *
 * @param item       El elemento de columna del árbol para el que se crea el modelo de lista.
 * @param user_data  Datos de usuario opcionales para la creación del modelo de lista.
 *
 * @return Un modelo de lista creado.
 */
static GListModel *gobu_fn_factory_tree_list_model_create_fn(GobuWorldOutlineItemColumn *item, gpointer user_data)
{
    return G_LIST_MODEL(item->children);
}

/**
 * Crea un modelo de lista de árbol en el explorador de mundos del editor en Gobu.
 *
 * @param self  El explorador de mundos en el que se crea el modelo de lista de árbol.
 */
static void gobu_fn_create_tree_list_model(GobuEditorWorldOutliner *self)
{
    GtkTreeListModel *tree_model;
    GtkSingleSelection *selection_model;

    GobuEditorWorldOutlinerPrivate *private = gobu_editor_world_outliner_get_instance_private(self);

    // private->store_root = g_list_store_new(WORLD_OUTLINER_TYPE_ITEM_COLUMN);
    // private->item_root = gobu_world_outline_item_column_new("Root", TRUE);
    // g_list_store_append(private->store_root, private->item_root);

    tree_model = gtk_tree_list_model_new(G_LIST_MODEL(private->store_root), FALSE, TRUE,
                                         (GtkTreeListModelCreateModelFunc)gobu_fn_factory_tree_list_model_create_fn,
                                         NULL, NULL);

    selection_model = gtk_single_selection_new(G_LIST_MODEL(tree_model));

    gtk_column_view_set_model(GTK_COLUMN_VIEW(private->colview),
                              GTK_SELECTION_MODEL(selection_model));
}

/**
 * Inicializa el explorador de mundos del editor en Gobu.
 *
 * @param self  El explorador de mundos que se inicializa.
 */
static void gobu_editor_world_outliner_init(GobuEditorWorldOutliner *self)
{
    GtkWidget *scroll, *vbox, *toolbar, *item;
    GtkListItemFactory *factory;
    GtkColumnViewColumn *column;

    GobuEditorWorldOutlinerPrivate *private = gobu_editor_world_outliner_get_instance_private(self);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_parent(vbox, self);
    {
        toolbar = gobu_widget_toolbar_new();
        gtk_box_append(vbox, toolbar);

        item = gobu_widget_button_new_icon_with_label("application-x-addon-symbolic", "Add Prefab");
        gtk_button_set_has_frame(item, FALSE);
        gtk_box_append(GTK_BOX(toolbar), item);
        // g_signal_connect(item, "clicked", G_CALLBACK(gobu_editor_world_browser_signal_popover), self);
        // gobu_widget_toolbar_separator_new(toolbar);
    }

    gtk_box_append(vbox, gobu_widget_separator_h());

    {
        scroll = gtk_scrolled_window_new();
        gtk_box_append(vbox, scroll);
        {
            private->colview = gtk_column_view_new(NULL);
            gtk_column_view_set_reorderable(GTK_COLUMN_VIEW(private->colview), FALSE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), private->colview);
            // gtk_widget_add_css_class(private->colview, "data-table");
            {
                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(gobu_signal_factory_setup_column_name), NULL);
                g_signal_connect(factory, "bind", G_CALLBACK(gobu_signal_factory_bind_column_name), NULL);
                {
                    column = gtk_column_view_column_new("Name", factory);
                    gtk_column_view_column_set_expand(column, TRUE);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(private->colview), column);
                    g_object_unref(column);
                }

                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(gobu_signal_factory_setup_column_visible), NULL);
                g_signal_connect(factory, "bind", G_CALLBACK(gobu_signal_factory_bind_column_visible), NULL);
                {
                    column = gtk_column_view_column_new("Visible", factory);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(private->colview), column);
                    g_object_unref(column);
                }
            }

            gobu_fn_create_tree_list_model(self);
        }
    }
}

/**
 * Crea un nuevo explorador de mundos en el editor de Gobu.
 *
 * @return Un nuevo objeto de explorador de mundos.
 */
GobuEditorWorldOutliner *gobu_editor_world_outliner_new(void)
{
    return g_object_new(GOBU_EDITOR_TYPE_WORLD_OUTLINER, NULL);
}

/**
 * Agrega un objeto hijo a un objeto padre en el explorador de mundos del editor en Gobu.
 *
 * @param parent  El objeto padre al que se agrega el objeto hijo.
 * @param child   El objeto hijo que se agrega.
 */
void gobu_editor_world_outliner_append(GObject *parent, GObject *child)
{
    GobuWorldOutlineItemColumn *parentt = WORLD_OUTLINER_ITEM_COLUMN(parent);

    if (parentt->children == NULL)
        parentt->children = gobu_world_outline_item_column_new_children();

    g_list_store_append(parentt->children, WORLD_OUTLINER_ITEM_COLUMN(child));
}
