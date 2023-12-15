#include "gbapp_asheets.h"
#include "gapp_gobu_embed.h"

struct _GbAppAsheets
{
    GtkWidget parent;
    gchar* filename;
    gb_world_t* world;
    GtkStringList* list_anim;
    GtkStringList* list_frame;
    GtkWidget* list_view_frame;
    //
    binn* data;
    gchar* default_animations;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GbAppAsheets, gbapp_asheets, GTK_TYPE_BOX);

static void gbapp_asheets_class_init(GbAppAsheetsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gbapp_asheets_init(GbAppAsheets* self)
{
    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);
}

static void signal_viewport_start(GtkWidget* viewport, GbAppAsheets* asheets)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);

    asheets->world = gb_app_init(&(gb_app_t) { .width = width, .height = height, .show_grid = true });

    gb_camera_t* camera = ecs_get(asheets->world, ecs_lookup(asheets->world, "Engine"), gb_camera_t);
    camera->mode = GB_CAMERA_EDITOR;
}

static void signal_viewport_render(GtkWidget* viewport, GbAppAsheets* asheets)
{
    gb_app_progress(asheets->world);
}

static void signal_animation_list_factory_setup(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkWidget* box, * icon, * label, * box_tool, * fps, * loop;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_list_item_set_child(item, box);
    {
        icon = gtk_image_new_from_icon_name("input-dialpad-symbolic");
        gtk_box_append(box, icon);

        label = gtk_label_new(NULL);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_box_append(box, label);

        gtk_box_append(box, gapp_widget_separator_h());

        box_tool = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_widget_set_halign(box_tool, GTK_ALIGN_END);
        gapp_widget_set_margin(box_tool, 5);
        gtk_box_append(box, box_tool);

        fps = gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(fps), 12.0);
        gtk_widget_set_tooltip_text(fps, "Frames per second");
        gtk_box_append(box_tool, fps);

        loop = gtk_check_button_new_with_label("Loop");
        gtk_widget_set_tooltip_text(loop, "Loop animation");
        gtk_box_append(box_tool, loop);
    }
}

static void signal_animation_list_factory_bind(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkStringObject* string_object = gtk_list_item_get_item(item);

    GtkWidget* box = GTK_BOX(gtk_list_item_get_child(item));
    GtkWidget* icon = gtk_widget_get_first_child(box);
    GtkWidget* label = gtk_widget_get_next_sibling(icon);
    GtkWidget* fps = gtk_widget_get_next_sibling(label);
    GtkWidget* loop = gtk_widget_get_next_sibling(fps);

    gchar* anim_name = gtk_string_object_get_string(string_object);
    bool is_default = strcmp(self->default_animations, anim_name) == 0;
    if (is_default)
        gtk_image_set_from_icon_name(icon, "user-bookmarks-symbolic");

    gtk_label_set_xalign(label, 0.0);
    gtk_label_set_text(label, anim_name);
}

static void signal_frames_list_factory_setup(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkWidget* box, * icon, * label;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_list_item_set_child(item, box);
    {
        icon = gtk_image_new_from_file(NULL);
        gtk_widget_set_size_request(icon, 64, 64);
        gtk_box_append(box, icon);

        label = gtk_label_new(NULL);
        gtk_box_append(box, label);
    }
}

static void signal_frames_list_factory_bind(GtkListItemFactory* factory, GtkListItem* item, GbAppAsheets* self)
{
    GtkStringObject* string_object = gtk_list_item_get_item(item);

    GtkLabel* box = GTK_BOX(gtk_list_item_get_child(item));
    GtkWidget* icon = gtk_widget_get_first_child(box);
    GtkWidget* label = gtk_widget_get_last_child(box);

    gchar* id_r = gtk_string_object_get_string(string_object);

    // resource 
    binn* resource = binn_object_object(self->data, "resources");
    binn* resource_item = binn_object_object(resource, id_r);
    char* filename = binn_object_str(resource_item, "path");

    char* path = gb_path_join(gb_project_get_path(), "Content", filename, NULL);
    path = gb_path_normalize(path);

    gtk_image_set_from_file(GTK_IMAGE(icon), path);
    gtk_label_set_text(label, id_r);
}

