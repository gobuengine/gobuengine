#include "GobuWidgets.h"
#include "config.h"
/**
 * GobuWidgetThemeInit:
 *
 * Cambia el tema a dark y cargamos un .css
 * para realizar algunos cambios en los Widgets.
 *
 * Returns: Void
 */

void GobuWidgetThemeInit(void)
{
    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "resource/theme/default.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

/**
 * GobuWidgetAlert:
 *
 * Mostramos un mensaje de alert
 *
 * Returns: Void
 */

void GobuWidgetAlert(GtkWidget *parent, const gchar *message)
{
    GtkAlertDialog *alert = gtk_alert_dialog_new(message);
    gtk_alert_dialog_set_modal(alert, TRUE);
    gtk_alert_dialog_show(alert, parent);
}

/**
 * GobuWidgetDialogConfirmDelete:
 *
 * Mostramos un tipo de confirmacion para borrar un archivo.
 *
 * Returns: Void
 */

void GobuWidgetDialogConfirmDelete(GtkWidget *parent, GFile *file, GCallback c_handler)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delete Assets", GTK_WINDOW(gtk_widget_get_root(parent)), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR, "Delete", GTK_RESPONSE_OK, NULL);
    gtk_window_set_resizable(dialog, FALSE);

    GtkWidget *box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GobuWidgetSetMargin(GTK_WIDGET(box), 18);

    GtkWidget *label = gtk_label_new(g_strdup_printf("Esta seguro que desea eliminar este archivo <b>%s</b> ?", g_file_get_basename(file)));
    gtk_label_set_use_markup(label, TRUE);
    gtk_box_append(GTK_BOX(box), label);

    g_signal_connect(dialog, "response", G_CALLBACK(c_handler), file);

    gtk_window_present(dialog);
}

/**
 * GobuWidgetDialogInput:
 *
 * Mostramos dialog con un input para escribir, se requiere una señal
 * tipo response para recibir la informacion.
 *
 * Para leer el input-text utilizar GobuWidgetDialogInputGet
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetDialogInput(GtkWidget *parent, const gchar *title, const gchar *text_default)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons(title, GTK_WINDOW(gtk_widget_get_root(parent)), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR, "Create", GTK_RESPONSE_OK, NULL);
    gtk_window_set_resizable(dialog, FALSE);

    GtkWidget *box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GobuWidgetSetMargin(GTK_WIDGET(box), 18);

    GtkWidget *entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(text_default, -1));
    gtk_box_append(GTK_BOX(box), entry);
    g_object_set_data(dialog, "entry", entry);

    gtk_window_present(dialog);

    return dialog;
}

/**
 * GobuWidgetDialogInputGet:
 *
 * Utilizamos para recibir el texto escrito
 * en el input del GobuWidgetDialogInput
 *
 * Returns: gchar
 */

const gchar *GobuWidgetDialogInputGetText(GtkWidget *dialog)
{
    return gtk_editable_get_text(GTK_EDITABLE(g_object_get_data(dialog, "entry")));
}

/**
 * GobuWidgetSetMargin:
 *
 * Aplicamos un margin en todas las direcciones
 *
 * Returns: Void
 */

void GobuWidgetSetMargin(GtkWidget *widget, gint margin)
{
    GobuWidgetSetMarginStart(widget, margin);
    GobuWidgetSetMarginEnd(widget, margin);
    GobuWidgetSetMarginTop(widget, margin);
    GobuWidgetSetMarginBottom(widget, margin);
}

/**
 * GobuWidgetButtonIconLabelNew:
 *
 * Creamos un button con un icono y un texto
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetButtonIconLabelNew(const gchar *icon_name, const gchar *label)
{
    GtkWidget *button = gtk_button_new();
    gtk_button_set_has_frame(button, FALSE);
    {
        GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_button_set_child(button, hbox_parent);

        GtkWidget *image_icon = gtk_image_new_from_icon_name(icon_name);
        // gtk_image_set_icon_size(image_icon, GTK_ICON_SIZE_LARGE);
        gtk_box_append(GTK_BOX(hbox_parent), image_icon);

        if (label != NULL)
            gtk_box_append(GTK_BOX(hbox_parent), gtk_label_new(label));
    }

    return button;
}

/**
 * GobuWidgetButtonInfoNew:
 *
 * Creamos un button con un icono de informacion, titulo y una desc
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetButtonInfoNew(const gchar *title, const gchar *desc)
{
    GtkWidget *button = gtk_button_new();
    gtk_button_set_has_frame(button, FALSE);
    {
        GtkWidget *hbox_parent = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_button_set_child(button, hbox_parent);

        GtkWidget *image_icon = gtk_image_new_from_icon_name("input-gaming");
        gtk_image_set_icon_size(image_icon, GTK_ICON_SIZE_LARGE);
        gtk_box_append(GTK_BOX(hbox_parent), image_icon);

        gtk_box_append(GTK_BOX(hbox_parent), gtk_separator_new(GTK_ORIENTATION_VERTICAL));

        GtkWidget *hbox_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_box_append(GTK_BOX(hbox_parent), hbox_text);

        GtkWidget *label_title = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label_title), g_strdup_printf("<b><big>%s</big></b>", title));
        gtk_widget_set_halign(label_title, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(hbox_text), label_title);

        gtk_box_append(GTK_BOX(hbox_text), gtk_label_new(desc));
    }

    return button;
}

/**
 * GobuWidgetPanedNew:
 *
 * Creamos un GtkPaned con una configuracion que se
 * utiliza mucho.
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetPanedNew(GtkOrientation orientation, gboolean start)
{
    GtkWidget *paned;

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

GtkWidget *GobuWidgetPanedNotebookNew(GtkOrientation orientation, gboolean rink_start, GtkWidget *label_start, GtkWidget *start, GtkWidget *label_end, GtkWidget *end)
{
    GtkWidget *paned, *notebook;

    paned = GobuWidgetPanedNew(orientation, rink_start);

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
 * GobuWidgetToolbarNew:
 *
 * Creamos un toolbar
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetToolbarNew(void)
{
    GtkWidget *self;
    self = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(self, "toolbar");
    GobuWidgetSetMargin(self, 3);
    return self;
}

/**
 * GobuWidgetToolbarSeparatorWidget:
 *
 * Un separador con una configuracion para un toolbar
 *
 * Returns: GtkWidget
 */

void GobuWidgetToolbarSeparatorW(GtkWidget *toolbar)
{
    GtkWidget *self;
    self = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    GobuWidgetSetMarginEnd(self, 5);
    GobuWidgetSetMarginStart(self, 5);
    gtk_box_append(toolbar, self);
}

/**
 * GobuWidgetSeparatorV:
 *
 * Creamos un separador para un toolbar
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetSeparatorV(void)
{
    return gtk_separator_new(GTK_ORIENTATION_VERTICAL);
}

/**
 * GobuWidgetSeparatorH:
 *
 * Creamos un separador Horizontal
 *
 * Returns: GtkWidget
 */

GtkWidget *GobuWidgetSeparatorH(void)
{
    return gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
}
