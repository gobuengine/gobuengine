#include "gapp_widget.h"
#include "gapp_common.h"


/**
 * Establece el margen de un widget en Gobu.
 *
 * @param widget  El widget al que se aplicará el margen.
 * @param margin  El valor del margen que se desea establecer.
 */
void gapp_widget_set_margin(GtkWidget *widget, gint margin)
{
    gtk_widget_set_margin_start(widget, margin);
    gtk_widget_set_margin_end(widget, margin);
    gtk_widget_set_margin_top(widget, margin);
    gtk_widget_set_margin_bottom(widget, margin);
}

/**
 * Crea un separador horizontal en Gobu.
 *
 * @return Un nuevo separador horizontal.
 */
GtkWidget *gapp_widget_separator_h(void)
{
    return gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
}

/**
 * Crea una nueva barra de herramientas en Gobu.
 *
 * @return Una nueva barra de herramientas.
 */
GtkWidget *gapp_widget_toolbar_new(void)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
    gtk_widget_add_css_class(box, "toolbar");
    // gtk_widget_set_size_request(box, -1, 25);
    // gapp_widget_set_margin(box, 1);

    return box;
}

/**
 * Crea un nuevo widget de botón con icono y etiqueta en Gobu.
 *
 * @param icon_name  El nombre del icono a mostrar en el botón.
 * @param label      El texto de la etiqueta del botón.
 *
 * @return Un nuevo widget de botón con icono y etiqueta.
 */ 
GtkWidget *gapp_widget_button_new_icon_with_label_custom(const gchar *icon_name, const gchar *label, GtkAlign align)
{
    GtkWidget *button = gtk_button_new();
    gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
    {
        GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_button_set_child(GTK_BUTTON(button), hbox_parent);
        gtk_widget_set_halign(hbox_parent, align);

        if (icon_name != NULL)
        {
            GtkWidget *icon = gtk_image_new_from_icon_name(icon_name);
            gtk_box_append(GTK_BOX(hbox_parent), icon);
        }

        if (label != NULL)
            gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(label));
    }

    return button;
}

/**
 * Cierra la página actual del notebook.
 *
 * @param button El botón que activó el cierre (no usado).
 * @param notebook El notebook del cual cerrar la página.
 */
static void s_notebook_close_page(GtkWidget *button, GtkNotebook *notebook)
{
    g_return_if_fail(GTK_IS_NOTEBOOK(notebook));

    int page = gtk_notebook_get_current_page(notebook);
    if (page >= 0)
        gtk_notebook_remove_page(notebook, page);
}

/**
 * Añade una nueva página al notebook con una etiqueta personalizable y un botón de cierre opcional.
 *
 * @param notebook El widget del notebook.
 * @param label La etiqueta para la nueva pestaña.
 * @param child El contenido de la nueva página.
 * @param isButtonClose Si es TRUE, se añade un botón de cierre a la pestaña.
 */
int gapp_widget_notebook_append_page(GtkWidget *notebook, GtkWidget *label, GtkWidget *child, gboolean isButtonClose)
{
    g_return_val_if_fail(GTK_IS_NOTEBOOK(notebook), -1);
    g_return_val_if_fail(GTK_IS_WIDGET(label), -1);
    g_return_val_if_fail(GTK_IS_WIDGET(child), -1);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_append(GTK_BOX(box), label);

    if (isButtonClose)
    {
        GtkWidget *button = gtk_button_new_from_icon_name("window-close-symbolic");
        gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
        gtk_box_append(GTK_BOX(box), button);
        g_signal_connect(button, "clicked", G_CALLBACK(s_notebook_close_page), notebook);
    }

    int page_index = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), child, box);
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), child, TRUE);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_index);

    return page_index;
}

GtkWidget *gapp_widget_notebook_get_label(GtkWidget *notebook, GtkWidget *child)
{
    GtkWidget *box_main_label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), child);
    GtkWidget *box_label = gtk_widget_get_first_child(box_main_label);
    GtkWidget *label = gtk_widget_get_last_child(box_label);
    return label;
}

//  -- widget dialog base
static void _dialog_new_button_cancel_pressed1(GtkButton *widget, GappWidgetDialogEntry *dialog)
{
    gapp_widget_dialog_entry_text_destroy(dialog);
}

