#include "gapp_extrasprites.h"
#include "gapp_gobu_embed.h"
#include "gapp_widget.h"

#include "gb_ecs_sprite.h"
#include "gb_ecs_gizmos.h"
#include "gb_type_shape_rect.h"

enum
{
    PROP_0,
    PROP_FILENAME,
    PROP_LAST
};

static GParamSpec* obj_props[PROP_LAST];

struct _GbAppExtraSprites {
    GtkWidget parent;
    GtkWidget* viewport;
    GtkWidget* input_name;
    GtkWidget* input_index;
    GtkWidget* input_cell_width;
    GtkWidget* input_cell_height;
    GtkWidget* input_naming_index;
    GtkWidget* input_num_cell_x;
    GtkWidget* input_num_cell_y;
    GtkWidget* input_margin_x;
    GtkWidget* input_margin_y;
    GtkWidget* input_spacing_x;
    GtkWidget* input_spacing_y;
    int cell_width_default;
    int cell_height_default;
    gb_rect_t* rects;
    int index;
    gchar* filename;
    ecs_entity_t entity;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GbAppExtraSprites, gbapp_extrasprites, GTK_TYPE_WINDOW);

static void set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec)
{
    GbAppExtraSprites* self = GBAPP_EXTRASPRITE(object);

    switch (property_id)
    {
    case PROP_FILENAME:
        self->filename = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void get_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec)
{
    GbAppExtraSprites* self = GBAPP_EXTRASPRITE(object);

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

static void gbapp_extrasprites_class_init(GbAppExtraSpritesClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = set_property;
    object_class->get_property = get_property;

    obj_props[PROP_FILENAME] = g_param_spec_string("filename", "FileName", "Set file name", "", G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, PROP_LAST, obj_props);
}

// ? ------------------------- CLASS METHODS -------------------------

static void fn_update_rect(GbAppExtraSprites* self, double num_cell_x, double num_cell_y, double margin_x, double margin_y, double spacing_x, double spacing_y)
{
    ecs_world_t* world = gapp_gobu_embed_get_world(self->viewport);

    float frame_w = (float)(self->cell_width_default / num_cell_x);
    float frame_h = (float)(self->cell_height_default / num_cell_y);
    float center_x = -(self->cell_width_default / 2) + (frame_w / 2);
    float center_y = -(self->cell_height_default / 2) + (frame_h / 2);

    ecs_entity_t parent = ecs_lookup(world, "extrasprites");
    ecs_delete_children(world, parent);

    int index_name = gtk_spin_button_get_value(self->input_index);

    char* name = gapp_widget_entry_get_text(self->input_name);

    int num_cell = num_cell_x * num_cell_y;

    if (self->rects != NULL) {
        g_free(self->rects);
        self->rects = NULL;
    }

    self->rects = g_realloc(self->rects, sizeof(gb_rect_t) * num_cell);

    for (int frame_current = 0; frame_current < num_cell; frame_current++) {
        int frame_row = (self->cell_width_default / frame_w);
        float sub_x = (float)(frame_current % frame_row) * frame_w;
        float sub_y = (float)(frame_current / frame_row) * frame_h;

        self->rects[frame_current] = (gb_rect_t){ sub_x, sub_y, frame_w, frame_h };

        char* name_format = gb_str_replace(name, "{0}", gb_strdups("%d", (frame_current + index_name)));

        ecs_entity_t entity = gb_ecs_entity_new(world, parent, name_format, gb_ecs_transform(sub_x + center_x, sub_y + center_y));
        ecs_set(world, entity, gb_shape_rect_t, { .width = frame_w, .height = frame_h, .border_color = ORANGE, .border_width = 1 });
        ecs_remove(world, entity, gb_gizmos_t);
        g_free(name_format);
    }
}

static void fn_extract_sprite_from_files(ecs_world_t* world, const char* name, int index_start, GbAppExtraSprites* self)
{
    int index_name = index_start;

    // guardamos el resource una sola ve por extract..
    gb_sprite_t clip_sprite;
    clip_sprite.resource = gb_path_relative_content(self->filename);

    ecs_entity_t parent = ecs_lookup(world, "extrasprites");
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            ecs_entity_t entity = it.entities[i];

            char* name_format = gb_str_replace(name, "{0}", gb_strdups("%d", index_name));
            char* filename = gb_path_join(gb_path_dirname(self->filename), gb_strdups("%s.sprite", name_format), NULL);

            clip_sprite.src.x = self->rects[i].x;
            clip_sprite.src.y = self->rects[i].y;
            clip_sprite.src.width = self->rects[i].width;
            clip_sprite.src.height = self->rects[i].height;

            binn* fsprite = gb_sprite_serialize(clip_sprite);
            binn_deserialize_from_file(fsprite, filename);
            binn_free(fsprite);

            index_name++;

            // ecs_os_free(buffer);
            ecs_os_free(filename);
            ecs_os_free(name_format);
        }
    }
}

