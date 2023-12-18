#include "gapp_widget.h"
#include "config.h"

/**
 * Inicializa el tema de widgets en Gobu.
 */
void gapp_widget_theme_init(gboolean dark_mode)
{
    if (dark_mode)
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "Content/theme/default.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

/**
 * Muestra una alerta de widget en Gobu.
 *
 * @param parent   El widget padre al que se asociará la alerta.
 * @param message  El mensaje de la alerta a mostrar.
 */
void gapp_widget_alert(GtkWidget* parent, const gchar* message)
{
    GtkAlertDialog* alert = gtk_alert_dialog_new(message);
    gtk_alert_dialog_set_modal(alert, TRUE);
    gtk_alert_dialog_show(alert, parent);
}

/**
 * Muestra un diálogo de confirmación de eliminación en Gobu.
 *
 * @param parent     El widget padre al que se asociará el diálogo.
 * @param name       El nombre del elemento que se desea eliminar.
 * @param data       Datos adicionales relacionados con la eliminación.
 * @param c_handler  El manejador de callback a invocar cuando se confirma la eliminación.
 */
void gapp_widget_dialog_confirm_delete(GtkWidget* parent, const gchar* name, gpointer* data, GCallback c_handler)
{
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Delete Assets", GTK_WINDOW(gtk_widget_get_root(parent)), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR, "Delete", GTK_RESPONSE_OK, NULL);
    gtk_window_set_resizable(dialog, FALSE);

    GtkWidget* box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gapp_widget_set_margin(GTK_WIDGET(box), 18);

    GtkWidget* label = gtk_label_new(g_strdup_printf("Esta seguro que desea eliminar <b>%s</b> ?", name));
    gtk_label_set_use_markup(label, TRUE);
    gtk_box_append(GTK_BOX(box), label);

    g_signal_connect(dialog, "response", G_CALLBACK(c_handler), data);

    gtk_window_present(dialog);
}

/**
 * Muestra un diálogo de entrada de texto en Gobu.
 *
 * @param parent         El widget padre al que se asociará el diálogo.
 * @param title          El título del diálogo.
 * @param text_default   El texto por defecto que se mostrará en el campo de entrada.
 *
 * @return Un widget que representa el diálogo de entrada de texto.
 */
GtkWidget* gapp_widget_dialog_input(GtkWidget* parent, const gchar* title, const gchar* text_default)
{
    GtkWidget* dialog = gtk_dialog_new_with_buttons(title, GTK_WINDOW(gtk_widget_get_root(parent)), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR, "Create", GTK_RESPONSE_OK, NULL);
    gtk_window_set_resizable(dialog, FALSE);

    GtkWidget* box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gapp_widget_set_margin(GTK_WIDGET(box), 18);

    GtkWidget* entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(text_default, -1));
    gtk_box_append(GTK_BOX(box), entry);
    g_object_set_data(dialog, "entry", entry);

    gtk_window_present(dialog);

    return dialog;
}

/**
 * Obtiene el texto ingresado en un diálogo de entrada de texto en Gobu.
 *
 * @param dialog  El widget que representa el diálogo de entrada de texto.
 *
 * @return El texto ingresado en el diálogo de entrada de texto.
 */
const gchar* gapp_widget_dialog_input_get_text(GtkWidget* dialog)
{
    return gtk_editable_get_text(GTK_EDITABLE(g_object_get_data(dialog, "entry")));
}

/**
 * Establece el margen de un widget en Gobu.
 *
 * @param widget  El widget al que se aplicará el margen.
 * @param margin  El valor del margen que se desea establecer.
 */
void gapp_widget_set_margin(GtkWidget* widget, gint margin)
{
    gapp_widget_set_margin_start(widget, margin);
    gapp_widget_set_margin_end(widget, margin);
    gapp_widget_set_margin_top(widget, margin);
    gapp_widget_set_margin_bottom(widget, margin);
}

/**
 * Crea un nuevo widget de botón con icono y etiqueta en Gobu.
 *
 * @param icon_name  El nombre del icono a mostrar en el botón.
 * @param label      El texto de la etiqueta del botón.
 *
 * @return Un nuevo widget de botón con icono y etiqueta.
 */
GtkWidget* gapp_widget_button_new_icon_with_label(const gchar* icon_name, const gchar* label)
{
    GtkWidget* button = gtk_button_new();
    gtk_button_set_has_frame(button, FALSE);
    {
        GtkWidget* hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_button_set_child(button, hbox_parent);

        GtkWidget* image_icon = gtk_image_new_from_icon_name(icon_name);
        gtk_image_set_icon_size(image_icon, GTK_ICON_SIZE_INHERIT);
        gtk_box_append(GTK_BOX(hbox_parent), image_icon);

        if (label != NULL)
            gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(label));
    }

    return button;
}

/**
 * Crea un nuevo widget de botón de información en Gobu.
 *
 * @param title  El título de la información a mostrar en el botón.
 * @param desc   La descripción de la información a mostrar en el botón.
 *
 * @return Un nuevo widget de botón de información.
 */
