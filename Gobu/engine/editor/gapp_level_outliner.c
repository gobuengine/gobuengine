#include "gapp_level_outliner.h"
#include "gapp_main.h"
#include "gapp_widget.h"

// -------------------------
// BEGIN DEFINE TYPE
// ------------------------

struct _ObjectOutlinerItem
{
    GObject parent;

    char* name;
    gboolean visible;
    gboolean expand;
    ecs_entity_t entity;
    ecs_entity_t root;
    ecs_world_t* world;

    GPtrArray *children;
};

G_DEFINE_TYPE(ObjectOutlinerItem, object_outliner_item, G_TYPE_OBJECT);

static void object_outliner_item_init(ObjectOutlinerItem* item)
{

}

static void object_outliner_item_finalize(GObject* object)
{
    ObjectOutlinerItem* self = OBJECT_OUTLINER_ITEM(object);
    g_free(self->name);
    G_OBJECT_CLASS(object_outliner_item_parent_class)->finalize(object);
}

static void object_outliner_item_class_init(ObjectOutlinerItemClass* class)
{
    GObjectClass* object_class = G_OBJECT_CLASS(class);
    object_class->finalize = object_outliner_item_finalize;
}

ObjectOutlinerItem* object_outliner_item_new(ecs_world_t* world, ecs_entity_t entity)
{
    ObjectOutlinerItem* item = g_object_new(OBJECT_TYPE_OUTLINER_ITEM, NULL);
    item->name = gb_strdup(gb_ecs_entity_get_name(world, entity));
    item->visible = ecs_is_alive(world, entity);
    item->expand = FALSE;
    item->entity = entity;
    item->world = world;
    item->children = g_ptr_array_new();
    return item;
}

static gboolean equal_func(gpointer a, gpointer b)
{
    ObjectOutlinerItem* aa = OBJECT_OUTLINER_ITEM(a);
    ObjectOutlinerItem* bb = OBJECT_OUTLINER_ITEM(b);

    return (aa->entity == bb->entity);
}

// -------------------------
// END DEFINE TYPE
// -------------------------


// -------------------------
// BEGIN DEFINE OUTLINER
// ------------------------
struct _GappLevelOutliner
{
    GtkWidget parent;
    GappLevelEditor* editor;

    GtkWidget* colview;
    GtkTreeListModel* tree_model;
    GtkSingleSelection* selection;

    GListStore* store;
    ecs_world_t* world;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelOutliner, gapp_level_outliner, GTK_TYPE_BOX);

static void gapp_level_outliner_class_init(GappLevelOutlinerClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static GListModel* fn_factory_tree_list_model_create(GObject* object, GappLevelOutliner* outliner)
{
    ObjectOutlinerItem* item = OBJECT_OUTLINER_ITEM(object);
    GListStore* store = NULL;
    if (item->children->len > 0)
    {
        store = g_list_store_new(OBJECT_TYPE_OUTLINER_ITEM);
        for (int i = 0; i < item->children->len; i++)
        {
            g_list_store_append(store, g_ptr_array_index(item->children, i));
            fn_factory_tree_list_model_create(item, outliner);
        }
    }

    return G_LIST_MODEL(store);
}

static gboolean signal_drop(GtkDropTarget* target, const GValue* value, double x, double y, GtkListItem* list_item)
{
    if (G_VALUE_HOLDS(value, OBJECT_TYPE_OUTLINER_ITEM))
    {
        ObjectOutlinerItem* parent = OBJECT_OUTLINER_ITEM(gtk_tree_list_row_get_item(gtk_list_item_get_item(list_item)));
        ObjectOutlinerItem* children = g_value_get_object(value);
        return TRUE;
    }
    return FALSE;
}

static GdkContentProvider* signal_drag_prepare(GtkDragSource* source, gdouble x, gdouble y, GtkListItem* list_item)
{
    GtkTreeListRow* row_item = gtk_list_item_get_item(list_item);
    if (row_item == NULL)
        return NULL;

    ObjectOutlinerItem* item = gtk_tree_list_row_get_item(row_item);
    return gdk_content_provider_new_typed(OBJECT_TYPE_OUTLINER_ITEM, item);
}

static void signal_factory_setup_name(GtkSignalListItemFactory* factory, GtkListItem* listitem, GappLevelOutliner* outliner)
{
    GtkWidget* label, * expander;
    GtkDropTarget* drop;
    GtkDragSource* drag;

    expander = gtk_tree_expander_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), expander);

    // drop = gtk_drop_target_new(OBJECT_TYPE_OUTLINER_ITEM, GDK_ACTION_MOVE);
    // g_signal_connect(drop, "drop", G_CALLBACK(signal_drop), listitem);
    // gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop));

    drag = gtk_drag_source_new();
    gtk_drag_source_set_actions(drag, GDK_ACTION_MOVE);
    g_signal_connect(drag, "prepare", G_CALLBACK(signal_drag_prepare), listitem);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drag));

    label = gtk_label_new("");
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
}

