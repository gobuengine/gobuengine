#include "gapp_inspector_widgets.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "types/type_enum.h"

/**
 * @brief Convierte un color de pixio_color_t a GdkRGBA.
 *
 * @param pixio_color El color en formato pixio_color_t.
 * @return GdkRGBA El color convertido a formato GdkRGBA.
 */
static GdkRGBA pixio_color_to_gdk_rgba(const pixio_color_t *pixio_color)
{
    return (GdkRGBA){
        .red = (gdouble)pixio_color->r / 255.0,
        .green = (gdouble)pixio_color->g / 255.0,
        .blue = (gdouble)pixio_color->b / 255.0,
        .alpha = (gdouble)pixio_color->a / 255.0};
}

/**
 * @brief Convierte un color de GdkRGBA a pixio_color_t.
 *
 * @param gdk_color El color en formato GdkRGBA.
 * @return pixio_color_t El color convertido a formato pixio_color_t.
 */
static pixio_color_t gdk_rgba_to_pixio_color(const GdkRGBA *gdk_color)
{
    return (pixio_color_t){
        .r = (uint8_t)(gdk_color->red * 255.0),
        .g = (uint8_t)(gdk_color->green * 255.0),
        .b = (uint8_t)(gdk_color->blue * 255.0),
        .a = (uint8_t)(gdk_color->alpha * 255.0)};
}

/**
 * @brief Maneja el cambio en un widget GtkEditable para un campo de cadena.
 *
 * Esta función se llama cuando el texto en un GtkEditable cambia. Actualiza
 * el campo de cadena ECS correspondiente con el nuevo texto.
 *
 * @param self El widget GtkEditable que cambió.
 * @param field Puntero al campo de cadena ECS que se actualizará.
 */
static void signal_input_string(GtkEditable *self, ecs_string_t **field)
{
    const gchar *text = gtk_editable_get_text(self);
    *field = ecs_os_strdup(text);
}

/**
 * Manejador de señal para actualizar un campo booleano desde un GtkCheckButton.
 *
 * Esta función se conecta a la señal "toggled" de un GtkCheckButton y actualiza
 * un campo booleano (ecs_bool_t) con el estado actual del botón.
 *
 * @param self El GtkCheckButton que emitió la señal.
 * @param field Puntero al campo ecs_bool_t que se actualizará.
 */
static void signal_input_bool(GtkCheckButton *self, ecs_bool_t *field)
{
    g_return_if_fail(GTK_IS_CHECK_BUTTON(self));
    g_return_if_fail(field != NULL);

    *field = gtk_check_button_get_active(GTK_CHECK_BUTTON(self));
}

/**
 * @brief Maneja el cambio de valor en un GtkSpinButton para un campo u32.
 *
 * Esta función se llama cuando el valor de un GtkSpinButton cambia. Actualiza
 * el campo u32 de ECS correspondiente con el nuevo valor.
 *
 * @param self El GtkSpinButton que cambió.
 * @param field Puntero al campo u32 de ECS que se actualizará.
 */
static void signal_input_u32(GtkSpinButton *self, ecs_u32_t *field)
{
    *field = (ecs_u32_t)gtk_spin_button_get_value(self);
}

/**
 * @brief Maneja el cambio de valor en un GtkSpinButton para un campo f64.
 *
 * Esta función se llama cuando el valor de un GtkSpinButton cambia. Actualiza
 * el campo f64 de ECS correspondiente con el nuevo valor.
 *
 * @param self El GtkSpinButton que cambió.
 * @param field Puntero al campo f64 de ECS que se actualizará.
 */
static void signal_input_f64(GtkSpinButton *self, ecs_f64_t *field)
{
    *field = gtk_spin_button_get_value(self);
}

/**
 * Signal handler for updating a float (f32) field when a GtkSpinButton value changes.
 *
 * This function is designed to be connected as a callback to a GtkSpinButton's
 * value-changed signal. It updates the float value pointed to by 'field' with
 * the current value of the spin button.
 *
 * @param self The GtkSpinButton that triggered the signal.
 * @param field Pointer to the float (ecs_f32_t) field to be updated.
 */
