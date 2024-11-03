#include "gapp_level_editor.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_viewport.h"
#include "gapp_level_outliner.h"
#include "gapp_level_inspector.h"

#include "types/type_outliner_item.h"
#include "pixio/pixio.h"

struct _GobuLevelEditor
{
    GtkBox parent_instance;
    GtkWidget *outliner;
    GtkWidget *viewport;
    GtkWidget *inspector;
    ecs_world_t *world;
    ecs_entity_t root;
};

static void gapp_level_viewport_s_render(GtkWidget *viewport, gdouble delta, GobuLevelEditor *self);
static void gapp_level_viewport_s_ready(GtkWidget *viewport, int width, int height, GobuLevelEditor *self);
static void gobu_level_editor_dispose(GObject *object);
static void gapp_s_hooks_callback(ecs_iter_t *it);

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

    //
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
            gapp_viewport_set_custom_render(self->viewport, FALSE);
            gtk_paned_set_start_child(GTK_PANED(paned_b), self->viewport);

            self->inspector = gapp_inspector_new();
            gtk_widget_set_size_request(self->inspector, 300, -1);
            gtk_paned_set_end_child(GTK_PANED(paned_b), self->inspector);

            g_signal_connect(self->viewport, "viewport-ready", G_CALLBACK(gapp_level_viewport_s_ready), self);
            g_signal_connect(self->viewport, "viewport-render", G_CALLBACK(gapp_level_viewport_s_render), self);
        }
    }

    // iniciamos el world ecs
    self->world = pixio_world_init();
    self->root = pixio_get_root(self->world);

    // Configura los hooks para el componente pixio_transform_t
    ecs_set_hooks(self->world, pixio_transform_t, {.on_add = gapp_s_hooks_callback, .on_remove = gapp_s_hooks_callback, .ctx = self});

    pixio_new_root(self->world);
}

// --- END UI ---

static void gapp_level_viewport_s_ready(GtkWidget *viewport, int width, int height, GobuLevelEditor *self)
{
}

static void gapp_level_viewport_s_render(GtkWidget *viewport, gdouble delta, GobuLevelEditor *self)
{
    float deltaTime = (float)delta;
    pixio_world_process(self->world, deltaTime);
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

/**
 * Función de callback para los hooks de ECS para manejar eventos de adición y eliminación de entidades.
 *
 * @param it Puntero al iterador ECS que contiene la información del evento.
 */
static void gapp_s_hooks_callback(ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;
    GobuLevelEditor *levelEditor = it->ctx;
    GtkSingleSelection *outlinerSelect = gapp_outliner_get_selection(levelEditor->outliner);

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];

        if (event == EcsOnAdd)
        {
            TOutlinerItem *item = toutliner_item_new(world, entity);
            toutliner_item_set_root(item, gapp_outliner_get_store(levelEditor->outliner));

            // Verificamos si la entidad tiene padre
            if (pixio_has_parent(world, entity))
            {
                // Obtenemos el padre
                ecs_entity_t parent = pixio_get_parent(world, entity);
                TOutlinerItem *item_find = gapp_outliner_fn_find_item_by_entity(outlinerSelect, parent);
                if (item_find != NULL)
                {
                    GListStore *children = toutliner_item_get_children(item_find);

                    toutliner_item_set_root(item, children);
                    gapp_outliner_append_item_children(children, item);
                    continue;
                }
            }

            // La entidad no tiene padre, se agrega al almacén raíz
            gapp_outliner_append_item_root(levelEditor->outliner, item);
        }
        else if (event == EcsOnRemove)
        {
            guint position;
            TOutlinerItem *item = gapp_outliner_fn_find_item_by_entity(outlinerSelect, entity);
            if (item && g_list_store_find(toutliner_item_get_root(item), item, &position))
                g_list_store_remove(toutliner_item_get_root(item), position);
        }
    }
}