GappWidgetDialogEntry *gapp_widget_dialog_new_entry_text(GtkWidget *parent, const gchar *title, const gchar *label)
{
    GappWidgetDialogEntry *dialog = g_new0(GappWidgetDialogEntry, 1);

    dialog->window = gtk_window_new();
    gtk_window_set_transient_for(GTK_WINDOW(dialog->window), parent);

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), FALSE);
    gtk_window_set_titlebar(GTK_WINDOW(dialog->window), header);
    gtk_window_set_title(GTK_WINDOW(dialog->window), title);
    gtk_window_set_resizable(GTK_WINDOW(dialog->window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(dialog->window), 450, -1);
    gtk_window_set_deletable(GTK_WINDOW(dialog->window), FALSE);
    gtk_window_set_modal(GTK_WINDOW(dialog->window), TRUE);

    GtkWidget *button1 = gtk_button_new_with_mnemonic("_Cancelar");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button1);
    g_signal_connect(button1, "clicked", G_CALLBACK(_dialog_new_button_cancel_pressed1), dialog);

    dialog->button_ok = gtk_button_new_with_mnemonic("_Crear");
    gtk_widget_add_css_class(dialog->button_ok, "suggested-action");
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), dialog->button_ok);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(dialog->window), box);
    gapp_widget_set_margin(box, 18);

    GtkWidget *label1 = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(label1), 0.0);
    gtk_box_append(GTK_BOX(box), label1);

    dialog->entry = gtk_entry_new();
    // gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Ingrese el texto...");
    gtk_box_append(GTK_BOX(box), dialog->entry);

    gtk_widget_set_visible(dialog->window, TRUE);

    return dialog;
}

void gapp_widget_dialog_entry_text_destroy(GappWidgetDialogEntry *dialog)
{
    gtk_window_destroy(GTK_WINDOW(dialog->window));
    g_free(dialog);
}

// -- widget remove file

void gapp_widget_dialog_new_confirm_action(GtkWidget *parent, const gchar *title, const gchar *label, GAsyncReadyCallback *result, gpointer data)
{
    GtkAlertDialog *dialog1 = gtk_alert_dialog_new(title, NULL);
    gtk_alert_dialog_set_detail(dialog1, label);
    gtk_alert_dialog_set_buttons(dialog1, (const char *const[]){"Cancel", "Confirm", NULL});
    gtk_alert_dialog_choose(dialog1, parent, NULL, result, data);
}

// --

/**
 * @brief Creates a new GTK expander widget with a label and an icon button.
 *
 * The expander widget consists of a horizontal box containing a label and a button
 * with an icon. The box is used as the label widget for the expander.
 *
 * @param label The text to be displayed as the label.
 * @param icon_name The name of the icon to be displayed in the button.
 * @param expanded A flag indicating whether the expander should be initially expanded or not.
 * @return The created expander widget.
 */
// deprecated
GtkWidget *gapp_widget_expander_new_label_icon(const gchar *label, const gchar *icon_name, gboolean expanded)
{
    GtkWidget *expander = gtk_expander_new(NULL);
    gtk_expander_set_expanded(GTK_EXPANDER(expander), expanded);
    gtk_widget_add_css_class(expander, "expander_n");

    GtkWidget *boxt = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_expander_set_label_widget(GTK_EXPANDER(expander), boxt);
    {
        // label
        gtk_box_append(GTK_BOX(boxt), gtk_label_new(label));

        // button
        GtkWidget *button = gtk_button_new_from_icon_name(icon_name);
        gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
        gtk_widget_set_halign(button, GTK_ALIGN_END);
        gtk_widget_set_hexpand(button, TRUE);
        gtk_widget_add_css_class(button, "expander_button");
        gtk_box_append(GTK_BOX(boxt), button);
    }

    return expander;
}

GtkWidget *gapp_widget_expander_get_button(GtkWidget *expander)
{
    GtkWidget *box = gtk_expander_get_label_widget(GTK_EXPANDER(expander));
    return gtk_widget_get_last_child(box);
}

void gapp_widget_text_view_set_text(GtkTextView *view, const gchar *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(view), text, strlen(text));
}

gchar *gapp_widget_text_view_get_text(GtkTextBuffer *buffer)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

void gapp_widget_entry_set_text(GtkEntry *entry, const gchar *text)
{
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(entry)), text, strlen(text));
}

const gchar *gapp_widget_entry_get_text(GtkEntry *entry)
{
    return gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
}