static void signal_input_f32(GtkSpinButton *self, ecs_f32_t *field)
{
    *field = gtk_spin_button_get_value(self);
}

/**
 * @brief Maneja el cambio de color en un GtkColorDialogButton.
 *
 * Esta función se llama cuando el color seleccionado en un GtkColorDialogButton cambia.
 * Actualiza el campo pixio_color_t de ECS correspondiente con el nuevo color.
 *
 * @param self El GtkColorDialogButton que cambió.
 * @param ignored El GParamSpec del color (no utilizado en esta función).
 * @param field Puntero al campo pixio_color_t de ECS que se actualizará.
 */
static void signal_input_color(GtkColorDialogButton *self, GParamSpec *ignored, pixio_color_t *field)
{
    const GdkRGBA *color = gtk_color_dialog_button_get_rgba(self);
    *field = gdk_rgba_to_pixio_color(color);
}

/**
 * @brief Maneja el cambio de valor en el componente X de un vector 2D.
 *
 * @param self El GtkSpinButton que cambió.
 * @param field Puntero al campo pixio_vector2_t de ECS que se actualizará.
 */
static void signal_input_vect2_x(GtkSpinButton *self, pixio_vector2_t *field)
{
    field->x = (float)gtk_spin_button_get_value(self);
}

/**
 * @brief Maneja el cambio de valor en el componente Y de un vector 2D.
 *
 * @param self El GtkSpinButton que cambió.
 * @param field Puntero al campo pixio_vector2_t de ECS que se actualizará.
 */
static void signal_input_vect2_y(GtkSpinButton *self, pixio_vector2_t *field)
{
    field->y = (float)gtk_spin_button_get_value(self);
}

static void signal_input_enum(GtkWidget *self, GParamSpec *pspec, void *field_ptr)
{
    GObject *item = gtk_drop_down_get_selected_item(self);
    *(uint8_t *)field_ptr = object_ienum_get_value(item);
}

/**
 * @brief Crea un widget de entrada para un campo de cadena.
 *
 * Esta función crea un widget GtkEntry para un campo de cadena ECS,
 * inicializa su valor y conecta la señal "changed" a la función de manejo.
 *
 * @param cursor El cursor de metadatos ECS para el campo de cadena.
 * @return GtkWidget* El widget de entrada creado.
 */
static GtkWidget *inspectorWidgetCreate_StringInput(ecs_meta_cursor_t cursor)
{
    ecs_string_t **field = (ecs_string_t **)ecs_meta_get_ptr(&cursor);

    GtkWidget *entry = gtk_entry_new();
    const char *current_text = ecs_meta_get_string(&cursor);
    gtk_editable_set_text(GTK_EDITABLE(entry), current_text ? current_text : "");

    g_signal_connect(entry, "changed", G_CALLBACK(signal_input_string), field);

    return entry;
}

/**
 * Crea un widget de entrada booleana para el inspector.
 *
 * Esta función crea un GtkCheckButton basado en un cursor de metadatos ECS.
 * El estado del botón se inicializa con el valor actual del campo booleano,
 * y se conecta una señal para actualizar el campo cuando el estado del botón cambie.
 *
 * @param cursor Cursor de metadatos ECS que apunta al campo booleano.
 * @return GtkWidget* Un nuevo GtkCheckButton configurado y conectado.
 */
static GtkWidget *inspectorWidgetCreate_BoolInput(ecs_meta_cursor_t cursor)
{
    ecs_bool_t *field = (ecs_bool_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *check = gtk_check_button_new();
    gtk_widget_set_valign(check, GTK_ALIGN_CENTER);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(check), (ecs_bool_t)*field);

    g_signal_connect(check, "toggled", G_CALLBACK(signal_input_bool), field);

    return check;
}

