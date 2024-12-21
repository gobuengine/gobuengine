#include "gapp_scene_hierarchy.h"
#include "pixio/pixio.h"
#include "gapp_common.h"
#include "gapp_scene_manager.h"
#include "gapp_whierarchy.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappSceneHierarchy
{
    GtkBox parent_instance;

    GtkWidget *hierarchy;
    GtkWidget *listbox;
};

static void gapp_scene_hierarchy_signal_observer_scene(ecs_iter_t *it);
static void gapp_scene_hierarchy_signal_observer_scene_active(ecs_iter_t *it);
static void gapp_scene_hierarchy_signal_create_scene(GtkWidget *widget, GappSceneHierarchy *self);
static void gapp_scene_hierarchy_signal_open_scene(GtkListBox *listbox, GtkListBoxRow *row, GappSceneHierarchy *self);
static void gapp_scene_hierarchy_signal_open_scene_manager(GtkWidget *widget, GappSceneHierarchy *self);

static const gchar *gapp_scene_hierarchy_get_name_scene(GtkListBoxRow *row);
static GtkWidget *gapp_scene_hierarchy_find_by_name(GtkWidget *listbox, const gchar *name);
static void gapp_scene_hierarchy_add(GappSceneHierarchy *self, const gchar *name, bool active, bool main);
static void gapp_scene_hierarchy_remove(GappSceneHierarchy *self, const gchar *name);
static void gapp_scene_hierarchy_set_active(GappSceneHierarchy *self, const gchar *name, bool active);

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
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(self, box);

    GtkWidget *expander = gtk_expander_new(NULL);
    gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
    gtk_box_append(box, expander);
    {
        GtkWidget *boxt = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_margin_start(GTK_WIDGET(boxt), 5);
        gtk_widget_set_margin_end(GTK_WIDGET(boxt), 5);
        gtk_expander_set_label_widget(GTK_EXPANDER(expander), boxt);
        {
            gtk_box_append(GTK_BOX(boxt), gtk_label_new("Scene"));
        }
        {
            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_halign(hbox, GTK_ALIGN_END);
            gtk_widget_set_hexpand(hbox, TRUE);
            gtk_box_append(GTK_BOX(boxt), hbox);

            GtkWidget *btn_open_scene_manager = gtk_button_new_from_icon_name("view-grid-symbolic");
            gtk_button_set_has_frame(GTK_BUTTON(btn_open_scene_manager), FALSE);
            gtk_widget_add_css_class(btn_open_scene_manager, "expander_button");
            g_signal_connect(btn_open_scene_manager, "clicked", G_CALLBACK(gapp_scene_hierarchy_signal_open_scene_manager), self);
            gtk_box_append(GTK_BOX(hbox), btn_open_scene_manager);

            GtkWidget *btn_create_scene = gtk_button_new_from_icon_name("list-add-symbolic");
            gtk_button_set_has_frame(GTK_BUTTON(btn_create_scene), FALSE);
            gtk_widget_add_css_class(btn_create_scene, "expander_button");
            g_signal_connect(btn_create_scene, "clicked", G_CALLBACK(gapp_scene_hierarchy_signal_create_scene), self);
            gtk_box_append(GTK_BOX(hbox), btn_create_scene);
        }
        {
            self->listbox = gtk_list_box_new();
            gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(self->listbox), TRUE);
            gtk_list_box_set_selection_mode(GTK_LIST_BOX(self->listbox), GTK_SELECTION_SINGLE);
            gtk_widget_add_css_class(self->listbox, "listbox_scene_hierarchy");
            gtk_expander_set_child(GTK_EXPANDER(expander), self->listbox);
            g_signal_connect(self->listbox, "row-activated", G_CALLBACK(gapp_scene_hierarchy_signal_open_scene), self);
        }
    }

    gtk_box_append(box, gapp_widget_separator_h());

    self->hierarchy = gapp_widget_hierarchy_new();
    gtk_box_append(box, self->hierarchy);

    // ecs_observer(GWORLD, {
    //     .query.terms = {{EcsPixioTagScene}},
    //     .events = {EcsOnAdd, EcsOnRemove},
    //     .callback = gapp_scene_hierarchy_signal_observer_scene,
    //     .ctx = self,
    // });

    // ecs_observer(GWORLD, {
    //     .query.terms = {{EcsPixioTagScene},{EcsDisabled}},
    //     .events = {EcsOnAdd, EcsOnRemove},
    //     .callback = gapp_scene_hierarchy_signal_observer_scene_active,
    //     .ctx = self,
    // });
}

// -----------------
// MARK:PRIVATE
// -----------------

static const gchar *gapp_scene_hierarchy_get_name_scene(GtkListBoxRow *row)
{
    return g_object_get_data(G_OBJECT(row), "name_scene");
}