static void signal_viewport_start(GtkWidget* viewport, GbAppExtraSprites* self)
{
    ecs_world_t* world = gapp_gobu_embed_get_world(viewport);

    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);

    float x = width / 2;
    float y = height / 2;

    gb_camera_t* camera = ecs_get(world, ecs_lookup(world, "Engine"), gb_camera_t);
    camera->offset.x = x;
    camera->offset.y = y;

    GdkTexture* texture = gdk_texture_new_from_filename(self->filename, NULL);
    int w = gdk_texture_get_width(texture);
    int h = gdk_texture_get_height(texture);
    g_object_ref(texture);

    self->cell_width_default = w;
    self->cell_height_default = h;

    char* key = gb_resource_set(world, self->filename);
    self->entity = gb_ecs_entity_new(world, 0, "extrasprites", gb_ecs_transform(0, 0));
    ecs_remove(world, self->entity, gb_gizmos_t);
    ecs_set(world, self->entity, gb_sprite_t, { .resource = key });

    fn_update_rect(self, 1, 1, 0, 0, 0, 0);

    gb_log_info(TF("Init ExtraSprites: %s", gb_path_relative_content(self->filename)));
}

static void signal_destroy_close(GtkWidget* widget, GbAppExtraSprites* self)
{
    if (self->rects != NULL) {
        g_free(self->rects);
        self->rects = NULL;
    }

    gtk_window_close(self);
    gb_log_info(TF("Close ExtraSprites: %s", gb_path_relative_content(self->filename)));
}

static void signal_btn_extract(GtkWidget* widget, GbAppExtraSprites* self)
{
    ecs_world_t* world = gapp_gobu_embed_get_world(self->viewport);
    char* name = gapp_widget_entry_get_text(self->input_name);
    int index_name = gtk_spin_button_get_value(self->input_index);

    fn_extract_sprite_from_files(world, name, index_name, self);

    signal_destroy_close(widget, self);
}

static void input_updates(GtkSpinButton* self, GbAppExtraSprites* sprite)
{
    double num_cell_x = gtk_spin_button_get_value(sprite->input_num_cell_x);
    double num_cell_y = gtk_spin_button_get_value(sprite->input_num_cell_y);

    double margin_x = gtk_spin_button_get_value(sprite->input_margin_x);
    double margin_y = gtk_spin_button_get_value(sprite->input_margin_y);

    double spacing_x = gtk_spin_button_get_value(sprite->input_spacing_x);
    double spacing_y = gtk_spin_button_get_value(sprite->input_spacing_y);

    fn_update_rect(sprite, num_cell_x, num_cell_y, margin_x, margin_y, spacing_x, spacing_y);
}

