#include "gapp_tool_console.h"
#include "gapp_main.h"

struct _GobuToolConsole
{
    GtkWidget parent;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuToolConsole, gapp_tool_console, GTK_TYPE_BOX);
static void gapp_tool_console_print(const gchar* msg);
static void gapp_tool_console_class_init(GobuToolConsoleClass* klass) {}

//  -------------------------

extern GAPP* EditorCore;

/**
 * @brief Imprime un mensaje en la consola de la herramienta.
 *
 * @param msg El mensaje a imprimir.
 */
static void gapp_tool_console_print(const gchar* msg)
{
    if (EditorCore != NULL && EditorCore->console != NULL) {
        GobuToolConsolePrivate* private = gapp_tool_console_get_instance_private(EditorCore->console);

        GtkWidget* label = gtk_label_new(g_strdup_printf("%s %s", __TIME__, msg));
        gtk_label_set_use_markup(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_start(label, 10);
        gtk_widget_set_margin_top(label, 5);
        gtk_widget_set_margin_bottom(label, 5);
        gtk_box_prepend(private->console, label);

        // utiliza el scroll para mostrar el ultimo mensaje
        GtkAdjustment* vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(private->scroll));
        double upper = gtk_adjustment_get_upper(vadjustment);
        gtk_adjustment_set_value(vadjustment, -upper);
    }
}

/**
 * Muestra la vista de código fuente de un script en Gobu.
 *
 * @return Un widget que representa la vista de código fuente del script.
 */
static void gapp_tool_console_init(GobuToolConsole* self)
{
    GtkWidget* scroll;

    GobuToolConsolePrivate* private = gapp_tool_console_get_instance_private(self);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_BOTTOM, 0, "Console", self);
    {
        gtk_widget_set_size_request(GTK_WIDGET(self), -1, 100);

        private->scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(private->scroll), TRUE);
        // gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(private->scroll), FALSE);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(private->scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(private->scroll, TRUE);
        gtk_widget_set_hexpand(private->scroll, TRUE);
        gtk_box_append(self, private->scroll);
        {
            private->console = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(private->scroll), private->console);
        }
    }

    gb_log_success("Inicialize console");
}

GobuToolConsole* gapp_tool_console_new(void)
{
    return g_object_new(GAPP_TOOL_TYPE_CONSOLE, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}

/**
 * @brief Imprime un mensaje de error junto con un mensaje de error adicional.
 *
 * @param msg El mensaje de error principal.
 * @param error El mensaje de error adicional.
 */
void gb_log_error(const gchar* msg, const gchar* error)
{
    gapp_tool_console_print(gb_strdups("<span color='#FF453A'><b>ERROR:</b></span> %s <span color='#FF453A'><b>%s</b></span>", msg, error));
    g_error("%s %s", msg, error);
}

/**
 * @brief Imprime una advertencia en la consola.
 *
 * Esta función imprime un mensaje de advertencia en la consola.
 *
 * @param msg El mensaje de advertencia a imprimir.
 */
void gb_log_warn(const gchar* msg)
{
    gapp_tool_console_print(gb_strdups("<span color='#FFEB3B'><b>WARNING:</b></span> %s", msg));
    g_warning("%s", msg);
}

/**
 * @brief Imprime un mensaje de éxito en la consola.
 *
 * @param msg El mensaje a imprimir.
 */
void gb_log_success(const gchar* msg)
{
    gapp_tool_console_print(gb_strdups("<span color='#4CAF50'><b>SUCCESS:</b></span> %s", msg));
    g_message("%s", msg);
}

/**
 * @brief Imprime un mensaje de información en la consola.
 *
 * @param msg El mensaje a imprimir.
 */
void gb_log_info(const gchar* msg)
{
    gapp_tool_console_print(gb_strdups("<span color='#2196F3'><b>INFO:</b></span> %s", msg));
    g_info("%s", msg);
}
