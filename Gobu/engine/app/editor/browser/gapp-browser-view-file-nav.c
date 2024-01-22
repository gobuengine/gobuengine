#include "gapp-browser-view-file-nav.h"

/**
 * Limpia el historial de rutas hacia adelante en el explorador de mundos del editor en Gobu.
 *
 * @param browser  El explorador de mundos en el que se limpia el historial.
 */
void gapp_browser_fn_history_path_forward_clear(GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    if (private->path_back->len == 0 && private->path_forward != 0)
    {
        g_ptr_array_remove_range(private->path_forward, 0, private->path_forward->len);
    }
}

/**
 * Maneja la señal de navegación a la página de inicio en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la navegación a la página de inicio.
 * @param data    Datos relacionados con la navegación a la página de inicio en el explorador de mundos.
 */
void gapp_browser_view_file_signal_navhome(GtkWidget *button, GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);
    gapp_browser_fn_open_folder(browser, private->path_default, TRUE);
}

/**
 * Maneja la señal de navegación hacia atrás en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la navegación hacia atrás.
 * @param data    Datos relacionados con la navegación hacia atrás en el explorador de mundos.
 */
void gapp_browser_view_file_signal_navback(GtkWidget *button, GappBrowser *browser)
{
    // GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);
    gapp_browser_fn_history_path_back(browser);
}

/**
 * Maneja la señal de navegación hacia adelante en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la navegación hacia adelante.
 * @param data    Datos relacionados con la navegación hacia adelante en el explorador de mundos.
 */
void gapp_browser_view_file_signal_navforward(GtkWidget *button, GappBrowser *browser)
{
    // GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);
    gapp_browser_fn_history_path_forward(browser);
}

/**
 * Navega hacia atrás en el historial de rutas en el explorador de mundos del editor en Gobu.
 *
 * @param browser  El explorador de mundos en el que se navega hacia atrás en el historial.
 */
void gapp_browser_fn_history_path_back(GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    guint index_last = (private->path_back->len - 1);
    const gchar *path = g_ptr_array_index(private->path_back, index_last);
    g_ptr_array_remove_index(private->path_back, index_last);
    g_ptr_array_add(private->path_forward, gb_strdup(private->path_current));

    gapp_browser_fn_open_folder(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), !(private->path_back->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), TRUE);
}

/**
 * Navega hacia adelante en el historial de rutas en el explorador de mundos del editor en Gobu.
 *
 * @param browser  El explorador de mundos en el que se navega hacia adelante en el historial.
 */
void gapp_browser_fn_history_path_forward(GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    guint index_last = (private->path_forward->len - 1);
    const gchar *path = g_ptr_array_index(private->path_forward, index_last);
    g_ptr_array_remove_index(private->path_forward, index_last);
    g_ptr_array_add(private->path_back, gb_strdup(private->path_current));

    gapp_browser_fn_open_folder(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), !(private->path_forward->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
}

