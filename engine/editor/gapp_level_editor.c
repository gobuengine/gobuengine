#include "gapp_level_editor.h"
#include "gapp_inspector.h"
#include "gapp_level_outliner.h"
#include "gapp_tool_console.h"
#include "gapp_widget.h"

struct _GappLevelEditor
{
    GtkWidget parent;

    GtkWidget* embed;
    GtkWidget* inspector;
    GtkWidget* outliner;

    ecs_world_t* world;
    gchar* filename;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelEditor, gapp_level_editor, GTK_TYPE_BOX);

static void finalize(GObject* object)
{
    GappLevelEditor* self = GAPP_LEVEL_EDITOR(object);

    g_free(self->filename);
    ecs_quit(self->world);

    debug_print(CONSOLE_INFO, TF("Level Editor Free [%s]\n", gb_fs_get_name(self->filename, false)));
}

static void gapp_level_editor_class_init(GappLevelEditorClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = finalize;
}

static void gapp_level_editor_init(GappLevelEditor* self)
{
}

// -------------- --------------

static void gb_editor_world_signal_start(GtkWidget* embed, GappLevelEditor* self)
{
    gobu_rendering_init(self->world,
        gapp_gobu_embed_get_width(embed),
        gapp_gobu_embed_get_height(embed),
        "LevelEditor", true);
}

static void gb_editor_world_signal_render(GtkWidget* embed, GappLevelEditor* self)
{
    gobu_rendering_progress(self->world);
}

static void gb_editor_world_signal_resize(GtkWidget* embed, int width, int height, GappLevelEditor* self)
{
    ViewportSizeCallback(width, height);
}

static void gb_editor_world_signal_drop(GtkWidget* embed, GFile* file, double x, double y, GappLevelEditor* self)
{
    gchar* filename = g_file_get_path(file);
    gchar* name = gb_fs_get_name(filename, true);


    if (gobu_resource_set(self->world, name, filename))
        debug_print(CONSOLE_INFO, gb_strdups("Resource load: %s", name));

    ecs_entity_t entity = ecs_new_id(self->world);
    ecs_set_name(self->world, entity, gb_strdups("%s%d", name, entity));
    ecs_set(self->world, entity, GPosition, { x, y });

    if (gb_fs_is_extension(filename, ".png")) {
        ecs_set(self->world, entity, GSprite, { .resource = name });
        debug_print(CONSOLE_INFO, gb_strdups("Spawn Entity [%lld], Component Sprite [%s]", entity, name));
    }
}

static void gapp_level_editor_signal_save(GtkWidget* button, GappLevelEditor* self)
{
    ecs_world_t* world = self->world;
    const gchar* filename = self->filename;

    char* json = ecs_world_to_json(world, NULL);
    gb_fs_write_file(filename, json);
    ecs_os_free(json);

    debug_print(CONSOLE_INFO, TF("Save level %s", filename));
}

static void gapp_level_querys(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    GappLevelEditor* self = it->ctx;

    for (int i = 0; i < it->count; i++)
    {
        ecs_entity_t entity = it->entities[i];

        if (event == EcsOnRemove) {
            gapp_level_outliner_remove(self->outliner, entity);
        }
        if (event == EcsOnAdd || event == 0) {
            gapp_level_outliner_append(self->outliner, ecs_get_name(it->world, entity), entity, true);
        }
        if (event == EcsOnSet) {
        }
    }
}

void gapp_level_editor_query_world(GappLevelEditor* self)
{
    ecs_world_t* world = self->world;

    ecs_query_t* q = ecs_query(world, {
        .filter.terms = {
            { ecs_id(GPosition) }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    it.ctx = self;
    while (ecs_query_next(&it)) {
        gapp_level_querys(&it);
    }

    ecs_query_fini(q);
}

// -------------- --------------

static GappLevelEditor* gapp_level_editor_template(GappLevelEditor* self)
{
    GtkWidget* paned, * paned2, * toolbar;

    // toolbar
    toolbar = gapp_widget_toolbar_new();
    gtk_box_append(self, toolbar);
    {
        GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
        g_signal_connect(btn_s, "clicked", G_CALLBACK(gapp_level_editor_signal_save), NULL);
        gtk_box_append(toolbar, btn_s);
    }

    paned = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
    gtk_box_append(self, paned);
    {
        // outliner
        {
            self->outliner = gapp_level_outliner_new();
            gtk_widget_set_size_request(GTK_WIDGET(self->outliner), 200, -1);
            gtk_paned_set_start_child(GTK_PANED(paned), self->outliner);
        }

        paned2 = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, FALSE);
        gtk_paned_set_end_child(GTK_PANED(paned), paned2);
        {
            // viewport
            self->embed = gapp_gobu_embed_new();
            gtk_paned_set_start_child(GTK_PANED(paned2), self->embed);
            g_signal_connect(self->embed, "gobu-embed-start", G_CALLBACK(gb_editor_world_signal_start), self);
            g_signal_connect(self->embed, "gobu-embed-render", G_CALLBACK(gb_editor_world_signal_render), self);
            g_signal_connect(self->embed, "gobu-embed-resize", G_CALLBACK(gb_editor_world_signal_resize), self);
            g_signal_connect(self->embed, "gobu-embed-drop", G_CALLBACK(gb_editor_world_signal_drop), self);

            // inspector
            self->inspector = gapp_inspector_new();
            gtk_widget_set_size_request(GTK_WIDGET(self->inspector), 250, -1);
            gtk_paned_set_end_child(GTK_PANED(paned2), self->inspector);
        }
    }

    return self;
}

GappLevelEditor* gapp_level_editor_new(const gchar* filename)
{
    GappLevelEditor* self = g_object_new(GAPP_LEVEL_TYPE_EDITOR, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    self->filename = gb_strdup(filename);
    char* name = gb_fs_get_name(self->filename, false);
    self->world = ecs_init();
    gobu_import_all(self->world);

    gapp_project_editor_append_page(GOBU_NOTEBOOK_DEFAULT, 1, name, gapp_level_editor_template(self));

    int size = 0;
    unsigned char* buffer = gb_fs_get_contents(self->filename, &size);

    if (size > 0) {
        ecs_world_from_json(self->world, buffer, NULL);
        free(buffer);
        gapp_level_editor_query_world(self);
        debug_print(CONSOLE_INFO, TF("Load content level %d %s", size, self->filename));
    }

    ecs_observer(self->world, {
        .filter = {.terms = { {.id = ecs_id(GPosition)} }},
        .events = { EcsOnRemove, EcsOnAdd, EcsOnSet },
        .callback = gapp_level_querys,
        .ctx = self
    });

    return self;
}
