#include "gapp_script.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include <gtksourceview/gtksource.h>

struct _GappScript
{
    GtkBox parent_instance;
    GtkWidget *buffer, *view_source, *console;
    gchar *filename;
};

// MARK:PRIVATE
static void gapp_script_interface(GappScript *self);

// MARK:BASE CLASS
G_DEFINE_TYPE(GappScript, gapp_script, GTK_TYPE_BOX)

static void dispose(GObject *object)
{
    GappScript *self = GAPP_SCRIPT(object);
    G_OBJECT_CLASS(gapp_script_parent_class)->dispose(object);
}

static void gapp_script_class_init(GappScriptClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = dispose;
}

static void gapp_script_init(GappScript *self)
{
    gapp_script_interface(self);
}

GappScript *gapp_script_new(void)
{
    return g_object_new(GAPP_TYPE_SCRIPT,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        NULL);
}

// MARK: SIGNALS

// MARK: UI
static void gapp_script_interface(GappScript *self)
{
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, "Adwaita-dark");

    GtkSourceLanguageManager *managerLanguage = gtk_source_language_manager_get_default();
    GtkSourceLanguage *language = gtk_source_language_manager_get_language(managerLanguage, GAPP_SCRIPT_LENGUAGE);

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(self), scroll);

    self->view_source = gtk_source_view_new();
    gtk_source_view_set_show_line_numbers(self->view_source, TRUE);
    gtk_source_view_set_highlight_current_line(self->view_source, TRUE);
    gtk_source_view_set_show_line_marks(self->view_source, TRUE);
    gtk_source_view_set_enable_snippets(self->view_source, TRUE);
    gtk_source_view_set_auto_indent(self->view_source, TRUE);
    gtk_source_view_set_indent_on_tab(self->view_source, TRUE);
    gtk_source_view_set_tab_width(self->view_source, 4);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(self->view_source), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(self->view_source), 10);

    self->buffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->view_source)));
    gtk_source_buffer_set_style_scheme(self->buffer, scheme);
    gtk_source_buffer_set_language(self->buffer, language);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->view_source);
}

// MARK: PUBLIC
void gapp_script_save(GappScript *self)
{
    gchar *text = gapp_script_get_text(self);

    g_file_set_contents(self->filename, text, -1, NULL);
    g_free(text);

    g_print("File successfully saved to %s", self->filename);
}

void gapp_script_load_file(GappScript *self, const gchar *filename)
{
    g_return_if_fail(GAPP_IS_SCRIPT(self));
    g_return_if_fail(filename != NULL);

    gsize size = 0;
    g_autoptr(GError) error = NULL;
    g_autofree gchar *buffer = NULL;

    if (!g_file_get_contents(filename, &buffer, &size, &error))
    {
        g_warning("Failed to load file '%s': %s", filename, error->message);
        return;
    }

    gtk_text_buffer_set_text(self->buffer, buffer, size);
    self->filename = stringDup(filename);
}

const gchar *gapp_script_get_filename(GappScript *self)
{
    g_return_val_if_fail(GAPP_IS_SCRIPT(self), NULL);
    return self->filename;
}

gchar *gapp_script_get_text(GappScript *self)
{
    g_return_val_if_fail(GAPP_IS_SCRIPT(self), NULL);

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(self->buffer, &start, &end);
    return gtk_text_buffer_get_text(self->buffer, &start, &end, FALSE);
}

void gapp_script_set_text(GappScript *self, const gchar *text)
{
    g_return_if_fail(GAPP_IS_SCRIPT(self));
    g_return_if_fail(text != NULL);

    gtk_text_buffer_set_text(self->buffer, text, strlen(text));
}

GtkWidget *gapp_script_get_buffer(GappScript *self)
{
    g_return_val_if_fail(GAPP_IS_SCRIPT(self), NULL);
    return self->buffer;
}
