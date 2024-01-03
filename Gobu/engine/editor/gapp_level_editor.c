#include "gapp_level_editor.h"
#include "gapp_level_inspector.h"
#include "gapp_level_outliner.h"
#include "gapp_level_viewport.h"
#include "gapp_tool_console.h"

#include "gapp_widget.h"
#include "gapp_main.h"
#include "gobu.h"

struct _GappLevelEditor
{
    GtkWidget parent;

    GappLevelViewport* viewport;
    GappLevelInspector* inspector;
    GappLevelOutliner* outliner;

    ecs_world_t* world; // world ecs
    ecs_entity_t root;  // entity root level
    gchar* filename;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelEditor, gapp_level_editor, GTK_TYPE_BOX);

static void signal_toolbar_click_save_level(GtkWidget* button, GappLevelEditor* self);
static void signal_toolbar_click_zoom_reset(GtkWidget* button, GappLevelEditor* self);
static void signal_observer_state_world(ecs_iter_t* it);
static void signal_viewport_init(GappLevelEditor* self, int width, int height, gpointer data);

static void gapp_level_editor_class_finalize(GObject* object)
{
    GappLevelEditor* self = GAPP_LEVEL_EDITOR(object);

    gb_log_info(TF("Level Editor Free [%s]\n", gb_fs_get_name(self->filename, false)));

    ecs_quit(self->world);
    g_free(self->filename);
}

static void gapp_level_editor_class_init(GappLevelEditorClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = gapp_level_editor_class_finalize;

    g_signal_new("level-viewport-init", G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
}

static void gapp_level_editor_init(GappLevelEditor* self)
{
    GtkWidget* paned, * paned2, * toolbar, * vbox;

    paned = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
    gtk_box_append(self, paned);
    {
        // outliner
        {
            self->outliner = gapp_level_outliner_new(self);
            gtk_widget_set_size_request(GTK_WIDGET(self->outliner), 200, -1);
            gtk_paned_set_start_child(GTK_PANED(paned), self->outliner);
        }

        paned2 = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, FALSE);
        gtk_paned_set_end_child(GTK_PANED(paned), paned2);
        {
            // toolbar
            vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_paned_set_start_child(GTK_PANED(paned2), vbox);
            {
                toolbar = gapp_widget_toolbar_new();
                gtk_box_append(vbox, toolbar);
                {
                    GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
                    g_signal_connect(btn_s, "clicked", G_CALLBACK(signal_toolbar_click_save_level), self);
                    gtk_box_append(toolbar, btn_s);

                    GtkWidget* btn_reset_zoom = gapp_widget_button_new_icon_with_label("zoom-original-symbolic", "Zoom Reset");
                    g_signal_connect(btn_reset_zoom, "clicked", G_CALLBACK(signal_toolbar_click_zoom_reset), self);
                    gtk_box_append(toolbar, btn_reset_zoom);
                }

                // viewport
                self->viewport = gapp_level_viewport_new(self);
                gtk_box_append(vbox, self->viewport);
            }

            // inspector
            self->inspector = gapp_level_inspector_new();
            gtk_widget_set_size_request(GTK_WIDGET(self->inspector), 250, -1);
            gtk_paned_set_end_child(GTK_PANED(paned2), self->inspector);
        }
    }

    g_signal_connect(self, "level-viewport-init", G_CALLBACK(signal_viewport_init), NULL);
}

static void signal_toolbar_click_save_level(GtkWidget* button, GappLevelEditor* self)
{
    ecs_world_t* world = self->world;
    const gchar* filename = self->filename;

    char* json = ecs_world_to_json(world, NULL);
    gb_fs_write_file(filename, json);
    ecs_os_free(json);

    gb_log_success(TF("Save level %s", filename));
}

static void signal_toolbar_click_zoom_reset(GtkWidget* button, GappLevelEditor* self)
{
    gb_camera_t* camera = ecs_get(self->world, ecs_lookup(self->world, "Engine"), gb_camera_t);
    camera->zoom = 1.0f;
}

static void hook_callback(ecs_iter_t* it)
{
    ecs_world_t* world = it->world;
    ecs_entity_t event = it->event;
    GappLevelEditor* self = it->ctx;

    for (int i = 0; i < it->count; i++) {
        ecs_entity_t e = it->entities[i];
        if (event == EcsOnAdd)
            gapp_level_outliner_append_entity(self->outliner, e);
        else if (event == EcsOnRemove)
            gapp_level_outliner_remove_entity(self->outliner, e);
    }
}

// Cuando el viewport esta listo, cargamos el .level en el mundo ecs
static void signal_viewport_init(GappLevelEditor* self, int width, int height, gpointer data)
{
    size_t level_buffer_size;

    self->world = gapp_gobu_embed_get_world(self->viewport);

    ecs_set_hooks(self->world, gb_transform_t, {
        .on_add = hook_callback,
        .on_remove = hook_callback,
        .ctx = self
    });

    gchar* level_buffer = gb_fs_get_contents(self->filename, &level_buffer_size);

    gb_camera_t* camera = ecs_get(self->world, ecs_lookup(self->world, "Engine"), gb_camera_t);
    camera->mode = GB_CAMERA_EDITOR;

    if (level_buffer_size > 0) {
        ecs_world_from_json(self->world, level_buffer, NULL);
        free(level_buffer);
    }

    // Buscamos o creamos una entidad principal de nombre World...
    // experimental
    self->root = ecs_lookup(self->world, GAME_ROOT_ENTITY);
    if (self->root == 0) {
        self->root = gb_ecs_entity_new(self->world, 0, GAME_ROOT_ENTITY, gb_ecs_transform(0, 0));
    }
}

GappLevelEditor* gapp_level_editor_new(const gchar* filename)
{
    GappLevelEditor* self = g_object_new(GAPP_LEVEL_TYPE_EDITOR, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
    {
        self->filename = gb_strdup(filename);
        gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 1, gb_fs_get_name(filename, FALSE), self);
    }
    return self;
}

ecs_world_t* gapp_level_editor_get_world(GappLevelEditor* self)
{
    return self->world;
}
