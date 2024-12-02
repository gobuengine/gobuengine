#include "gapp_inspector.h"
#include "gapp_inspector_widgets.h"
#include "pixio/pixio_type.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp.h"

struct _GappInspector
{
    GtkBox parent_instance;
    GtkWidget *size_group;
    GtkWidget *listbox;
};

// MARK: CLASS
G_DEFINE_TYPE(GappInspector, gapp_inspector, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappInspector *self = GAPP_INSPECTOR(object);
    G_OBJECT_CLASS(gapp_inspector_parent_class)->dispose(object);
}

static void gapp_inspector_class_init(GappInspectorClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_inspector_init(GappInspector *self)
{
    self->listbox = gtk_list_box_new();
    gtk_widget_add_css_class(self->listbox, "inspector_list");
    gtk_list_box_set_selection_mode(self->listbox, GTK_SELECTION_NONE);
    gtk_box_append(GTK_BOX(self), self->listbox);
}

GappInspector *gapp_inspector_new(void)
{
    return g_object_new(GAPP_TYPE_INSPECTOR, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL);
}

// MARK: PUBLIC
void inspectorSetEntity(GappInspector *self, ecs_world_t *world, ecs_entity_t entity)
{
    gtk_list_box_remove_all(self->listbox);
    self->size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

    if (!pixio_get_enabled(world, entity))
        return;

    const ecs_type_t *type = ecs_get_type(world, entity);

    for (uint32_t i = 0; i < type->count; i++)
    {
        ecs_id_t id = type->array[i];
        ecs_entity_t e_component = ecs_pair_second(world, id);
        const void *component_ptr = ecs_get_id(world, entity, e_component);
        if (!component_ptr)
            continue;

        const char *component_name = ecs_get_name(world, e_component);

        gboolean is_removable = (g_strcmp0(component_name, "pixio_transform_t") != 0 &&
                                 g_strcmp0(component_name, "pixio_entity_t") != 0);

        GtkWidget *expander = inspectorWidgetCreateGroup(self->listbox, is_removable, component_name, world, entity, e_component);
        g_object_set_data(G_OBJECT(expander), "size-group", self->size_group);
        inspectorWidgetAddComponentToInspector(expander, world, component_ptr, e_component);
    }
}

void inspectorSetEmpty(GappInspector *self, const gchar *message)
{
    gtk_list_box_remove_all(self->listbox);

    GtkWidget *label = gtk_label_new(message);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_list_box_append(self->listbox, label);
}
