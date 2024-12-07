#include "gapp_components.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include <gtksourceview/gtksource.h>

enum
{
    PROP_0,
    PROP_FILENAME,
    N_PROPS
};

struct _GappComponent
{
    GtkBox parent_instance;
    GtkWidget *buffer, *view_source, *console;
    gchar *filename;
    gboolean modified;
    guint timeout;
    GtkWidget *componentslist;
};

static GParamSpec *properties[N_PROPS];

// MARK:PRIVATE
static void gappComponentConfigureInterface(GappComponent *self);

static void onComponentReady(GtkTextBuffer *buffer, GappComponent *self);
static void onComponentBufferChanged(GtkTextBuffer *buffer, GappComponent *self);
static gboolean onComponentAutosaveTimeout(GappComponent *self);
static void onComponentToolbarSave(GtkWidget *widget, GappComponent *self);
static const gchar *componentGetFilename(GappComponent *self);
static void componentLoadFile(GappComponent *self, const gchar *filename);
static void componentSaveFile(GappComponent *self);

// MARK:BASE CLASS
G_DEFINE_TYPE(GappComponent, gapp_component, GTK_TYPE_BOX)

static void set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GappComponent *self = GAPP_COMPONENT(object);

    switch (property_id)
    {
    case PROP_FILENAME:
        g_free(self->filename);
        self->filename = g_value_dup_string(value);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GappComponent *self = GAPP_COMPONENT(object);

    switch (property_id)
    {
    case PROP_FILENAME:
        g_value_set_string(value, self->filename);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void dispose(GObject *object)
{
    GappComponent *self = GAPP_COMPONENT(object);

    if (self->timeout > 0)
        g_source_remove(self->timeout);

    G_OBJECT_CLASS(gapp_component_parent_class)->dispose(object);
}

static void gapp_component_class_init(GappComponentClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = dispose;
    object_class->set_property = set_property;
    object_class->get_property = get_property;

    // Registrar las propiedades del objeto
    properties[PROP_FILENAME] = g_param_spec_string("filename", "fileName", "The file name of the item", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void gapp_component_init(GappComponent *self)
{
    gappComponentConfigureInterface(self);
}

GappComponent *gapp_component_new(const gchar *filename)
{
    return g_object_new(GAPP_TYPE_COMPONENT,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        "filename", filename,
                        NULL);
}

// MARK: SIGNALS
static void onComponentReady(GtkTextBuffer *buffer, GappComponent *self)
{
    const gchar *filename = componentGetFilename(self);
    if (self->filename)
        componentLoadFile(self, self->filename);
}

static void onComponentBufferChanged(GtkTextBuffer *buffer, GappComponent *self)
{
    self->modified = TRUE;
}

static gboolean onComponentAutosaveTimeout(GappComponent *self)
{
    if (self->modified)
    {
        componentSaveFile(self);
        self->modified = FALSE;
    }

    return TRUE;
}

static void onComponentToolbarSave(GtkWidget *widget, GappComponent *self)
{
    componentSaveFile(self);
    self->modified = FALSE;
}

static void onComponentDialogNewComponent(GtkWidget *widget, GappWidgetDialogEntry *dialog)
{
    const char *name_component = gapp_widget_entry_get_text(GTK_ENTRY(dialog->entry));
    
    GtkWidget *item = gtk_label_new(name_component);
    gapp_widget_set_margin(item, 5);
    gtk_list_box_append(GTK_LIST_BOX(dialog->data), item);

    gapp_widget_dialog_entry_text_destroy(dialog);
}

static void onComponentListComponentAddNew(GtkWidget *widget, GappComponent *self)
{
    GappWidgetDialogEntry *dialog = gapp_widget_dialog_new_entry_text(gtk_widget_get_root(widget), "New component", "Name");
    dialog->data = self->componentslist;
    g_signal_connect(dialog->button_ok, "clicked", onComponentDialogNewComponent, dialog);
}

// MARK: UI
static void gappComponentConfigureInterface(GappComponent *self)
{
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, GAPP_SCRIPT_THEME);

    GtkSourceLanguageManager *managerLanguage = gtk_source_language_manager_get_default();
    GtkSourceLanguage *language = gtk_source_language_manager_get_language(managerLanguage, GAPP_SCRIPT_LENGUAGE);

    GtkWidget *toolbar = gapp_widget_toolbar_new();
    gtk_box_append(GTK_BOX(self), toolbar);
    {
        GtkWidget *item;

        item = gapp_widget_button_new_icon_with_label("media-removable-symbolic", "Save");
        gtk_box_append(GTK_BOX(toolbar), item);
        gtk_widget_set_tooltip_text(item, "Save script");
        g_signal_connect(item, "clicked", G_CALLBACK(onComponentToolbarSave), self);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    // paned
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(paned), 200);
    gtk_box_append(GTK_BOX(self), paned);

    { // panel left
        GtkWidget *expander;

        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_paned_set_start_child(GTK_PANED(paned), scroll);
        gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);

        GtkWidget *boxleft = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), boxleft);

        expander = gapp_widget_expander_new_label_icon("Components", "tab-new-symbolic", TRUE);
        GtkWidget *button_add_comps = gapp_widget_expander_get_button(expander);
        g_signal_connect(button_add_comps, "clicked", G_CALLBACK(onComponentListComponentAddNew), self);
        gtk_box_append(GTK_BOX(boxleft), expander);
        {
            self->componentslist = gtk_list_box_new();
            gtk_expander_set_child(GTK_EXPANDER(expander), self->componentslist);
            gtk_widget_set_vexpand(self->componentslist, TRUE);
            gtk_widget_set_hexpand(self->componentslist, TRUE);
        }
    }

    { // panel right
        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_paned_set_end_child(GTK_PANED(paned), scroll);
        // gtk_box_append(GTK_BOX(self), scroll);

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

        g_signal_connect(self->buffer, "changed", G_CALLBACK(onComponentBufferChanged), self);
        g_signal_connect(self, "realize", G_CALLBACK(onComponentReady), self);
    }
}

// MARK: PUBLIC
static void componentSaveFile(GappComponent *self)
{
    GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->view_source)));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);

    gchar *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, FALSE);

    g_file_set_contents(self->filename, text, -1, NULL);
    g_free(text);

    g_print("File successfully saved to %s", self->filename);
}

static void componentLoadFile(GappComponent *self, const gchar *filename)
{
    g_return_if_fail(GAPP_IS_COMPONENT(self));
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
    g_object_set_data(self, "filename", g_strdup(filename));

    self->modified = FALSE;

    if (self->timeout > 0)
        g_source_remove(self->timeout);

    // self->timeout = g_timeout_add(5000, onComponentAutosaveTimeout, self);
}

static const gchar *componentGetFilename(GappComponent *self)
{
    g_return_val_if_fail(GAPP_IS_COMPONENT(self), NULL);
    return self->filename;
}
