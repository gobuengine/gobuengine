#include "gapp_inspector_entity.h"
#include "gapp_common.h"
#include "gapp_widget.h"

#include "pixio/pixio_type.h"

// - - - - - - - - - - - - - -
// MARK:BASE CLASS
// - - - - - - - - - - - - - -

struct _GappInspectorEntity
{
    GtkBox parent_instance;
    GtkWidget *size_group;
    GtkWidget *listbox;
};

static void gapp_inspector_entity_dispose(GObject *object);

G_DEFINE_TYPE(GappInspectorEntity, gapp_inspector_entity, GTK_TYPE_BOX)

static void gapp_inspector_entity_class_init(GappInspectorEntityClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gapp_inspector_entity_dispose;
}

static void gapp_inspector_entity_dispose(GObject *object)
{
    GappInspectorEntity *self = GAPP_INSPECTOR_ENTITY(object);

    G_OBJECT_CLASS(gapp_inspector_entity_parent_class)->dispose(object);
}

static void gapp_inspector_entity_init(GappInspectorEntity *self)
{
    self->listbox = gtk_list_box_new();
    gtk_widget_add_css_class(self->listbox, "inspector_list");
    gtk_list_box_set_selection_mode(self->listbox, GTK_SELECTION_NONE);
    gtk_box_append(GTK_BOX(self), self->listbox);
}

// - - - - - - - - - - - - - -
// MARK:WIDGETS
// - - - - - - - - - - - - - -

// ---------------------------
// WIDGET PROPERTY INPUT TEXT
// ---------------------------
static void gapp_inspector_entity_s_entry_changed(GtkEditable *self, ecs_string_t **field)
{
    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(self));
    *field = ecs_os_strdup(text);
}

static GtkWidget *gapp_inspector_entity_widget_string(ecs_meta_cursor_t cursor)
{
    ecs_string_t **field = (ecs_string_t **)ecs_meta_get_ptr(&cursor);

    GtkWidget *entry = gtk_entry_new();
    gapp_widget_entry_set_text(GTK_ENTRY(entry), ecs_meta_get_string(&cursor));
    g_signal_connect(entry, "changed", G_CALLBACK(gapp_inspector_entity_s_entry_changed), field);

    return entry;
}

// ---------------------------
// WIDGET PROPERTY INPUT NUMBER
// ---------------------------
static void gapp_inspector_entity_s_spin_button_changed(GtkSpinButton *self, ecs_u32_t *field)
{
    *field = gtk_spin_button_get_value(self);
}

static GtkWidget *gapp_inspector_entity_widget_number(double min, double max, double step, ecs_meta_cursor_t cursor)
{
    ecs_u32_t *field = (ecs_u32_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, step);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), *field);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(gapp_inspector_entity_s_spin_button_changed), field);

    return number_spin;
}

static void gapp_inspector_entity_s_spin_button_changed_f64_t(GtkSpinButton *self, ecs_f64_t *field)
{
    *field = (ecs_f64_t)gtk_spin_button_get_value(self);
}

static GtkWidget *gapp_inspector_entity_widget_number_f64_t(double min, double max, double step, ecs_meta_cursor_t cursor)
{
    ecs_f64_t *field = (ecs_f64_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, step);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), *field);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(gapp_inspector_entity_s_spin_button_changed_f64_t), field);

    return number_spin;
}

static void gapp_inspector_entity_s_spin_button_changed_f32_t(GtkSpinButton *self, ecs_f32_t *field)
{
    *field = (ecs_f32_t)gtk_spin_button_get_value(self);
}

static GtkWidget *gapp_inspector_entity_widget_number_f32_t(double min, double max, double step, ecs_meta_cursor_t cursor)
{
    ecs_f32_t *field = (ecs_f32_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, step);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), *field);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(gapp_inspector_entity_s_spin_button_changed_f32_t), field);

    return number_spin;
}

// ---------------------------
// WIDGET PROPERTY INPUT COLOR
// ---------------------------
static void gapp_inspector_entity_s_color_changed(GtkColorDialogButton *self, GParamSpec *ignored, pixio_color_t *field)
{
    const GdkRGBA *color = gtk_color_dialog_button_get_rgba(self);
    field->r = (guint8)(color->red * 255);
    field->g = (guint8)(color->green * 255);
    field->b = (guint8)(color->blue * 255);
    field->a = (guint8)(color->alpha * 255);
}

