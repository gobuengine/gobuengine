#include "gapp_tool_console.h"
#include "gapp_main.h"
#include "gobu_utility.h"

struct _GobuToolConsole
{
    GtkWidget parent;
    GtkWidget* console;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuToolConsole, gapp_tool_console, GTK_TYPE_BOX);

static void gapp_tool_console_class_init(GobuToolConsoleClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

//  -------------------------

extern GAPP* EditorCore;

void gapp_console_print(ConsoleTypeMessage type, const gchar* msg)
{
    gapp_tool_console_print(EditorCore->console, type, msg);
}

void gapp_tool_console_print(GobuToolConsole* self, ConsoleTypeMessage type, const gchar* msg)
{
    if (self->console != NULL) {
        gchar* out;
        if (type == CONSOLE_INFO) {
            out = gb_strdups("<b><span font='10' color='#8BC34A'>%s</span></b>", msg);
            g_print("INFO: %s\n", msg);
        }
        else if (type == CONSOLE_WARNING) {
            out = gb_strdups("<b><span font='10' color='#8BC34A'>%s</span></b>", msg);
            g_print("WARNING: %s\n", msg);
        }
        else if (type == CONSOLE_ERROR) {
            out = gb_strdups("<b><span font='10' color='#8BC34A'>%s</span></b>", msg);
            g_print("ERROR: %s\n", msg);
        }
        else if (type == CONSOLE_SUCCESS) {
            out = gb_strdups("<b><span font='10' color='#8BC34A'>%s</span></b>", msg);
            g_print("SUCCESS: %s\n", msg);
        }

        GtkWidget* label = gtk_label_new(g_strdup_printf("%s %s", __TIME__, out));
        gtk_label_set_use_markup(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_start(label, 10);
        gtk_widget_set_margin_top(label, 5);
        gtk_box_prepend(self->console, label);
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

    gapp_project_editor_append_page(GOBU_NOTEBOOK_BOTTOM, 0, "Console", self);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(scroll), -1, 100);
    gtk_box_append(self, scroll);
    {
        self->console = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_hexpand(self->console, TRUE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->console);
    }

    gapp_tool_console_print(self, CONSOLE_INFO, "Inicialize console");
}

GobuToolConsole* gapp_tool_console_new(void)
{
    GobuToolConsole* console = g_object_new(GAPP_TOOL_TYPE_CONSOLE, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
    return console;
}