/**
 * @brief Crea un widget de entrada para un campo u32.
 *
 * Esta función crea un widget GtkSpinButton para un campo u32 de ECS,
 * inicializa su valor y conecta la señal "value-changed" a la función de manejo.
 *
 * @param cursor El cursor de metadatos ECS para el campo u32.
 * @return GtkWidget* El widget GtkSpinButton creado.
 */
static GtkWidget *inspectorWidgetCreate_NumberU32Input(ecs_meta_cursor_t cursor, ecs_member_t *member)
{
    ecs_u32_t *field = (ecs_u32_t *)ecs_meta_get_ptr(&cursor);

    double min = member->range.max == 0.000000 && member->range.min == 0.000000 ? 0 : member->range.min;
    double max = member->range.max == 0.000000 && member->range.min == 0.000000 ? UINT32_MAX : member->range.max;

    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, 1.0);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), (gdouble)*field);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(signal_input_u32), field);

    return number_spin;
}

/**
 * @brief Crea un widget de entrada para un campo f64.
 *
 * Esta función crea un widget GtkSpinButton para un campo f64 de ECS,
 * inicializa su valor y conecta la señal "value-changed" a la función de manejo.
 *
 * @param cursor El cursor de metadatos ECS para el campo f64.
 * @return GtkWidget* El widget GtkSpinButton creado.
 */
static GtkWidget *inspectorWidgetCreate_NumberF64Input(ecs_meta_cursor_t cursor, ecs_member_t *member)
{
    ecs_f64_t *field = (ecs_f64_t *)ecs_meta_get_ptr(&cursor);

    double min = member->range.max == 0.000000 && member->range.min == 0.000000 ? INTMAX_MIN : member->range.min;
    double max = member->range.max == 0.000000 && member->range.min == 0.000000 ? INTMAX_MAX : member->range.max;

    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, 0.1);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), *field);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(signal_input_f64), field);

    return number_spin;
}

/**
 * Creates a spin button widget for inputting float (f32) values in the inspector.
 *
 * This function generates a GtkSpinButton widget for editing a float value.
 * It sets up the spin button with a wide range, fine-grained step size,
 * and high precision. The current value is set based on the field pointed
 * to by the ECS meta cursor, and a signal handler is connected to update
 * the field when the value changes.
 *
 * @param cursor ECS meta cursor pointing to an ecs_f32_t field.
 * @return A GtkWidget (GtkSpinButton) for inputting float values.
 */
static GtkWidget *inspectorWidgetCreate_NumberF32Input(ecs_meta_cursor_t cursor, ecs_member_t *member)
{
    ecs_f32_t *field = (ecs_f32_t *)ecs_meta_get_ptr(&cursor);
    ecs_entity_t field_type = ecs_meta_get_type(&cursor);

    double min = member->range.max == 0.000000 && member->range.min == 0.000000 ? INTMAX_MIN : member->range.min;
    double max = member->range.max == 0.000000 && member->range.min == 0.000000 ? INTMAX_MAX : member->range.max;

    GtkWidget *number_spin = gtk_spin_button_new_with_range(min, max, 0.1);
    gtk_widget_set_valign(number_spin, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spin, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spin), *field);

    g_signal_connect(number_spin, "value-changed", G_CALLBACK(signal_input_f32), field);

    return number_spin;
}

/**
 * @brief Crea un widget de entrada para un campo de color.
 *
 * Esta función crea un widget GtkColorDialogButton para un campo pixio_color_t de ECS,
 * inicializa su valor y conecta la señal "notify::rgba" a la función de manejo.
 *
 * @param cursor El cursor de metadatos ECS para el campo de color.
 * @return GtkWidget* El widget GtkColorDialogButton creado.
 */