static void signal_factory_bind_name(GtkSignalListItemFactory* factory, GtkListItem* list_item, GappLevelOutliner* outliner)
{
    GtkWidget* expander, * child;
    GtkTreeListRow* row_item;

    row_item = gtk_list_item_get_item(list_item);
    ObjectOutlinerItem* item = gtk_tree_list_row_get_item(row_item);

    expander = gtk_list_item_get_child(list_item);
    gtk_tree_expander_set_list_row(expander, row_item);
    // gtk_tree_list_row_set_expanded(expander, FALSE);

    GtkWidget* label = gtk_tree_expander_get_child(expander);
    gtk_label_set_label(GTK_LABEL(label), item->name);

    g_object_unref(item);
}

static void signal_factory_setup_visible(GtkSignalListItemFactory* factory, GtkListItem* listitem, GappLevelOutliner* outliner)
{
    GtkWidget* checkbox;
    checkbox = gtk_check_button_new();
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), checkbox);
}

static void signal_factory_bind_visible(GtkSignalListItemFactory* factory, GtkListItem* listitem, GappLevelOutliner* outliner)
{
    GtkTreeListRow* row_item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    ObjectOutlinerItem* item = gtk_tree_list_row_get_item(row_item);

    GtkWidget* checkbox = gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), item->visible);
}

static void signal_selection_selected(GObject* object, GParamSpec* pspec, GappLevelOutliner* self)
{
    gint post = gtk_single_selection_get_selected(GTK_SINGLE_SELECTION(object));
    ObjectOutlinerItem* item = gtk_tree_list_row_get_item(gtk_single_selection_get_selected_item(GTK_SINGLE_SELECTION(object)));
}

static void signal_toolbar_clicked_add_prefab(GtkWidget* widget, GappLevelOutliner* self)
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
        g_signal_connect(item, "clicked", G_CALLBACK(signal_toolbar_clicked_add_prefab), self);
        // gapp_widget_toolbar_separator_new(toolbar);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    {
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(self, scroll);
        {

            self->store = g_list_store_new(OBJECT_TYPE_OUTLINER_ITEM);
            self->tree_model = gtk_tree_list_model_new(G_LIST_MODEL(self->store), FALSE, TRUE, fn_factory_tree_list_model_create, self, NULL);
            self->selection = gtk_single_selection_new(G_LIST_MODEL(self->tree_model));

            self->colview = gtk_column_view_new(NULL);
            gtk_column_view_set_model(GTK_COLUMN_VIEW(self->colview), GTK_SELECTION_MODEL(self->selection));
            gtk_column_view_set_reorderable(GTK_COLUMN_VIEW(self->colview), FALSE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->colview);
            // g_signal_connect(self->selection, "notify::selected", signal_selection_selected, self);
            {
                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(signal_factory_setup_name), self);
                g_signal_connect(factory, "bind", G_CALLBACK(signal_factory_bind_name), self);
                {
                    column = gtk_column_view_column_new("Name", factory);
                    gtk_column_view_column_set_expand(column, TRUE);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                    g_object_unref(column);
                }

                factory = gtk_signal_list_item_factory_new();
                g_signal_connect(factory, "setup", G_CALLBACK(signal_factory_setup_visible), self);
                g_signal_connect(factory, "bind", G_CALLBACK(signal_factory_bind_visible), self);
                {
                    column = gtk_column_view_column_new("Visible", factory);
                    gtk_column_view_append_column(GTK_COLUMN_VIEW(self->colview), column);
                    g_object_unref(column);
                }
            }
        }
    }
}

GappLevelOutliner* gapp_level_outliner_new(GappLevelEditor* editor)
{
    GappLevelOutliner* self = g_object_new(GAPP_LEVEL_TYPE_OUTLINER, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
    {
        self->editor = editor;
    }
    return self;
}

// Hack: set_expanded actualiza la lista borrándola y creándola de nuevo.
// Aprovechamos esta funcionalidad para realizar una actualización rápida...
// En las pruebas realizadas hasta el momento de este comentario, no se han observado parpadeos.
static gboolean _outliner_hack_update_expanded(GtkTreeListRow* row)
{
    gtk_tree_list_row_set_expanded(row, TRUE);
    return FALSE;
}

void gapp_level_outliner_append_entity(GappLevelOutliner* self, ecs_entity_t entity)
{
    gb_world_t* world = gapp_level_editor_get_world(self->editor);
    ObjectOutlinerItem* item = object_outliner_item_new(world, entity);
    g_list_store_append(self->store, item);
}

void gapp_level_outliner_remove_entity(GappLevelOutliner* self, ecs_entity_t entity)
{
    GList* list = g_list_first(self->store);
    while (list != NULL)
    {
        ObjectOutlinerItem* item = list->data;
        if (item->entity == entity)
        {
            g_list_store_remove(self->store, item);
            break;
        }
        list = g_list_next(list);
    }
}

// -------------------------
// END DEFINE OUTLINER
// ------------------------