GtkWidget* gapp_widget_button_new_info(const gchar* title, const gchar* desc)
{
    GtkWidget* button = gtk_button_new();
    gtk_button_set_has_frame(button, FALSE);
    {
        GtkWidget* hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_button_set_child(button, hbox_parent);

        GtkWidget* image_icon = gtk_image_new_from_icon_name("input-gaming");
        gtk_image_set_icon_size(image_icon, GTK_ICON_SIZE_LARGE);
        gtk_box_append(GTK_BOX(hbox_parent), image_icon);

        gtk_box_append(GTK_BOX(hbox_parent), gtk_separator_new(GTK_ORIENTATION_VERTICAL));

        GtkWidget* hbox_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_box_append(GTK_BOX(hbox_parent), hbox_text);

        GtkWidget* label_title = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label_title), g_strdup_printf("<b><big>%s</big></b>", title));
        gtk_widget_set_halign(label_title, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(hbox_text), label_title);

        gtk_box_append(GTK_BOX(hbox_text), gtk_label_new(desc));
    }

    return button;
}

/**
 * Crea un nuevo widget de paneles (paned) en Gobu.
 *
 * @param orientation  La orientación de los paneles (GtkOrientation).
 * @param start        TRUE si se debe colocar el widget en el inicio, FALSE en caso contrario.
 *
 * @return Un nuevo widget de paneles (paned).
 */
GtkWidget* gapp_widget_paned_new(GtkOrientation orientation, gboolean start)
{
    GtkWidget* paned;

    paned = gtk_paned_new(orientation);
    if (start)
    {
        gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);
        gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    }
    else
    {
        gtk_paned_set_resize_end_child(GTK_PANED(paned), FALSE);
        gtk_paned_set_shrink_end_child(GTK_PANED(paned), FALSE);
    }

    gtk_paned_set_wide_handle(GTK_PANED(paned), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(paned), TRUE);

    return paned;
}

/**
 * Crea un nuevo widget de paneles con pestañas en Gobu.
 *
 * @param orientation  La orientación de los paneles (GtkOrientation).
 * @param rink_start   TRUE si se deben mostrar las pestañas al inicio, FALSE en caso contrario.
 * @param label_start  El widget de etiqueta para el panel de inicio.
 * @param start        El contenido del panel de inicio.
 * @param label_end    El widget de etiqueta para el panel de fin.
 * @param end          El contenido del panel de fin.
 *
 * @return Un nuevo widget de paneles con pestañas.
 */
GtkWidget* gapp_widget_paned_new_with_notebook(GtkOrientation orientation, gboolean rink_start, GtkWidget* label_start, GtkWidget* start, GtkWidget* label_end, GtkWidget* end)
{
    GtkWidget* paned, * notebook;

    paned = gapp_widget_paned_new(orientation, rink_start);

    notebook = gtk_notebook_new();
    gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
    gtk_widget_set_size_request(GTK_WIDGET(notebook), DOCK_L_MIN_SIZE, DOCK_T_MIN_SIZE);
    gtk_paned_set_start_child(GTK_PANED(paned), notebook);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), start, label_start);

    if (label_end != NULL && end != NULL)
    {
        notebook = gtk_notebook_new();
        gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
        gtk_widget_set_size_request(GTK_WIDGET(notebook), DOCK_L_MIN_SIZE, DOCK_T_MIN_SIZE);
        gtk_paned_set_end_child(GTK_PANED(paned), notebook);

        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), end, label_end);
    }

    return paned;
}

/**
 * Crea una nueva barra de herramientas en Gobu.
 *
 * @return Una nueva barra de herramientas.
 */
GtkWidget* gapp_widget_toolbar_new(void)
{
    GtkWidget* self;
    self = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(self, "toolbar");
    gapp_widget_set_margin(self, 3);
    return self;
}

/**
 * Agrega un separador en una barra de herramientas en Gobu.
 *
 * @param toolbar  La barra de herramientas a la que se agregará el separador.
 */
void gapp_widget_toolbar_separator_new(GtkWidget* toolbar)
{
    GtkWidget* self;
    self = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    // gapp_widget_set_margin_end(self, 2);
    // gapp_widget_set_margin_start(self, 2);
    gtk_box_append(toolbar, self);
}

/**
 * Crea un separador vertical en Gobu.
 *
 * @return Un nuevo separador vertical.
 */
GtkWidget* gapp_widget_separator_v(void)
{
    return gtk_separator_new(GTK_ORIENTATION_VERTICAL);
}

/**
 * Crea un separador horizontal en Gobu.
 *
 * @return Un nuevo separador horizontal.
 */
GtkWidget* gapp_widget_separator_h(void)
{
    return gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
}

/**
 * Crea un nuevo expansor con un widget interno en Gobu.
 *
 * @param title   El título del expansor.
 * @param widget  El widget interno que se mostrará u ocultará al expandir o contraer el expansor.
 *
 * @return Un nuevo widget que representa el expansor con el widget interno.
 */
GtkWidget* gapp_widget_expander_with_widget(const gchar* title, GtkWidget* widget)
{
    GtkWidget* box, * expand, * label;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    {
        label = gtk_label_new(title);
        gtk_label_set_use_markup(label, TRUE);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_box_append(box, label);

        if (widget != NULL)
            gtk_box_append(box, widget);
    }

    expand = gtk_expander_new(NULL);
    {
        gtk_expander_set_label_widget(expand, box);
        gtk_expander_set_expanded(expand, TRUE);
        gtk_expander_set_use_markup(expand, TRUE);
        gtk_widget_add_css_class(expand, "expander_n");
    }

    return expand;
}

void gapp_widget_entry_set_text(GtkEntry* entry, const gchar* text)
{
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), text, strlen(text));
}

const gchar* gapp_widget_entry_get_text(GtkEntry* entry)
{
    return gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
}

void gapp_widget_view_set_text(GtkTextView* view, const gchar* text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(view), text, strlen(text));
}

gchar* gapp_widget_view_get_text(GtkTextBuffer* buffer)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}
