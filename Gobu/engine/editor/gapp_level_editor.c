#include "gapp_level_editor.h"
#include "gapp_level_inspector.h"
#include "gapp_level_outliner.h"
#include "gapp_level_viewport.h"
#include "gapp_tool_console.h"
#include "gapp_widget.h"

struct _GappLevelEditor
{
    GtkWidget parent;

    GappLevelViewport* viewport;
    GappLevelInspector* inspector;
    GappLevelOutliner* outliner;

    ecs_world_t* world; // world ecs
    ecs_entity_t root;  // entity root level
    // level data
    gchar* filename;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelEditor, gapp_level_editor, GTK_TYPE_BOX);

static void signal_toolbar_click_save(GtkWidget* button, GappLevelEditor* self);
static void signal_observer_state_world(ecs_iter_t* it);
static void signal_viewport_init(GappLevelEditor* self, gpointer data);

static void gapp_level_editor_class_finalize(GObject* object)
{
    GappLevelEditor* self = GAPP_LEVEL_EDITOR(object);

    debug_print(CONSOLE_INFO, TF("Level Editor Free [%s]\n", gb_fs_get_name(self->filename, false)));

    // ecs_quit(self->world);
    g_free(self->filename);
}

static void gapp_level_editor_class_init(GappLevelEditorClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = gapp_level_editor_class_finalize;

    g_signal_new("level-viewport-init", G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static void gapp_level_editor_init(GappLevelEditor* self)
{
    GtkWidget* paned, * paned2, * toolbar;

    // toolbar
    toolbar = gapp_widget_toolbar_new();
    gtk_box_append(self, toolbar);
    {
        GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
        g_signal_connect(btn_s, "clicked", G_CALLBACK(signal_toolbar_click_save), self);
        gtk_box_append(toolbar, btn_s);
    }

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
            // viewport
            self->viewport = gapp_level_viewport_new(self);
            gtk_paned_set_start_child(GTK_PANED(paned2), self->viewport);

            // inspector
            self->inspector = gapp_level_inspector_new();
            gtk_widget_set_size_request(GTK_WIDGET(self->inspector), 250, -1);
            gtk_paned_set_end_child(GTK_PANED(paned2), self->inspector);
        }
    }

    g_signal_connect(self, "level-viewport-init", G_CALLBACK(signal_viewport_init), NULL);
}

static void signal_toolbar_click_save(GtkWidget* button, GappLevelEditor* self)
{
    ecs_world_t* world = self->world;
    const gchar* filename = self->filename;

    char* json = ecs_world_to_json(world, NULL);
    gb_fs_write_file(filename, json);
    ecs_os_free(json);

    debug_print(CONSOLE_INFO, TF("Save level %s", filename));
}

static void signal_observer_state_world(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    GappLevelEditor* self = it->ctx;

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];
        gapp_level_outliner_events(self->outliner, event, entity, it->world);
    }
}

// Cuando el viewport esta listo, cargamos el .level en el mundo ecs
static void signal_viewport_init(GappLevelEditor* self, gpointer data)
{
    size_t level_buffer_size;

    // cargamos el archivo .level
    unsigned char* level_buffer = LoadFileData(self->filename, &level_buffer_size);

    GCamera* camera = ecs_get(self->world, ecs_lookup(self->world, "Engine"), GCamera);
    camera->mode = CAMERA_EDITOR;

    if (level_buffer_size > 0) {
        ecs_world_from_json(self->world, level_buffer, NULL);
        free(level_buffer);
    }

    // Buscamos o creamos una entidad principal de nombre World...
    self->root = ecs_lookup(self->world, "World");
    if (self->root == 0) {
        self->root = ecs_new_entity(self->world, "World");
    }

    // Creamos la entidad World en el Outliner que fueron cargadas
    // en el .level
    gapp_level_outliner_init_root(self->outliner, self->root);

    ecs_observer(self->world, {
        .filter = {.terms = { {.id = ecs_id(GPosition)} }},
        .events = { EcsOnRemove, EcsOnAdd, EcsOnSet },
        .callback = signal_observer_state_world,
        .ctx = self
    });
}


// // 
// static void gapp_level_outliner_init(ecs_world_t* world, ecs_entity_t root)
// {
//     // ecs_iter_t it = ecs_children(world, root);
//     // while (ecs_children_next(&it)) {
//     //     signal_observer_state_world(&it);
//     // }

//     // ecs_query_t* q = ecs_query(world, {
//     //     .filter.terms = {
//     //         { ecs_id(GPosition) },
//     //         {.id = ecs_pair(EcsChildOf, root)}
//     //     }
//     // });

//     // ecs_iter_t it = ecs_query_iter(world, q);
//     // while (ecs_query_next(&it)) {
//     //     signal_observer_state_world(&it);
//     // }

//     // ecs_query_fini(q);
// }

// -------------- --------------

GappLevelEditor* gapp_level_editor_new(const gchar* filename)
{
    GappLevelEditor* self = g_object_new(GAPP_LEVEL_TYPE_EDITOR, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
    {
        self->filename = gb_strdup(filename);
        // creamos el mundo ecs
        self->world = ecs_init();
        gobu_import_all(self->world);

        gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 1, gb_fs_get_name(filename, FALSE), self);
    }
    return self;
}

ecs_world_t* gapp_level_editor_get_world(GappLevelEditor* self)
{
    return self->world;
}