static GtkWidget *inspectorWidgetCreate_ColorInput(ecs_meta_cursor_t cursor)
{
    pixio_color_t *field = (pixio_color_t *)ecs_meta_get_ptr(&cursor);
    GdkRGBA color = pixio_color_to_gdk_rgba(field);

    GtkWidget *color_button = gtk_color_dialog_button_new(gtk_color_dialog_new());
    gtk_color_dialog_button_set_rgba(GTK_COLOR_DIALOG_BUTTON(color_button), &color);

    g_signal_connect(color_button, "notify::rgba", G_CALLBACK(signal_input_color), field);

    return color_button;
}

/**
 * @brief Crea un widget de entrada para un campo de vector 2D.
 *
 * Esta función crea un widget compuesto por dos GtkSpinButton para un campo
 * pixio_vector2_t de ECS, inicializa sus valores y conecta las señales
 * "value-changed" a las funciones de manejo correspondientes.
 *
 * @param cursor El cursor de metadatos ECS para el campo de vector 2D.
 * @return GtkWidget* El widget compuesto creado.
 */
static GtkWidget *inspectorWidgetCreate_Vector2Input(ecs_meta_cursor_t cursor)
{
    pixio_vector2_t *field = (pixio_vector2_t *)ecs_meta_get_ptr(&cursor);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

    GtkWidget *number_spinx = gtk_spin_button_new_with_range(INTMAX_MIN, INTMAX_MAX, 0.1);
    gtk_widget_set_valign(number_spinx, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spinx, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spinx), field->x);
    gtk_widget_set_tooltip_text(number_spinx, "X");
    gtk_box_append(GTK_BOX(box), number_spinx);

    GtkWidget *number_spiny = gtk_spin_button_new_with_range(INTMAX_MIN, INTMAX_MAX, 0.1);
    gtk_widget_set_valign(number_spiny, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(number_spiny, TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(number_spiny), field->y);
    gtk_widget_set_tooltip_text(number_spiny, "Y");
    gtk_box_append(GTK_BOX(box), number_spiny);

    g_signal_connect(number_spinx, "value-changed", G_CALLBACK(signal_input_vect2_x), field);
    g_signal_connect(number_spiny, "value-changed", G_CALLBACK(signal_input_vect2_y), field);

    return box;
}

/**
 * Creates a dropdown widget for selecting texture filter options in the inspector.
 *
 * This function generates a GtkDropDown widget populated with texture filter options.
 * It sets the currently selected option based on the value pointed to by the ECS meta cursor
 * and connects a signal handler to update the field when a new option is selected.
 *
 * @param cursor ECS meta cursor pointing to a pixio_texture_filter_t field.
 * @return A GtkWidget (GtkDropDown) for selecting texture filter options.
 */

static void _input_enum_factory_setup(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    GtkWidget *label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), label);
}

static void _input_enum_factory_bind(GtkSignalListItemFactory *factory, GtkListItem *listitem, gpointer data)
{
    ObjectIEnum *ienum = gtk_list_item_get_item(listitem);

    GtkWidget *label = gtk_list_item_get_child(listitem);
    gtk_label_set_label(GTK_LABEL(label), object_ienum_get_name(ienum));
}

static gint object_ienum_compare_func(ObjectIEnum *ienum_a, ObjectIEnum *ienum_b, gpointer user_data)
{
    gint value_a = object_ienum_get_value(ienum_a);
    gint value_b = object_ienum_get_value(ienum_b);

    if (value_a < value_b)
        return -1;
    else if (value_a > value_b)
        return 1;
    else
        return 0;
}

static GtkWidget *inspectorWidgetCreate_EnumInput(ecs_meta_cursor_t cursor)
{
    void *field_ptr = ecs_meta_get_ptr(&cursor);
    ecs_entity_t field_type = ecs_meta_get_type(&cursor);
    const EcsEnum *enum_type = ecs_get(cursor.world, field_type, EcsEnum);

    uint8_t position = (uint8_t)ecs_meta_get_int(&cursor);

    GListStore *store = g_list_store_new(OBJECT_TYPE_IENUM);

    ecs_map_iter_t it = ecs_map_iter(&enum_type->constants);
    while (ecs_map_next(&it))
    {
        ecs_enum_constant_t *constant = ecs_map_ptr(&it);
        g_list_store_append(store, object_ienum_new(constant->name, constant->value, 0));
    }
    g_list_store_sort(store, object_ienum_compare_func, NULL);

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(_input_enum_factory_setup), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(_input_enum_factory_bind), NULL);

    GtkWidget *select_option = gtk_drop_down_new(G_LIST_MODEL(store), NULL);
    gtk_drop_down_set_factory(GTK_DROP_DOWN(select_option), factory);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(select_option), position);
    g_signal_connect(select_option, "notify::selected", G_CALLBACK(signal_input_enum), field_ptr);

    return select_option;
}