static gboolean signal_list_frames_drop_file(GtkDropTarget* target, const GValue* value, double x, double y, GbAppAsheets* self)
{
    if (G_VALUE_HOLDS(value, G_TYPE_LIST_STORE))
    {
        GListStore* filess = G_LIST_STORE(g_value_get_object(value));

        guint items_n = g_list_model_get_n_items(G_LIST_MODEL(filess));
        for (int i = 0; i < items_n; i++)
        {
            GFileInfo* file_info = G_FILE_INFO(g_list_model_get_item(G_LIST_MODEL(filess), i));
            GFile* file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

            gchar* filename = g_file_get_path(file);
            if (gb_fs_is_extension(filename, ".png") || gb_fs_is_extension(filename, ".jpg")) {
                gchar* name = gb_str_remove_spaces(gb_fs_get_name(filename, true));

                gtk_string_list_append(self->list_frame, filename);
            }
        }

        return TRUE;
    }
    return FALSE;
}

static void fn_clear_string_list(GtkStringList* list)
{
    gtk_string_list_splice(list, 0, g_list_model_get_n_items(list), NULL);
}

static void fn_load_list_animations(GbAppAsheets* self)
{
    binn* animations = binn_object_list(self->data, "animations");
    binn_iter iter;
    binn value;
    char key[256];

    fn_clear_string_list(self->list_anim);

    binn_object_foreach(animations, key, value)
    {
        gtk_string_list_append(self->list_anim, key);
    }
}

static void fn_frames_for_selected_animation(GbAppAsheets* self, const gchar* name)
{
    binn* animations = binn_object_list(self->data, "animations");
    binn* animation = binn_object_object(animations, name);

    int fps = binn_object_int32(animation, "fps");
    bool loop = binn_object_bool(animation, "loop");
    binn* frames = binn_object_list(animation, "frames");

    fn_clear_string_list(self->list_frame);

    int count = binn_count(frames);
    for (int i = 1; i <= count; i++)
    {
        binn* frame_props = binn_list_object(frames, i);
        double fps = binn_object_double(frame_props, "duration");
        char* id = binn_object_str(frame_props, "id");

        gtk_string_list_append(self->list_frame, gb_strdup(id));
    }
}