static GtkWidget *gapp_scene_hierarchy_find_by_name(GtkWidget *listbox, const gchar *name)
{
    gtk_list_box_select_all(GTK_LIST_BOX(listbox));
    GList *children = gtk_list_box_get_selected_rows(GTK_LIST_BOX(listbox));
    gtk_list_box_unselect_all(GTK_LIST_BOX(listbox));
    for (GList *l = children; l != NULL; l = l->next)
    {
        GtkListBoxRow *row = GTK_LIST_BOX_ROW(l->data);
        const gchar *text = gapp_scene_hierarchy_get_name_scene(row);
        if (g_strcmp0(text, name) == 0)
        {
            g_list_free(children);
            return row;
        }
    }
    g_list_free(children);
    return NULL;
}

static void gapp_scene_hierarchy_add(GappSceneHierarchy *self, const gchar *name, bool active, bool main)
{
    GtkWidget *row = gtk_list_box_row_new();
    g_object_set_data(G_OBJECT(row), "name_scene", stringDup(name));
    gtk_list_box_append(GTK_LIST_BOX(self->listbox), row);
    {
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gapp_widget_set_margin(GTK_WIDGET(hbox), 4);
        gtk_list_box_row_set_child(row, hbox);
        {
            GtkWidget *icon_check = gtk_image_new_from_icon_name("object-select-symbolic");
            gtk_widget_set_visible(icon_check, active);
            gtk_box_append(GTK_BOX(hbox), icon_check);

            GtkWidget *label = gtk_label_new(name);
            gtk_widget_set_halign(label, GTK_ALIGN_START);
            gtk_widget_set_hexpand(GTK_WIDGET(label), TRUE);
            gtk_box_append(GTK_BOX(hbox), label);

            GtkWidget *icon_home = gtk_image_new_from_icon_name("user-home-symbolic");
            gtk_widget_set_visible(icon_home, main);
            gtk_box_append(GTK_BOX(hbox), icon_home);
        }
    }

    gtk_list_box_select_row(GTK_LIST_BOX(self->listbox), GTK_LIST_BOX_ROW(row));
}

static void gapp_scene_hierarchy_remove(GappSceneHierarchy *self, const gchar *name)
{
    GtkListBoxRow *row = gapp_scene_hierarchy_find_by_name(self->listbox, name);
    if (row != NULL){
        printf("remove: %s\n", name);
        gtk_list_box_remove(GTK_LIST_BOX(self->listbox), row);
    }
}

static void gapp_scene_hierarchy_set_active(GappSceneHierarchy *self, const gchar *name, bool active)
{
    GtkListBoxRow *row = gapp_scene_hierarchy_find_by_name(self->listbox, name);
    if (row != NULL)
    {
        GtkWidget *hbox = gtk_list_box_row_get_child(row);
        GtkWidget *icon_check = gtk_widget_get_first_child(hbox);
        gtk_widget_set_visible(icon_check, active);
    }
}

// -----------------
// MARK:SIGNAL
// -----------------

static void gapp_scene_hierarchy_signal_observer_scene(ecs_iter_t *it)
{
    ecs_entity_t event = it->event;
    GappSceneHierarchy *self = GAPP_SCENE_HIERARCHY(it->ctx);

    printf("gapp_scene_hierarchy_signal_observer_scene\n");
}

static void gapp_scene_hierarchy_signal_observer_scene_active(ecs_iter_t *it)
{
    ecs_entity_t event = it->event;
    GappSceneHierarchy *self = GAPP_SCENE_HIERARCHY(it->ctx);

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];
        const char *name = pixio_get_name(it->world, entity);
        gapp_scene_hierarchy_set_active(self, name, pixio_is_alive(it->world, entity));
    }
}

static void gapp_scene_hierarchy_signal_create_scene(GtkWidget *widget, GappSceneHierarchy *self)
{
    g_return_if_fail(self != NULL);

    for (int i = 0;; i++)
    {
        g_autofree gchar *name = i == 0 ? stringDup("Scene") : stringDups("Scene %d", i);
        if (pixio_scene_find_by_name(GWORLD, name) == 0)
        {
            pixio_scene_new(GWORLD, name);
            break;
        }
    }
}

static void gapp_scene_hierarchy_signal_open_scene(GtkListBox *listbox, GtkListBoxRow *row, GappSceneHierarchy *self)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(row != NULL);
    g_return_if_fail(self != NULL);

    const gchar *name = gapp_scene_hierarchy_get_name_scene(row);
    ecs_entity_t entity = pixio_scene_find_by_name(GWORLD, name);
    if (entity != 0)
        pixio_scene_set_active(GWORLD, entity);
}

static void gapp_scene_hierarchy_signal_open_scene_manager(GtkWidget *widget, GappSceneHierarchy *self)
{
    g_return_if_fail(self != NULL);

    GappSceneManager *scene_manager = gapp_scene_manager_new();
    gtk_window_set_transient_for(GTK_WINDOW(scene_manager), GTK_WINDOW(gapp_get_window_instance()));
    gtk_window_present(GTK_WINDOW(scene_manager));
}

// -----------------
// MARK: PUBLIC
// -----------------

GappSceneHierarchy *gapp_scene_hierarchy_new(void)
{
    return g_object_new(GAPP_SCENE_TYPE_HIERARCHY, NULL);
}

// MARK: PUBLIC
