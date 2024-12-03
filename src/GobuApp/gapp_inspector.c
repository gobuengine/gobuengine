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
    ecs_entity_t entity;
    ecs_world_t *world;
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

// --
// inspectorWidgetCreateComponentDefaultEntity
// --
static void _entity_enabled_toggled(GtkWidget *widget, GappInspector *self)
{
    gboolean enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(widget));
    pixio_set_enabled(self->world, self->entity, enabled);
    inspectorSetEntity(self, self->world, self->entity);
}

static void _entity_name_changed(GtkEditable *widget, GappInspector *self)
{
    gtk_widget_remove_css_class(GTK_WIDGET(widget), "error");

    const gchar *name = gtk_editable_get_text(widget);

    if (pixio_find_by_name(self->world, name) == 0)
    {
        pixio_set_name(self->world, self->entity, name);
    }
    else
    {
        gtk_widget_add_css_class(GTK_WIDGET(widget), "error");
    }
}

static void inspectorWidgetCreateComponentDefaultEntity(GappInspector *self, GtkWidget *size_group)
{
    bool enabled = pixio_get_enabled(self->world, self->entity);
    bool is_root = pixio_get_root(self->world) == self->entity;
    const char *name = pixio_get_name(self->world, self->entity);

    GtkWidget *content = inspectorWidgetCreateComponentGroup(self->listbox, FALSE, "pixio_entity_t", NULL, 0, 0);

    GtkWidget *entity_enabled = gtk_check_button_new();
    gtk_check_button_set_active(GTK_CHECK_BUTTON(entity_enabled), enabled);
    g_signal_connect(entity_enabled, "toggled", G_CALLBACK(_entity_enabled_toggled), self);
    gtk_box_append(GTK_BOX(content), inspectorWidgetCreateFieldRow(size_group, "enabled", entity_enabled, GTK_ORIENTATION_HORIZONTAL));

    GtkWidget *entity_name = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(entity_name), name);
    gtk_widget_set_sensitive(entity_name, !is_root);
    g_signal_connect(entity_name, "changed", G_CALLBACK(_entity_name_changed), self);
    gtk_box_append(GTK_BOX(content), inspectorWidgetCreateFieldRow(size_group, "name", entity_name, GTK_ORIENTATION_HORIZONTAL));

    GtkWidget *button = gapp_widget_button_new_icon_with_label_custom("list-add-symbolic", "Add component", GTK_ALIGN_CENTER);
    gtk_widget_set_tooltip_text(button, "Add component");
    gtk_button_set_has_frame(GTK_BUTTON(button), TRUE);
    gtk_widget_set_sensitive(button, enabled);
    gtk_box_append(GTK_BOX(content), button);
}

// MARK: PUBLIC
void inspectorSetEntity(GappInspector *self, ecs_world_t *world, ecs_entity_t entity)
{
    gtk_list_box_remove_all(self->listbox);
    self->size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

    self->entity = entity;
    self->world = world;

    inspectorWidgetCreateComponentDefaultEntity(self, self->size_group);

    if (!pixio_get_enabled(world, entity))
        return;

    const ecs_type_t *type = ecs_get_type(world, entity);

    for (uint32_t i = type->count - 1; i < type->count; i--)
    {
        ecs_id_t id = type->array[i];
        ecs_entity_t e_component = ecs_pair_second(world, id);
        const void *component_ptr = ecs_get_id(world, entity, e_component);
        if (!component_ptr)
            continue;

        const char *component_name = ecs_get_name(world, e_component);

        gboolean is_removable = (g_strcmp0(component_name, "pixio_transform_t") != 0 &&
                                 g_strcmp0(component_name, "pixio_entity_t") != 0);

        GtkWidget *expander = inspectorWidgetCreateComponentGroup(self->listbox, is_removable, component_name, world, entity, e_component);
        g_object_set_data(G_OBJECT(expander), "size-group", self->size_group);
        inspectorWidgetCreateComponentInputs(expander, world, component_ptr, e_component);
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

