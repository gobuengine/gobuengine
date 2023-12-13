#include "gapp_script_editor.h"
#include "config.h"
#include "gapp_main.h"
#include "gapp_widget.h"

struct _GobuEditorScript
{
    GtkWidget parent;
    GtkWidget* buffer, * view_source, * console;
    gchar* filename;
    binn* data;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuEditorScript, gapp_editor_script, GTK_TYPE_BOX);

static void gapp_editor_script_class_init(GobuEditorScriptClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gapp_editor_script_init(GobuEditorScript* self)
{

}

//  -------------------------

extern GAPP* EditorCore;

/**
 * Maneja la señal de construcción de un script en Gobu.
 *
 * @param widget  El widget que desencadenó la señal de construcción.
 * @param data    Datos relacionados con la señal de construcción del script.
 */
static void gb_script_signal_build(GtkWidget* widget, GobuEditorScript* self)
{
    // gb_script_view_console_print("Build initialize...");
    // gb_script_reset();

    // int id_script = 0;
    // while (true) {
    //     gchar* key = gtk_string_list_get_string(module.cmp_list, id_script);
    //     if (key == NULL)break;
    //     gchar* script_buffer = g_hash_table_lookup(module.cmp_table, key);
    //     bool result = gb_script_string_load(script_buffer);
    //     id_script++;

    //     gchar* result_str = (result) ? MSG_OK : g_strdup_printf(MSG_ERROR, gb_script_traceback());
    //     gb_script_view_console_print(g_strdup_printf("Compile > <b>%s</b> %s", key, result_str));
    // }

    // gb_script_reset();

    gb_print_info(TF("Build gbscript [%s]", self->filename));
}

/**
 * Maneja la señal de guardado de un script en Gobu.
 *
 * @param widget  El widget que desencadenó la señal de guardado.
 * @param data    Datos relacionados con la señal de guardado del script.
 */
static void gb_script_signal_save(GtkWidget* widget, GobuEditorScript* self)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(self->buffer, &start, &end);
    gchar* buffer = gtk_text_buffer_get_text(self->buffer, &start, &end, TRUE);

    gb_fs_write_file(self->filename, buffer);

    // gb_script_view_console_print("Save Scripts");
    gb_print_success(TF("Save gbscript [%s]", self->filename));
}


/**
 * Configura un componente de señal en Gobu para una fábrica de elementos de lista.
 *
 * @param self       La fábrica de elementos de lista en la que se configura el componente de señal.
 * @param listitem   El elemento de lista al que se le aplica la configuración.
 * @param user_data  Datos de usuario opcionales para la configuración del componente.
 */
// static void gb_script_list_component_signal_setup(GtkSignalListItemFactory* self, GtkListItem* listitem, gpointer user_data)
// {
//     GtkWidget* box, * icon, * label, * button;

//     box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
//     gtk_list_item_set_child(listitem, box);

//     icon = gtk_image_new_from_icon_name("application-x-addon-symbolic");
//     gtk_box_append(box, icon);

//     label = gtk_label_new(NULL);
//     gtk_widget_set_hexpand(label, TRUE);
//     gtk_widget_set_halign(label, GTK_ALIGN_START);
//     gtk_box_append(box, label);

//     button = gtk_button_new_from_icon_name("user-trash-full-symbolic");
//     gtk_button_set_has_frame(button, FALSE);
//     gtk_widget_set_halign(button, GTK_ALIGN_END);
//     gapp_widget_set_margin(button, 5);
//     gtk_box_append(box, button);
//     g_signal_connect(button, "clicked", G_CALLBACK(gb_script_list_component_signal_remove), NULL);
// }

/**
 * Vincula un componente de señal en Gobu a un elemento de lista en una fábrica de elementos de lista.
 *
 * @param self       La fábrica de elementos de lista en la que se realiza la vinculación.
 * @param listitem   El elemento de lista al que se vincula el componente de señal.
 * @param user_data  Datos de usuario opcionales para la vinculación del componente.
 */
// static void gb_script_list_component_signal_bind(GtkSignalListItemFactory* self, GtkListItem* listitem, gpointer user_data)
// {
//     GtkWidget* box = gtk_list_item_get_child(listitem);
//     GtkStringObject* strobj = gtk_list_item_get_item(listitem);

//     GtkWidget* icon = gtk_widget_get_first_child(box);
//     GtkWidget* label = gtk_widget_get_next_sibling(icon);

//     gtk_label_set_text(GTK_LABEL(label), gtk_string_object_get_string(strobj));
// }

/**
 * Obtiene una lista de componentes de un script en Gobu.
 *
 * @return Un widget que representa la lista de componentes del script.
 */
// static GtkWidget* gb_script_list_component(void)
// {
//     GtkWidget* expand, * vbox_content;
//     GtkWidget* view, * scroll, * button;
//     GtkListItemFactory* factory;

