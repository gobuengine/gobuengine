#include "gapp_inspector.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_inspector_widgets.h"

#include "pixio/pixio_type.h"

// - - - - - - - - - - - - - -
// MARK:BASE CLASS
// - - - - - - - - - - - - - -

struct _GappInspector
{
    GtkBox parent_instance;
    GtkWidget *size_group;
    GtkWidget *listbox;
};

static void gapp_inspector_dispose(GObject *object);

G_DEFINE_TYPE(GappInspector, gapp_inspector, GTK_TYPE_BOX)

static void gapp_inspector_class_init(GappInspectorClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gapp_inspector_dispose;
}

static void gapp_inspector_dispose(GObject *object)
{
    GappInspector *self = GAPP_INSPECTOR(object);

    G_OBJECT_CLASS(gapp_inspector_parent_class)->dispose(object);
}

static void gapp_inspector_init(GappInspector *self)
{
    self->listbox = gtk_list_box_new();
    gtk_widget_add_css_class(self->listbox, "inspector_list");
    gtk_list_box_set_selection_mode(self->listbox, GTK_SELECTION_NONE);
    gtk_box_append(GTK_BOX(self), self->listbox);


}

// ---------------------------
// WIDGET BASE
// ---------------------------

static GtkWidget *gapp_inspector_group_child_new(GtkWidget *size_group, const char *label_str, GtkWidget *input, GtkOrientation orientation)
{
    GtkWidget *label, *box;

    box = gtk_box_new(orientation, 1);

    if (label_str != NULL)
    {
        label = gtk_label_new(label_str);
        gtk_label_set_xalign(label, 0);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_size_request(label, 100, -1);
        gtk_widget_add_css_class(label, "title-4");
        gtk_box_append(box, label);
    }

    gtk_widget_set_hexpand(input, TRUE);
    gtk_box_append(box, input);
    gtk_size_group_add_widget(size_group, input);

    return box;
}

static GtkWidget *gapp_inspector_group_new(GtkWidget *list, const gchar *title_str)
{
    GtkWidget *expander = gtk_expander_new(NULL);
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
    gtk_widget_add_css_class(expander, "expander_n");
    gtk_list_box_append(list, expander);

    // toolbar expander
    GtkWidget *title = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_expander_set_label_widget(GTK_EXPANDER(expander), title);
    {
        GtkWidget *label = gtk_label_new(title_str);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(title), label);

        if (strcmp(title_str, "pixio_transform_t") != 0)
        {
            // GtkWidget *button_off = gtk_switch_new();
            // gtk_box_append(GTK_BOX(title), button_off);
            // gtk_widget_set_margin_end(button_off, 5);
            // gtk_widget_add_css_class(button_off, "expander_button");
            // gtk_widget_set_margin_start(button_off, 0);
            // gtk_widget_set_tooltip_text(button_off, "Remove component");
            // gtk_switch_set_active(GTK_SWITCH(button_off), TRUE);
            // // g_signal_connect(button_off, "clicked", G_CALLBACK(gapp_inspector_component_remove_clicked), entity);

            GtkWidget *button = gtk_button_new_from_icon_name("user-trash-symbolic");
            gtk_widget_add_css_class(button, "expander_button");
            gtk_box_append(GTK_BOX(title), button);
            gtk_widget_set_margin_start(button, 0);
            gtk_widget_set_tooltip_text(button, "Remove component");
            // g_signal_connect(button, "clicked", G_CALLBACK(gapp_inspector_component_remove_clicked), entity);
        }
    }

    // content expander
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gapp_widget_set_margin(content, 4);
    gtk_expander_set_child(GTK_EXPANDER(expander), content);

    return content;
}

static void gapp_inspector_component_add_props_widget(GtkWidget *content, ecs_world_t *world, void *ptr, ecs_entity_t component)
{
    ecs_meta_cursor_t cursor = ecs_meta_cursor(world, component, ptr);

    ecs_meta_push(&cursor);
    for (int i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i++)
    {
        GtkWidget *input = NULL;
        const char *field_name = ecs_meta_get_member(&cursor);
        if (!field_name)
            break;

        struct WidgetCreator
        {
            ecs_entity_t type;
            GtkWidget *(*create_widget)(ecs_meta_cursor_t);
        } WidgetCreator[] = {
            {ecs_id(ecs_string_t), gapp_inspector_widgets_input_string},
            {ecs_id(ecs_bool_t), gapp_inspector_widgets_input_bool},
            {ecs_id(ecs_u32_t), gapp_inspector_widgets_input_u32},
            {ecs_id(ecs_f64_t), gapp_inspector_widgets_input_f64},
            {ecs_id(ecs_f32_t), gapp_inspector_widgets_input_f32},
            {ecs_id(pixio_vector2_t), gapp_inspector_widgets_input_vector2},
            {ecs_id(pixio_color_t), gapp_inspector_widgets_input_color},
            {ecs_id(pixio_resource_texture_t), gapp_inspector_widgets_input_texture},
            {ecs_id(pixio_resource_font_t), gapp_inspector_widgets_input_font},
            {ecs_id(pixio_texture_filter_t), gapp_inspector_widgets_input_enum},
            {ecs_id(pixio_transform_origin_t), gapp_inspector_widgets_input_enum},
            {0, NULL} // Marca de fin
            // Agregar más tipos según sea necesario
        };

        ecs_entity_t field_type = ecs_meta_get_type(&cursor);

        for (size_t i = 0; i < G_N_ELEMENTS(WidgetCreator); i++)
        {
            if (WidgetCreator[i].type == field_type && WidgetCreator[i].create_widget != NULL)
            {
                input = WidgetCreator[i].create_widget(cursor);
                break;
            }
        }

        if (input)
        {
            GtkWidget *size_group = g_object_get_data(G_OBJECT(content), "size-group");
            GtkWidget *child = gapp_inspector_group_child_new(size_group, field_name, input, GTK_ORIENTATION_HORIZONTAL);
            gtk_box_append(GTK_BOX(content), child);
        }

        ecs_meta_next(&cursor);
    }
    ecs_meta_pop(&cursor);
}

// - - - - - - - - - - - - - -
// MARK:API
// - - - - - - - - - - - - - -

GappInspector *gapp_inspector_new(void)
{
    return g_object_new(GAPP_TYPE_INSPECTOR, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL);
}

void gapp_inspector_set_entity(GappInspector *self, ecs_world_t *world, ecs_entity_t entity)
{
    const ecs_type_t *type = ecs_get_type(world, entity);

    gtk_list_box_remove_all(self->listbox);
    self->size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

    // TODO: add widget based on type
    for (uint32_t i = 0; i < type->count; i++)
    {
        ecs_id_t id = type->array[i];
        ecs_entity_t component = ecs_pair_second(world, id);
        const void *component_ptr = ecs_get_id(world, entity, component);
        if (!component_ptr)
            continue;

        const char *component_name = ecs_get_name(world, component);
        {
            GtkWidget *expander = gapp_inspector_group_new(self->listbox, component_name);
            g_object_set_data(G_OBJECT(expander), "size-group", self->size_group);
            gapp_inspector_component_add_props_widget(expander, world, component_ptr, component);
        }
    }
}