static GtkWidget *gapp_inspector_entity_widget_color(ecs_meta_cursor_t cursor)
{
    pixio_color_t *field = (pixio_color_t *)ecs_meta_get_ptr(&cursor);
    GdkRGBA color;
    color.red = (gdouble)field->r / 255;
    color.green = (gdouble)field->g / 255;
    color.blue = (gdouble)field->b / 255;
    color.alpha = (gdouble)field->a / 255;

    GtkWidget *color_button = gtk_color_dialog_button_new(gtk_color_dialog_new());
    gtk_color_dialog_button_set_rgba(GTK_COLOR_DIALOG_BUTTON(color_button), &color);

    g_signal_connect(color_button, "notify::rgba", G_CALLBACK(gapp_inspector_entity_s_color_changed), field);

    return color_button;
}

// ---------------------------
// WIDGET PROPERTY INPUT VECT2
// ---------------------------
static void gapp_inspector_entity_s_vect2x(GtkSpinButton *self, pixio_vector2_t *field)
{
    field->x = gtk_spin_button_get_value(self);
}

static void gapp_inspector_entity_s_vect2y(GtkSpinButton *self, pixio_vector2_t *field)
{
    field->y = gtk_spin_button_get_value(self);
}

static GtkWidget *gapp_inspector_entity_widget_vector2(ecs_meta_cursor_t cursor)
{
    pixio_vector2_t *field = (pixio_vector2_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

    GtkWidget *number_spinx = gtk_spin_button_new_with_range(INTMAX_MIN, INTMAX_MAX, 0.5);
    gtk_widget_set_valign(number_spinx, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spinx, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spinx), field->x);
    gtk_widget_set_tooltip_text(number_spinx, "X");
    // gtk_widget_add_css_class(number_spinx, "vx");
    gtk_box_append(box, number_spinx);

    GtkWidget *number_spiny = gtk_spin_button_new_with_range(INTMAX_MIN, INTMAX_MAX, 0.5);
    gtk_widget_set_valign(number_spiny, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spiny, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spiny), field->y);
    gtk_widget_set_tooltip_text(number_spiny, "Y");
    // gtk_widget_add_css_class(number_spiny, "vy");
    gtk_box_append(box, number_spiny);

    g_signal_connect(number_spinx, "value-changed", G_CALLBACK(gapp_inspector_entity_s_vect2x), field);
    g_signal_connect(number_spiny, "value-changed", G_CALLBACK(gapp_inspector_entity_s_vect2y), field);

    return box;
}

// ---------------------------
// WIDGET PROPERTY INPUT RESOURCE
// ---------------------------
static void gapp_inspector_entity_s_resource_changed(GtkColorDialogButton *self, GParamSpec *ignored, pixio_color_t *field)
{
}

static GtkWidget *gapp_inspector_entity_widget_resource(ecs_entity_t field_type, ecs_meta_cursor_t cursor)
{
    pixio_color_t *field = (pixio_color_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

    GtkWidget *dropdown = gtk_drop_down_new_from_strings(NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dropdown), TRUE);
    gtk_box_append(box, dropdown);

    // g_signal_connect(color_button, "notify::rgba", G_CALLBACK(gapp_inspector_entity_s_resource_changed), field);

    return box;
}

// ---------------------------
// WIDGET PROPERTY INPUT ENUM
// ---------------------------
static void gapp_project_config_s_enums_activate(GtkDropDown *self, GParamSpec *pspec, gchar *id_property)
{
    guint value = gtk_drop_down_get_selected(self);
}

static GtkWidget *gapp_inspector_entity_widget_enums(ecs_meta_cursor_t cursor)
{
    const char *const *strings[] = {"None", "Linear", "Nearest", NULL};

    pixio_texture_filter_t *field = (pixio_texture_filter_t *)ecs_meta_get_ptr(&cursor);

    printf("ENUM: %d\n", *field);

    GtkWidget *select_option = gtk_drop_down_new_from_strings(strings);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(select_option), *field);

    g_signal_connect(select_option, "notify::selected", G_CALLBACK(gapp_project_config_s_enums_activate), field);

    return select_option;
}

// ---------------------------
// WIDGET PROPERTY GROUP CHILD
// ---------------------------

static GtkWidget *gapp_inspector_entity_group_child_new(GtkWidget *size_group, const char *label_str, GtkWidget *input, GtkOrientation orientation)
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