//     scroll = gtk_scrolled_window_new();
//     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
//     gtk_widget_set_vexpand(scroll, TRUE);
//     gtk_widget_set_size_request(GTK_WIDGET(scroll), DOCK_L_MIN_SIZE, -1);
//     {
//         button = gtk_button_new_from_icon_name("list-add-symbolic");
//         expand = gapp_widget_expander_with_widget("<b>Components</b>", button);
//         gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), expand);
//         {
//             module.selected = gtk_single_selection_new(G_LIST_MODEL(module.cmp_list));
//             gtk_single_selection_set_autoselect(module.selected, FALSE);
//             gtk_single_selection_set_can_unselect(module.selected, TRUE);
//             gtk_single_selection_set_selected(module.selected, GTK_INVALID_LIST_POSITION);

//             factory = gtk_signal_list_item_factory_new();

//             view = gtk_list_view_new(module.selected, factory);
//             gtk_list_view_set_show_separators(view, TRUE);
//             gtk_widget_add_css_class(view, "navigation-sidebar");
//             gtk_expander_set_child(expand, view);

//             g_signal_connect(factory, "setup", G_CALLBACK(gb_script_list_component_signal_setup), NULL);
//             g_signal_connect(factory, "bind", G_CALLBACK(gb_script_list_component_signal_bind), NULL);
//             g_signal_connect(module.selected, "notify::selected", G_CALLBACK(gb_script_list_component_signal_selected), NULL);
//             g_signal_connect(button, "clicked", G_CALLBACK(gb_script_list_component_signal_create), NULL);
//         }
//     }

//     return scroll;
// }

/**
 * Inicializa un script en Gobu.
 *
 * @return Un nuevo widget que representa el script inicializado.
 */
static GobuEditorScript* gapp_editor_script_template(GobuEditorScript* self)
{
    GtkWidget* vbox, * toolbar, * item;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_box_append(self, vbox);
    {
        toolbar = gapp_widget_toolbar_new();
        gtk_box_append(vbox, toolbar);
        {
            item = gapp_widget_button_new_icon_with_label("media-removable-symbolic", "Save");
            gtk_box_append(toolbar, item);
            g_signal_connect(item, "clicked", G_CALLBACK(gb_script_signal_save), self);

            gapp_widget_toolbar_separator_new(toolbar);

            item = gapp_widget_button_new_icon_with_label("media-view-subtitles-symbolic", "Build source");
            gtk_box_append(toolbar, item);
            g_signal_connect(item, "clicked", G_CALLBACK(gb_script_signal_build), self);
        }

        gtk_box_append(vbox, gapp_widget_separator_h());

        {
            GtkWidget* view_source_scroll;

            view_source_scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(view_source_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
            gtk_widget_set_vexpand(view_source_scroll, TRUE);
            gtk_box_append(vbox, view_source_scroll);
            // gtk_widget_set_size_request(GTK_WIDGET(view_source_scroll), -1, -1);
            {
                self->view_source = gtk_source_view_new();
                gtk_source_view_set_show_line_numbers(self->view_source, TRUE);
                gtk_source_view_set_highlight_current_line(self->view_source, TRUE);
                gtk_source_view_set_show_line_marks(self->view_source, TRUE);
                gtk_source_view_set_enable_snippets(self->view_source, TRUE);
                gtk_source_view_set_auto_indent(self->view_source, TRUE);
                gtk_source_view_set_indent_on_tab(self->view_source, TRUE);
                gtk_text_view_set_left_margin(GTK_TEXT_VIEW(self->view_source), 10);
                gtk_text_view_set_top_margin(GTK_TEXT_VIEW(self->view_source), 10);
                // gtk_widget_set_sensitive(self->view_source, FALSE);

                self->buffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->view_source)));
                gtk_source_buffer_set_style_scheme(self->buffer, EditorCore->setting.scheme);
                gtk_source_buffer_set_language(self->buffer, EditorCore->setting.language);
                gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(view_source_scroll), self->view_source);
                // g_signal_connect(self->buffer, "changed", G_CALLBACK(gb_script_list_component_signal_save_virtual), NULL);

                int size = 0;
                unsigned char* buffer = gb_fs_get_contents(self->filename, &size);
                gtk_text_buffer_set_text(self->buffer, buffer, size);
                free(buffer);
            }
        }
    }

    return self;
}

/**
 * Crea un nuevo editor de scripts en Gobu.
 *
 * @return Un nuevo widget que representa el editor de scripts.
 */
GobuEditorScript* gapp_editor_script_new(const gchar* filename)
{
    GobuEditorScript* self = g_object_new(GAPP_EDITOR_TYPE_SCRIPT, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    self->filename = gb_strdup(filename);
    char* name = gb_fs_get_name(filename, false);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 0, name, gapp_editor_script_template(self));

    return self;
}

