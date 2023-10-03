#include "gobu_world_outliner.h"
#include "gobu_type_factory_item_view_column.h"

struct _GobuWorldOutliner
{
    GtkWidget parent_instance;
    GtkWidget *colview;
    GListStore *store_root;
    GobuFactoryItemViewColumn *item_root;
};

G_DEFINE_FINAL_TYPE(GobuWorldOutliner, gobu_world_outliner, GTK_TYPE_WIDGET)

static void gobu_world_outliner_class_init(GobuWorldOutlinerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

/**
 * Configura la columna 1 en la fábrica de señales Gobu.
 * Esta función se encarga de establecer la configuración inicial
 * para la columna 1 en la fábrica de señales Gobu, utilizando el
 * objeto de fábrica y el elemento de lista especificados.
 *
 * @param factory   El objeto de fábrica de señales Gobu.
 * @param listitem  El elemento de lista GObject que se asocia con la columna 1.
 */
static void gobu_signal_factory_setup_column1(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
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
 * Vincula los datos a la columna 1 en la fábrica de señales Gobu.
 * Esta función se encarga de establecer la vinculación de datos
 * entre la columna 1 en la fábrica de señales Gobu y el elemento
 * de lista GObject especificado.
 *
 * @param factory   El objeto de fábrica de señales Gobu.
 * @param listitem  El elemento de lista GObject que se vinculará con la columna 1.
 */
static void gobu_signal_factory_bind_column1(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *label;
    GtkWidget *expander;
    GtkTreeListRow *row;
    GobuFactoryItemViewColumn *itemm;

    expander = GTK_TREE_EXPANDER(gtk_list_item_get_child(listitem));
    row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(listitem));

    itemm = gtk_tree_list_row_get_item(row);

    g_assert(GTK_IS_TREE_EXPANDER(expander));
    g_assert(GTK_IS_TREE_LIST_ROW(row));
    g_assert(FACTORY_IS_ITEM(itemm));

    gtk_tree_expander_set_list_row(expander, row);

    const gchar *name = gobu_factory_item_view_column_get_name(itemm);

    label = gtk_tree_expander_get_child(expander);
    gtk_label_set_label(GTK_LABEL(label), strcmp(name,"Root") == 0 ? g_strdup_printf("<b>%s</b>",name) : name);
}

/**
 * Configura la columna 2 en la fábrica de señales Gobu.
 * Esta función se encarga de establecer la configuración inicial
 * para la columna 2 en la fábrica de señales Gobu, utilizando el
 * objeto de fábrica y el elemento de lista especificados.
 *
 * @param factory   El objeto de fábrica de señales Gobu.
 * @param listitem  El elemento de lista GObject que se asocia con la columna 2.
 */
static void gobu_signal_factory_setup_column2(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *checkbox;

    checkbox = gtk_check_button_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), checkbox);
}

/**
 * Vincula los datos a la columna 2 en la fábrica de señales Gobu.
 * Esta función se encarga de establecer la vinculación de datos
 * entre la columna 2 en la fábrica de señales Gobu y el elemento
 * de lista GObject especificado.
 *
 * @param factory   El objeto de fábrica de señales Gobu.
 * @param listitem  El elemento de lista GObject que se vinculará con la columna 2.
 */
static void gobu_signal_factory_bind_column2(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *checkbox;
    GObject *item;

    checkbox = gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
    item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), gobu_factory_item_view_column_get_visible(item));
}

/**
 * Crea un modelo de lista GListModel a partir de un objeto y datos de usuario.
 * Esta función se utiliza para crear un modelo de lista GListModel basado en un
 * objeto GObject y datos de usuario proporcionados.
 *
 * @param item       El objeto GObject que se utilizará para crear el modelo de lista.
 * @param user_data  Datos de usuario opcionales para personalizar la creación del modelo.
 * @return           El modelo de lista GListModel creado.
 */
static GListModel *gobu_fn_factory_tree_list_model_create_fn(GobuFactoryItemViewColumn *item, gpointer user_data)
{
    // if (item->children == NULL)
    //     return NULL;

    return G_LIST_MODEL(item->children);
}

static void gobu_fn_create_tree_list_model(GobuWorldOutliner *self)
{
    GtkTreeListModel *tree_model;
    GtkSingleSelection *selection_model;

    self->store_root = g_list_store_new(GOBU_FACTORY_ITEM_TYPE_VIEW_COLUMN);
    self->item_root = gobu_factory_item_view_column_new("Root", TRUE);
    g_list_store_append(self->store_root, self->item_root);

    tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->store_root), FALSE, TRUE,
                                         (GtkTreeListModelCreateModelFunc)gobu_fn_factory_tree_list_model_create_fn,
                                         NULL, NULL);

    selection_model = gtk_single_selection_new(G_LIST_MODEL(tree_model));

    gtk_column_view_set_model(GTK_COLUMN_VIEW(self->colview),
                              GTK_SELECTION_MODEL(selection_model));
}

/**
 * Inicializa un objeto GobuWorldOutliner.
 * Esta función se encarga de realizar la inicialización necesaria para un objeto
 * GobuWorldOutliner dado, configurando sus propiedades y estados iniciales.
 *
 * @param self  El puntero al objeto GobuWorldOutliner que se va a inicializar.
 */
static void gobu_world_outliner_init(GobuWorldOutliner *self)
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
            g_signal_connect(factory, "setup", G_CALLBACK(gobu_signal_factory_setup_column1), NULL);
            g_signal_connect(factory, "bind", G_CALLBACK(gobu_signal_factory_bind_column1), NULL);
            {
                column = gtk_column_view_column_new("Name", factory);
                gtk_column_view_column_set_expand(column, TRUE);
                gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                g_object_unref(column);
            }

            factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(gobu_signal_factory_setup_column2), NULL);
            g_signal_connect(factory, "bind", G_CALLBACK(gobu_signal_factory_bind_column2), NULL);
            {
                column = gtk_column_view_column_new("Visible", factory);
                gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                g_object_unref(column);
            }
        }

        gobu_fn_create_tree_list_model(self);
    }
}

/**
 * Crea y devuelve un nuevo objeto GobuWorldOutliner.
 * Esta función se utiliza para crear una nueva instancia de GobuWorldOutliner
 * y realizar cualquier configuración inicial necesaria.
 *
 * @return Un puntero al nuevo objeto GobuWorldOutliner creado.
 */
GobuWorldOutliner *gobu_world_outliner_new(void)
{
    return g_object_new(GOBU_TYPE_WORLD_OUTLINER, NULL);
}

/**
 * Añade un hijo al outliner del mundo Gobu.
 *
 * Esta función se encarga de agregar un objeto hijo al outliner del mundo Gobu,
 * donde "parent" representa el objeto padre y "child" el objeto hijo.
 *
 * @param parent  El objeto padre en el outliner del mundo Gobu.
 * @param child   El objeto hijo que se agregará.
 */
void gobu_world_outliner_append(GObject *parent, GObject *child)
{
    GobuFactoryItemViewColumn *parentt = FACTORY_ITEM(parent);

    if (parentt->children == NULL)
        parentt->children = g_list_store_new(GOBU_FACTORY_ITEM_TYPE_VIEW_COLUMN);

    g_list_store_append(parentt->children, FACTORY_ITEM(child));
}