static GtkWidget *gapp_inspector_entity_group_new(GtkWidget *list, const gchar *title_str)
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
            // // g_signal_connect(button_off, "clicked", G_CALLBACK(gapp_inspector_entity_component_remove_clicked), entity);

            GtkWidget *button = gtk_button_new_from_icon_name("user-trash-symbolic");
            gtk_widget_add_css_class(button, "expander_button");
            gtk_box_append(GTK_BOX(title), button);
            gtk_widget_set_margin_start(button, 0);
            gtk_widget_set_tooltip_text(button, "Remove component");
            // g_signal_connect(button, "clicked", G_CALLBACK(gapp_inspector_entity_component_remove_clicked), entity);
        }
    }

    // content expander
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gapp_widget_set_margin(content, 4);
    gtk_expander_set_child(GTK_EXPANDER(expander), content);

    return content;
}

// ---------------------------
// WIDGET PROPERTY INPUT TEXT
// ---------------------------

static void gapp_inspector_entity_component_add_props_widget(GtkWidget *content, ecs_world_t *world, void *ptr, ecs_entity_t component)
{
    ecs_meta_cursor_t cursor = ecs_meta_cursor(world, component, ptr);
    ecs_meta_push(&cursor);
    for (int i = 0; i < ECS_MEMBER_DESC_CACHE_SIZE; i++)
    {
        const char *field_name = ecs_meta_get_member(&cursor);
        if (field_name == NULL)
            break;

        ecs_entity_t field_type = ecs_meta_get_type(&cursor);
        {
            GtkWidget *input = NULL;

            // TODO: add widget based on type
            if (field_type == ecs_id(ecs_string_t))
            {
                input = gapp_inspector_entity_widget_string(cursor);
            }
            else if (field_type == ecs_id(ecs_bool_t))
            {
                // Do nothing
            }
            else if (field_type == ecs_id(ecs_u32_t))
            {
                input = gapp_inspector_entity_widget_number(0, UINTMAX_MAX, 1, cursor);
            }
            else if (field_type == ecs_id(ecs_f64_t))
            {
                input = gapp_inspector_entity_widget_number_f64_t(INTMAX_MIN, INTMAX_MAX, 1.0, cursor);
            }
            else if (field_type == ecs_id(ecs_f32_t))
            {
                input = gapp_inspector_entity_widget_number_f32_t(INTMAX_MIN, INTMAX_MAX, 0.1, cursor);
            }
            else if (field_type == ecs_id(pixio_vector2_t))
            {
                input = gapp_inspector_entity_widget_vector2(cursor);
            }
            else if (field_type == ecs_id(pixio_color_t))
            {
                input = gapp_inspector_entity_widget_color(cursor);
            }
            else if (field_type == ecs_id(pixio_resource_font_t))
            {
                input = gapp_inspector_entity_widget_resource(ecs_id(pixio_resource_font_t), cursor);
            }
            else if (field_type == ecs_id(pixio_resource_texture_t))
            {
                input = gapp_inspector_entity_widget_resource(ecs_id(pixio_resource_texture_t), cursor);
            }
            else if (field_type == ecs_id(pixio_texture_filter_t))
            {
                input = gapp_inspector_entity_widget_enums(cursor);
            }

            if (input)
            {
                GtkWidget *size_group = g_object_get_data(G_OBJECT(content), "size-group");
                GtkWidget *child = gapp_inspector_entity_group_child_new(size_group, field_name, input, GTK_ORIENTATION_HORIZONTAL);
                gtk_box_append(GTK_BOX(content), child);
            }
        }
        ecs_meta_next(&cursor);
    }
    ecs_meta_pop(&cursor);
}

// - - - - - - - - - - - - - -
// MARK:API
// - - - - - - - - - - - - - -

GappInspectorEntity *gapp_inspector_entity_new(void)
{
    return g_object_new(GAPP_TYPE_INSPECTOR_ENTITY, "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 0, NULL);
}

void gapp_inspector_entity_set_entity(GappInspectorEntity *self, ecs_world_t *world, ecs_entity_t entity)
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
            GtkWidget *expander = gapp_inspector_entity_group_new(self->listbox, component_name);
            g_object_set_data(G_OBJECT(expander), "size-group", self->size_group);
            gapp_inspector_entity_component_add_props_widget(expander, world, component_ptr, component);
        }
    }
}