/**
 * Crea un widget de entrada de recursos para el inspector.
 *
 * Esta función genera un widget de caja que contiene un menú desplegable (dropdown)
 * poblado con archivos de un directorio específico. El contenido del menú desplegable
 * se filtra según el filtro proporcionado.
 *
 * @param filter Filtro personalizado para aplicar a la lista de archivos.
 * @param cursor Cursor de metadatos ECS (no utilizado en esta implementación).
 * @return Un widget GtkBox que contiene el menú desplegable de recursos.
 */
static void setup_listitem(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(box, 6);
    gtk_widget_set_margin_end(box, 6);
    gtk_widget_set_margin_top(box, 6);
    gtk_widget_set_margin_bottom(box, 6);
    gtk_list_item_set_child(list_item, box);

    GtkWidget *icon = gtk_image_new();
    gtk_box_append(GTK_BOX(box), icon);

    GtkWidget *label = gtk_label_new(NULL);
    gtk_box_append(GTK_BOX(box), label);
}

static void bind_listitem(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    GFileInfo *fileinfo = gtk_list_item_get_item(list_item);
    GFile *file = G_FILE(g_file_info_get_attribute_object(fileinfo, "standard::file"));

    GtkWidget *box = gtk_list_item_get_child(list_item);

    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_last_child(box);

    // TODO: Una func para obtener el icono de los archivos...
    const char *ext_file = g_file_info_get_name(fileinfo);
    if (fsIsExtension(ext_file, ".png") || fsIsExtension(ext_file, ".jpg"))
        gtk_image_set_from_file(icon, g_file_get_path(file));
    else
        gtk_image_set_from_gicon(GTK_IMAGE(icon), g_file_info_get_icon(fileinfo));

    const char *name = g_file_info_get_display_name(fileinfo);
    gtk_label_set_text(GTK_LABEL(label), name);
}

static GtkWidget *gapp_inspector_widgets_input_resource(GtkCustomFilter *filter, ecs_meta_cursor_t cursor)
{
    const char *content_path = "C:/Users/hbibl/OneDrive/Documentos/Gobu Projects/Platformer/Data/Content/";
    GFile *content_dir = g_file_new_for_path(content_path);
    GtkDirectoryList *dir_list = gtk_directory_list_new("standard::*", content_dir);

    GtkFilterListModel *filter_model = gtk_filter_list_model_new(G_LIST_MODEL(dir_list), GTK_FILTER(filter));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_listitem), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_listitem), NULL);

    GtkWidget *dropdown = gtk_drop_down_new(G_LIST_MODEL(filter_model), NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dropdown), TRUE);
    // gtk_drop_down_set_expression(GTK_DROP_DOWN(dropdown), gtk_property_expression_new(G_TYPE_FILE_INFO, NULL, "name"));
    gtk_drop_down_set_factory(GTK_DROP_DOWN(dropdown), factory);
    gtk_box_append(GTK_BOX(box), dropdown);

    g_object_unref(content_dir);

    return box;
}

// Function to filter texture files
static gboolean filter_texture_func(GFileInfo *file, gpointer data)
{
    const char *name = g_file_info_get_name(file);
    return g_str_has_suffix(name, ".png") ||
           g_str_has_suffix(name, ".jpg") ||
           g_str_has_suffix(name, ".jpeg");
}