static GbAppAsheets* gbapp_asheets_template(GbAppAsheets* self)
{
    GtkWidget* scroll, * vbox_anim, * viewport, * toolbar, * paned_main, * paned2, * vbox_anims, * vbox_frames_anims;

    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);

    toolbar = gapp_widget_toolbar_new();
    gtk_box_append(self, toolbar);
    {
        GtkWidget* btn_s = gapp_widget_button_new_icon_with_label("media-floppy-symbolic", "Save");
        // g_signal_connect(btn_s, "clicked", G_CALLBACK(signal_toolbar_click_save_level), self);
        gtk_box_append(toolbar, btn_s);
    }

    paned_main = gapp_widget_paned_new(GTK_ORIENTATION_VERTICAL, FALSE);
    gtk_widget_set_size_request(paned_main, -1, 250);
    gtk_box_append(self, paned_main);
    {
        viewport = gapp_gobu_embed_new();
        g_signal_connect(viewport, "gobu-embed-start", G_CALLBACK(signal_viewport_start), self);
        g_signal_connect(viewport, "gobu-embed-render", G_CALLBACK(signal_viewport_render), self);
        gtk_paned_set_start_child(GTK_PANED(paned_main), viewport);


        // --------------------
        // ?List Animations
        // --------------------
        vbox_anims = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        {
            toolbar = gapp_widget_toolbar_new();
            gtk_box_append(vbox_anims, toolbar);
            {
                GtkWidget* btn_n = gapp_widget_button_new_icon_with_label("document-new-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_n, "New animation");
                gtk_box_append(toolbar, btn_n);

                GtkWidget* btn_r = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_r, "Remove animation");
                gtk_box_append(toolbar, btn_r);

                GtkWidget* btn_d = gapp_widget_button_new_icon_with_label("bookmark-new-symbolic", "Default");
                gtk_widget_set_tooltip_text(btn_d, "Set as default animation");
                gtk_box_append(toolbar, btn_d);
            }

            GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_animation_list_factory_setup), self);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_animation_list_factory_bind), self);

            GtkWidget* list_view_anim = gtk_list_view_new(gtk_single_selection_new(self->list_anim), factory);
            gtk_widget_add_css_class(list_view_anim, "navigation-sidebar");

            scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view_anim);
            gtk_widget_set_vexpand(scroll, TRUE);
            gtk_box_append(vbox_anims, scroll);
        }

        // --------------------
        // ?List frames
        // --------------------
        vbox_frames_anims = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        {
            toolbar = gapp_widget_toolbar_new();
            gtk_box_append(vbox_frames_anims, toolbar);
            {
                GtkWidget* btn_cframe = gapp_widget_button_new_icon_with_label("edit-copy-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_cframe, "Copy Frame");
                gtk_box_append(toolbar, btn_cframe);

                GtkWidget* btn_pframe = gapp_widget_button_new_icon_with_label("edit-paste-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_pframe, "Paste Frame");
                gtk_box_append(toolbar, btn_pframe);

                gtk_box_append(toolbar, gapp_widget_separator_h());

                GtkWidget* btn_ml = gapp_widget_button_new_icon_with_label("go-previous-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_ml, "Move frame left");
                gtk_box_append(toolbar, btn_ml);

                GtkWidget* btn_mr = gapp_widget_button_new_icon_with_label("go-next-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_mr, "Move frame right");
                gtk_box_append(toolbar, btn_mr);

                GtkWidget* btn_r = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
                gtk_widget_set_tooltip_text(btn_r, "Remove frame");
                gtk_box_append(toolbar, btn_r);

                gtk_box_append(toolbar, gapp_widget_separator_h());

                gtk_box_append(toolbar, gtk_label_new("Frame Duration:"));

                GtkWidget *duration = gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(duration), 1.0);
                gtk_widget_set_tooltip_text(duration, "Frame Duration");
                gtk_box_append(toolbar, duration);
            }

            GtkListItemFactory* factory = gtk_signal_list_item_factory_new();
            g_signal_connect(factory, "setup", G_CALLBACK(signal_frames_list_factory_setup), self);
            g_signal_connect(factory, "bind", G_CALLBACK(signal_frames_list_factory_bind), self);

            self->list_view_frame = gtk_grid_view_new(gtk_single_selection_new(self->list_frame), factory);
            gtk_widget_add_css_class(self->list_view_frame, "navigation-sidebar");
            gtk_grid_view_set_max_columns(self->list_view_frame, 15);
            gtk_grid_view_set_min_columns(self->list_view_frame, 2);

            scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->list_view_frame);
            gtk_widget_set_vexpand(scroll, TRUE);
            gtk_box_append(vbox_frames_anims, scroll);

            GtkDropTarget* target = gtk_drop_target_new(G_TYPE_LIST_STORE, GDK_ACTION_COPY);
            g_signal_connect(target, "drop", G_CALLBACK(signal_list_frames_drop_file), self);
            gtk_widget_add_controller(self->list_view_frame, GTK_EVENT_CONTROLLER(target));
        }

        paned2 = gapp_widget_paned_new_with_notebook(GTK_ORIENTATION_HORIZONTAL, TRUE,
            gtk_label_new("Animation"), vbox_anims,
            gtk_label_new("Frames"), vbox_frames_anims);
        gtk_paned_set_end_child(GTK_PANED(paned_main), paned2);
    }

    return self;
}



/**
 * Crea un nuevo Animation Sprite Sheets.
 *
 * @return Un nuevo widget que Animation Sprite Sheets.
 */
GtkWidget* gbapp_asheets_new(const gchar* filename)
{
    GbAppAsheets* self = g_object_new(GBAPP_TYPE_ASHEETS, "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    self->filename = gb_strdup(filename);
    self->data = binn_serialize_from_file(filename);
    char* name = gb_fs_get_name(filename, false);

    self->list_anim = gtk_string_list_new(NULL);
    self->list_frame = gtk_string_list_new(NULL);

    self->default_animations = gb_strdup(binn_object_str(self->data, "default"));

    fn_load_list_animations(self);
    fn_frames_for_selected_animation(self, self->default_animations);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 0, name, gbapp_asheets_template(self));

    return self;
}

