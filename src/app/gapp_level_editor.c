#include "gapp_level_editor.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_viewport.h"
#include "gapp_level_outliner.h"
#include "gapp_level_inspector.h"

struct _GobuLevelEditor
{
    GtkBox parent_instance;
    GtkWidget *outliner;
    GtkWidget *viewport;
    GtkWidget *inspector;
};

static void gapp_level_viewport_s_ready(GtkWidget *viewport, int width, int height, GtkWidget *outliner);
static void gobu_level_editor_dispose(GObject *object);

// --- BASE CLASS ---

G_DEFINE_TYPE(GobuLevelEditor, gobu_level_editor, GTK_TYPE_BOX)

static void gobu_level_editor_class_init(GobuLevelEditorClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gobu_level_editor_dispose;
}

static void gobu_level_editor_dispose(GObject *object)
{
    GobuLevelEditor *self = GOBU_LEVEL_EDITOR(object);

    // g_clear_pointer(&self, gtk_widget_unparent);

    G_OBJECT_CLASS(gobu_level_editor_parent_class)->dispose(object);
}

// --- END CLASS ---

// --- BEGIN UI ---

static void gobu_level_editor_init(GobuLevelEditor *self)
{
    g_return_if_fail(GOBU_IS_LEVEL_EDITOR(self));

    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(paned), 220);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    gtk_box_append(GTK_BOX(self), paned);
    {
        self->outliner = gapp_outliner_new();
        gtk_widget_set_size_request(self->outliner, 220, -1);
        gtk_paned_set_start_child(GTK_PANED(paned), self->outliner);

        GtkWidget *paned_b = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_paned_set_resize_end_child(GTK_PANED(paned_b), FALSE);
        gtk_paned_set_shrink_end_child(GTK_PANED(paned_b), FALSE);
        gtk_paned_set_end_child(GTK_PANED(paned), paned_b);
        {
            self->viewport = gapp_viewport_new();
            gtk_paned_set_start_child(GTK_PANED(paned_b), self->viewport);

            self->inspector = gapp_inspector_new();
            gtk_widget_set_size_request(self->inspector, 300, -1);
            gtk_paned_set_end_child(GTK_PANED(paned_b), self->inspector);

            g_signal_connect(self->viewport, "viewport-ready", G_CALLBACK(gapp_level_viewport_s_ready), self->outliner);
        }
    }
}

// --- END UI ---

static void gapp_level_viewport_s_ready(GtkWidget *viewport, int width, int height, GtkWidget *outliner)
{
    ecs_world_t *world = gapp_viewport_get_world(viewport);
    gapp_outliner_start_process(outliner, world);
}

// --- BEGIN API ---

/**
 * gobu_level_editor_new:
 * @app: La instancia de GtkApplication a la que se asociará el GobuLevelEditor.
 *
 * Crea una nueva instancia de GobuLevelEditor.
 *
 * Returns: (transfer full): Una nueva instancia de #GobuLevelEditor.
 *   Usa g_object_unref() cuando ya no la necesites.
 */
GobuLevelEditor *gobu_level_editor_new(void)
{
    return g_object_new(GOBU_TYPE_LEVEL_EDITOR, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}

GtkWidget *gobu_level_editor_get_outliner(GobuLevelEditor *self)
{
    g_return_val_if_fail(GOBU_IS_LEVEL_EDITOR(self), NULL);
    return self->outliner;
}

GtkWidget *gobu_level_editor_get_viewport(GobuLevelEditor *self)
{
    g_return_val_if_fail(GOBU_IS_LEVEL_EDITOR(self), NULL);
    return self->viewport;
}

GtkWidget *gobu_level_editor_get_inspector(GobuLevelEditor *self)
{
    g_return_val_if_fail(GOBU_IS_LEVEL_EDITOR(self), NULL);
    return self->inspector;
}

// --- END API ---