static void gbapp_extrasprites_show(GbAppExtraSprites* self, GParamSpec* pspec, gpointer data)
{
    char* title = gb_strdups("Extra Sprites [ %s ]", gb_path_basename(self->filename));
    char* name = gb_str_remove_spaces(gb_fs_get_name(self->filename, TRUE));

    gtk_window_set_title(self, title);
    gtk_window_set_destroy_with_parent(self, TRUE);
    gtk_window_set_modal(self, TRUE);
    gtk_window_set_resizable(self, FALSE);
    gtk_window_set_default_size(self, 1000, 700);
    g_signal_connect(self, "destroy", G_CALLBACK(signal_destroy_close), self);

    GtkWidget* paned = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, FALSE);
    gtk_window_set_child(self, paned);

    self->viewport = gapp_gobu_embed_new();
    gapp_gobu_embed_set_grid(self->viewport, TRUE);
    g_signal_connect(self->viewport, "gobu-embed-start", G_CALLBACK(signal_viewport_start), self);
    gtk_paned_set_start_child(GTK_PANED(paned), self->viewport);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_paned_set_end_child(GTK_PANED(paned), box);
    gtk_widget_set_size_request(box, 300, -1);

    GtkWidget* expander, * group_size, * label, * spin_button;

    expander = gtk_expander_new("Naming");
    gtk_widget_add_css_class(expander, "expander_n");
    gtk_expander_set_expanded(expander, TRUE);
    gtk_box_append(box, expander);

    group_size = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    g_object_set_data_full(G_OBJECT(expander), "size-group", group_size, g_object_unref);
    {
        GtkWidget* box_exp = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_expander_set_child(expander, box_exp);

        self->input_name = gapp_widget_input_str(box_exp, "Naming Template", gb_strdups("%s_{0}", name));
        self->input_index = gapp_widget_input_number(box_exp, "Naming Start Index", 0.0f, 99999.0f, 1.0f);
    }

    expander = gtk_expander_new("Grid");
    gtk_widget_add_css_class(expander, "expander_n");
    gtk_expander_set_expanded(expander, TRUE);
    gtk_box_append(box, expander);
    {
        GtkWidget* box_exp = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_expander_set_child(expander, box_exp);

        // self->input_cell_width = gapp_widget_input_number(box_exp, "Cell Width", 0.0f, 99999.0f, 1.0f, &self->cell_width);
        // self->input_cell_height = gapp_widget_input_number(box_exp, "Cell Height", 0.0f, 99999.0f, 1.0f, &self->cell_height);
        self->input_num_cell_x = gapp_widget_input_number(box_exp, "Num Cells X", 1.0f, 128.0f, 1.0f);
        g_signal_connect(self->input_num_cell_x, "value-changed", G_CALLBACK(input_updates), self);

        self->input_num_cell_y = gapp_widget_input_number(box_exp, "Num Cells Y", 1.0f, 128.0f, 1.0f);
        g_signal_connect(self->input_num_cell_y, "value-changed", G_CALLBACK(input_updates), self);

        // self->input_margin_x = gapp_widget_input_number(box_exp, "Margin X", 0.0f, 99999.0f, 1.0f);
        // g_signal_connect(self->input_margin_x, "value-changed", G_CALLBACK(input_updates), self);

        // self->input_margin_y = gapp_widget_input_number(box_exp, "Margin Y", 0.0f, 99999.0f, 1.0f);
        // g_signal_connect(self->input_margin_y, "value-changed", G_CALLBACK(input_updates), self);

        // self->input_spacing_x = gapp_widget_input_number(box_exp, "Spacing X", 0.0f, 99999.0f, 1.0f);
        // g_signal_connect(self->input_spacing_x, "value-changed", G_CALLBACK(input_updates), self);

        // self->input_spacing_y = gapp_widget_input_number(box_exp, "Spacing Y", 0.0f, 99999.0f, 1.0f);
        // g_signal_connect(self->input_spacing_y, "value-changed", G_CALLBACK(input_updates), self);
    }

    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gapp_widget_set_margin(button_box, 3);
    gapp_widget_set_margin_bottom(button_box, 8);
    gtk_widget_set_hexpand(button_box, TRUE);
    gtk_widget_set_vexpand(button_box, TRUE);
    gtk_widget_set_valign(button_box, GTK_ALIGN_END);
    gtk_box_append(box, button_box);
    {
        GtkWidget* btn_extract = gapp_widget_button_new_icon_with_label("system-file-manager-symbolic", "Extract...");
        gtk_button_set_has_frame(btn_extract, TRUE);
        gtk_widget_set_hexpand(btn_extract, TRUE);
        gtk_box_append(button_box, btn_extract);
        g_signal_connect(btn_extract, "clicked", G_CALLBACK(signal_btn_extract), self);

        GtkWidget* btn_noextract = gapp_widget_button_new_icon_with_label("application-exit-symbolic", "Cancel");
        gtk_button_set_has_frame(btn_noextract, TRUE);
        gtk_widget_set_hexpand(btn_noextract, TRUE);
        gtk_box_append(button_box, btn_noextract);
        g_signal_connect(btn_noextract, "clicked", G_CALLBACK(signal_destroy_close), self);
    }

    gtk_window_present(self);
}

/**
 * Extra de un AtlasSprite varios Sprite.
 *
 * @return Un nuevo widget que ExtraSprites.
 */
static void gbapp_extrasprites_init(GbAppExtraSprites* self)
{
    g_signal_connect(self, "notify::filename", G_CALLBACK(gbapp_extrasprites_show), NULL);
}

GtkWidget* gbapp_extrasprites_new(const gchar* filename)
{
    return g_object_new(GBAPP_TYPE_EXTRASPRITE, "filename", filename, NULL);
}

