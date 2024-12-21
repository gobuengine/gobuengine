#include "gapp_components.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_script.h"
#include <binn/binn_json.h>
#include <gtksourceview/gtksource.h>

struct _GappComponent
{
    GtkBox parent_instance;
    GtkWidget *componentslist;
    GtkWidget *script_view;
    gboolean is_modified;
};

// MARK:PRIVATE
static void gapp_component_interface(GappComponent *self);
static void gapp_component_on_toolbar_save(GtkWidget *widget, GappComponent *self);
static void gapp_on_component_code_view_changed(GtkTextBuffer *buffer, GappComponent *self);

// MARK:BASE CLASS
G_DEFINE_TYPE(GappComponent, gapp_component, GTK_TYPE_BOX)

static void dispose(GObject *object)
{
    GappComponent *self = GAPP_COMPONENT(object);
    G_OBJECT_CLASS(gapp_component_parent_class)->dispose(object);
}

static void gapp_component_class_init(GappComponentClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = dispose;
}

static void gapp_component_init(GappComponent *self)
{
    gapp_component_interface(self);
}

GappComponent *gapp_component_new(void)
{
    return g_object_new(GAPP_TYPE_COMPONENT,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        NULL);
}

// MARK:PRIVATE
static void gapp_component_save_components(GappComponent *self)
{
    self->is_modified = FALSE;
    gapp_script_save(GAPP_SCRIPT(self->script_view));
}

// MARK: SIGNALS
static void gapp_component_on_toolbar_save(GtkWidget *widget, GappComponent *self)
{
    gapp_component_save_components(self);
}

static void gapp_on_component_code_view_changed(GtkTextBuffer *buffer, GappComponent *self)
{
    self->is_modified = TRUE;
}

// MARK: UI
static void gapp_component_interface(GappComponent *self)
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
        g_signal_connect(item, "clicked", G_CALLBACK(gapp_component_on_toolbar_save), self);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    // paned
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(paned), 200);
    gtk_box_append(GTK_BOX(self), paned);

    self->script_view = gapp_script_new();
    gtk_widget_set_sensitive(self->script_view, TRUE);
    gtk_paned_set_end_child(GTK_PANED(paned), self->script_view);

    GtkWidget *buffer = gapp_script_get_buffer(GAPP_SCRIPT(self->script_view));
    g_signal_connect(buffer, "changed", G_CALLBACK(gapp_on_component_code_view_changed), self);
}

// MARK: PUBLIC
void gapp_component_load_file(GappComponent *self, const gchar *filename)
{
    gapp_script_load_file(GAPP_SCRIPT(self->script_view), filename);
}

const gchar *gapp_component_get_filename(GappComponent *self)
{
    return gapp_script_get_filename(GAPP_SCRIPT(self->script_view));
}