static GtkWidget *inspectorWidgetCreate_TextureInput(ecs_meta_cursor_t cursor)
{
    GtkCustomFilter *filter = gtk_custom_filter_new(filter_texture_func, NULL, NULL);
    GtkWidget *widget = gapp_inspector_widgets_input_resource(filter, cursor);
    // g_object_unref(filter);
    return widget;
}

// Function to filter font
static gboolean filter_font_func(GFileInfo *file, gpointer data)
{
    const char *name = g_file_info_get_name(file);
    return g_str_has_suffix(name, ".ttf");
}

static GtkWidget *inspectorWidgetCreate_FontInput(ecs_meta_cursor_t cursor)
{
    GtkCustomFilter *filter = gtk_custom_filter_new(filter_font_func, NULL, NULL);
    GtkWidget *widget = gapp_inspector_widgets_input_resource(filter, cursor);
    // g_object_unref(filter);
    return widget;
}

// Function to filter audio, musics
static gboolean filter_sounds_func(GFileInfo *file, gpointer data)
{
    const char *name = g_file_info_get_name(file);
    return g_str_has_suffix(name, ".mp3") ||
           g_str_has_suffix(name, ".wav");
}

static GtkWidget *gapp_inspector_widgets_input_sound(ecs_meta_cursor_t cursor)
{
    GtkCustomFilter *filter = gtk_custom_filter_new(filter_sounds_func, NULL, NULL);
    GtkWidget *widget = gapp_inspector_widgets_input_resource(filter, cursor);
    // g_object_unref(filter);
    return widget;
}

// Function to filter scene
static gboolean filter_scene_func(GFileInfo *file, gpointer data)
{
    const char *name = g_file_info_get_name(file);
    return g_str_has_suffix(name, ".gscene");
}

static GtkWidget *gapp_inspector_widgets_input_scene(ecs_meta_cursor_t cursor)
{
    GtkCustomFilter *filter = gtk_custom_filter_new(filter_scene_func, NULL, NULL);
    GtkWidget *widget = gapp_inspector_widgets_input_resource(filter, cursor);
    // g_object_unref(filter);
    return widget;
}

GtkWidget *inspectorWidgetCreateGroupChild(GtkWidget *size_group, const char *label_str, GtkWidget *input, GtkOrientation orientation)
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

GtkWidget *inspectorWidgetCreateGroup(GtkWidget *list, const gchar *title_str, bool buttonRemove)
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

void inspectorWidgetAddComponentToInspector(GtkWidget *content, ecs_world_t *world, void *ptr, ecs_entity_t component)
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
            {ecs_id(ecs_string_t), inspectorWidgetCreate_StringInput},
            {ecs_id(ecs_bool_t), inspectorWidgetCreate_BoolInput},
            {ecs_id(ecs_u32_t), inspectorWidgetCreate_NumberU32Input},
            {ecs_id(ecs_f64_t), inspectorWidgetCreate_NumberF64Input},
            {ecs_id(ecs_f32_t), inspectorWidgetCreate_NumberF32Input},
            {ecs_id(pixio_vector2_t), inspectorWidgetCreate_Vector2Input},
            {ecs_id(pixio_color_t), inspectorWidgetCreate_ColorInput},
            {ecs_id(pixio_resource_texture_t), inspectorWidgetCreate_TextureInput},
            {ecs_id(pixio_resource_font_t), inspectorWidgetCreate_FontInput},
            {ecs_id(pixio_texture_filter_t), inspectorWidgetCreate_EnumInput},
            {ecs_id(pixio_texture_flip_t), inspectorWidgetCreate_EnumInput},
            {ecs_id(pixio_origin_t), inspectorWidgetCreate_EnumInput},
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
            GtkWidget *child = inspectorWidgetCreateGroupChild(size_group, field_name, input, GTK_ORIENTATION_HORIZONTAL);
            gtk_box_append(GTK_BOX(content), child);
        }

        ecs_meta_next(&cursor);
    }
    ecs_meta_pop(&cursor);
}
