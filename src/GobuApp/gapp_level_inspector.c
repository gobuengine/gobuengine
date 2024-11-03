#include "gapp_level_inspector.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_inspector_widgets.h"
#include "gapp_level_editor.h"
#include "gapp_level_outliner.h"

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
        gtk_widget_set_size_request(label, 80, -1);
        gtk_widget_add_css_class(label, "title-4");
        gtk_box_append(box, label);
    }

    gtk_widget_set_hexpand(input, TRUE);
    gtk_widget_set_size_request(input, 240, -1);
    gtk_box_append(box, input);
    gtk_size_group_add_widget(size_group, input);

    return box;
}

static GtkWidget *gapp_inspector_group_new(GtkWidget *list, const gchar *title_str, bool buttonRemove)
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

        if (buttonRemove)
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
            gtk_widget_set_margin_start(button, 0);
            gtk_widget_set_tooltip_text(button, "Remove component");
            gtk_box_append(GTK_BOX(title), button);
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

    const EcsStruct *struct_member = ecs_get(world, component, EcsStruct);

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
            GtkWidget *(*create_widget)(ecs_meta_cursor_t, ecs_member_t *);
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
            {ecs_id(pixio_texture_flip_t), gapp_inspector_widgets_input_enum},
            {ecs_id(pixio_origin_t), gapp_inspector_widgets_input_enum},
            {0, NULL} // Marca de fin
            // Agregar más tipos según sea necesario
        };

        ecs_entity_t field_type = ecs_meta_get_type(&cursor);
        ecs_member_t *member = ecs_vec_get_t(&struct_member->members, ecs_member_t, i);

        for (size_t n = 0; n < G_N_ELEMENTS(WidgetCreator); n++)
        {
            if (WidgetCreator[n].type == field_type && WidgetCreator[n].create_widget != NULL)
            {
                input = WidgetCreator[n].create_widget(cursor, member);
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

// -- -- -- -- -- --
static void gapp_inspector_entity_enabled_toggled(GtkWidget *widget, GappInspector *self)
{
    pixio_entity *pentity = g_object_get_data(G_OBJECT(widget), "entity");

    gboolean enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(widget));
    pixio_set_enabled(pentity->world, pentity->entity, enabled);

    gapp_inspector_set_entity(self, pentity->world, pentity->entity);
}

/**
 * gapp_inspector_entity_name_changed:
 * @widget: (transfer none): El #GtkEditable que emitió la señal de cambio.
 * @self: (transfer none): El #GappInspector asociado.
 *
 * Maneja el cambio de nombre de una entidad en el inspector.
 *
 * Esta función se activa cuando el usuario modifica el nombre de una entidad
 * en el widget editable del inspector. Realiza las siguientes operaciones:
 *
 * 1. Obtiene la entidad asociada al widget editable.
 * 2. Obtiene el nuevo nombre del widget editable.
 * 3. Actualiza el nombre de la entidad en el mundo del juego.
 * 4. Actualiza el nombre de la entidad en el esquema (outliner) de la interfaz.
 *
 * Nota: Esta función asume que el widget editable tiene asociada una entidad
 * como dato de usuario con la clave "entity".
 *
 * Advertencia: Esta función depende de la estructura jerárquica de los widgets
 * para encontrar el editor de niveles y el esquema. Si esta estructura cambia,
 * la función podría fallar.
 *
 * Since: 1.0
 */
static void gapp_inspector_entity_name_changed(GtkEditable *widget, GappInspector *self)
{
    // Borramos la class error del input
    gtk_widget_remove_css_class(GTK_WIDGET(widget), "error");

    // Obtener la entidad asociada al widget
    pixio_entity *pentity = g_object_get_data(G_OBJECT(widget), "entity");

    // Obtener el nuevo nombre del widget editable
    const gchar *name = gtk_editable_get_text(widget);

    // Actualizar el nombre de la entidad en el mundo del juego
    bool success = pixio_set_name(pentity->world, pentity->entity, name);
    if (success)
    {
        GtkWidget *level_editor = gtk_widget_get_ancestor(GTK_WIDGET(self), GOBU_TYPE_LEVEL_EDITOR);
        GtkWidget *outliner = gobu_level_editor_get_outliner(level_editor);
        gapp_outliner_set_name_entity(outliner, pentity->entity, name);
    }
    else
        gtk_widget_add_css_class(GTK_WIDGET(widget), "error");
}

static void gapp_inspector_component_default(GappInspector *self, ecs_world_t *world, ecs_entity_t entity)
{
    GtkWidget *size_group = self->size_group;

    pixio_entity *pentity = pixio_entity_new(world, entity);

    const char *name = pixio_get_name(world, entity);
    bool enabled = pixio_get_enabled(world, entity);

    GtkWidget *expander = gapp_inspector_group_new(self->listbox, "entity", FALSE);

    // enabled input
    GtkWidget *entity_enabled = gtk_check_button_new();
    gtk_check_button_set_active(GTK_CHECK_BUTTON(entity_enabled), enabled);
    gtk_widget_set_valign(entity_enabled, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(expander), gapp_inspector_group_child_new(size_group, "enabled", entity_enabled, GTK_ORIENTATION_HORIZONTAL));
    g_object_set_data(G_OBJECT(entity_enabled), "entity", pentity);
    g_signal_connect(entity_enabled, "toggled", G_CALLBACK(gapp_inspector_entity_enabled_toggled), self);

    // name input
    GtkWidget *entity_name = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(entity_name), name);
    gtk_widget_set_valign(entity_name, GTK_ALIGN_CENTER);
    gtk_widget_set_sensitive(entity_name, !(pixio_get_root(world) == entity));
    gtk_box_append(GTK_BOX(expander), gapp_inspector_group_child_new(size_group, "name", entity_name, GTK_ORIENTATION_HORIZONTAL));
    g_object_set_data(G_OBJECT(entity_name), "entity", pentity);
    g_signal_connect(entity_name, "changed", G_CALLBACK(gapp_inspector_entity_name_changed), self);

    // tags input
    // GtkWidget *entity_tags = gtk_entry_new();
    // gtk_widget_set_valign(entity_tags, GTK_ALIGN_CENTER);
    // gtk_box_append(GTK_BOX(expander), gapp_inspector_group_child_new(size_group, "tags", entity_tags, GTK_ORIENTATION_HORIZONTAL));

    // button add components
    GtkWidget *button = gapp_widget_button_new_icon_with_label("list-add-symbolic", "Add component");
    gtk_widget_set_tooltip_text(button, "Add component");
    gtk_button_set_has_frame(GTK_BUTTON(button), TRUE);
    gtk_box_append(GTK_BOX(expander), button);
}

// - - - - - - - - - - - - - -
// MARK:API
// - - - - - - - - - - - - - -

GappInspector *gapp_inspector_new(void)
{
    return g_object_new(GAPP_TYPE_INSPECTOR, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL);
}

/**
 * gapp_inspector_set_entity:
 * @self: (transfer none): El #GappInspector que se va a actualizar.
 * @world: (transfer none): El mundo ECS que contiene la entidad.
 * @entity: La entidad cuyos componentes se van a mostrar.
 *
 * Actualiza el GappInspector para mostrar los componentes de la entidad especificada.
 *
 * Esta función realiza las siguientes operaciones:
 * - Limpia la vista actual del inspector.
 * - Crea un nuevo grupo de tamaño horizontal.
 * - Agrega el componente por defecto.
 * - Para cada componente de la entidad:
 *   - Crea un widget expandible.
 *   - Agrega los widgets de propiedades del componente.
 *
 * Los componentes 'pixio_transform_t' y 'pixio_entity_t' se tratan como no removibles.
 *
 * Si la entidad no está habilitada o no tiene componentes, la función retorna sin hacer cambios.
 *
 * Since: 1.0
 */
void gapp_inspector_set_entity(GappInspector *self, ecs_world_t *world, ecs_entity_t entity)
{
    gtk_list_box_remove_all(self->listbox);
    self->size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

    // Component fake default
    gapp_inspector_component_default(self, world, entity);

    if (!pixio_get_enabled(world, entity))
        return;

    const ecs_type_t *type = ecs_get_type(world, entity);

    // TODO: add widget based on type
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

        GtkWidget *expander = gapp_inspector_group_new(self->listbox, component_name, is_removable);
        g_object_set_data(G_OBJECT(expander), "size-group", self->size_group);
        gapp_inspector_component_add_props_widget(expander, world, component_ptr, e_component);
    }
}
