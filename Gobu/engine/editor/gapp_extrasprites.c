#include "gapp_extrasprites.h"
#include "gapp_gobu_embed.h"
#include "gapp_widget.h"

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
    double cell_width_default;
    double cell_height_default;
    gb_rect_t rects[100];
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

static void fn_update_rect(GbAppExtraSprites* self, double num_cell_x, double num_cell_y, double margin_x, double margin_y, double spacing_x, double spacing_y)
{
    gb_world_t* world = gapp_gobu_embed_get_world(self->viewport);

    float w = (float)(self->cell_width_default / num_cell_x);
    float h = (float)(self->cell_height_default / num_cell_y);
    float x_o = -(self->cell_width_default / 2) + (w / 2);
    float y_o = -(self->cell_height_default / 2) + (h / 2);
    self->index = 0;

    int index_name = gtk_spin_button_get_value(self->input_index);

    char* name = gapp_widget_entry_get_text(self->input_name);

    char* namee = gb_str_replace(name, "{0}", "");

    ecs_entity_t parent = ecs_lookup(world, "extrasprites");
    ecs_delete_children(world, parent);

    for (int i = 0; i < num_cell_x; i++) {
        for (int n = 0; n < num_cell_y; n++) {
            float x = -(x_o + w * i);
            float y = -(y_o + h * n);

            self->rects[self->index] = (gb_rect_t){ x, y, w, h };

            char *real_name = gb_strdups("%s%d",namee,self->index);
            ecs_entity_t entity = gb_ecs_entity_new(world, 0, real_name, gb_ecs_transform(x, y));
            gb_ecs_entity_set_parent(world, parent, entity);
            gb_ecs_entity_set(world, entity, gb_shape_rect_t, { .width = w, .height = h, .color_line = ORANGE, .line_width = 1 });
            self->index++;
        }
    }
}

static void signal_viewport_start(GtkWidget* viewport, GbAppExtraSprites* self)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);

    float x = width / 2;
    float y = height / 2;

    gb_world_t* world = gapp_gobu_embed_get_world(viewport);

    gb_camera_t* camera = ecs_get(world, ecs_lookup(world, "Engine"), gb_camera_t);
    camera->offset.x = x;
    camera->offset.y = y;

    GdkTexture* texture = gdk_texture_new_from_filename(self->filename, NULL);
    int w = gdk_texture_get_width(texture);
    int h = gdk_texture_get_height(texture);
    g_object_ref(texture);

    self->cell_width_default = w;
    self->cell_height_default = h;

    // gtk_spin_button_set_value(self->input_cell_width, w);
    // gtk_spin_button_set_value(self->input_cell_height, h);

    char* key = gb_resource_set(world, self->filename);
    self->entity = gb_ecs_entity_new(world, 0, "extrasprites", gb_ecs_transform(0, 0));
    gb_ecs_entity_set(world, self->entity, gb_sprite_t, { .resource = key });

    fn_update_rect(self, 1, 1, 0, 0, 0, 0);
}

static void signal_close(GtkWidget* widget, GbAppExtraSprites* self)
{
    gtk_window_close(self);
}

static void signal_extract(GtkWidget* widget, GbAppExtraSprites* self)
{
    gb_world_t* world = gapp_gobu_embed_get_world(self->viewport);

    char* name = gapp_widget_entry_get_text(self->input_name);
    int index_name = gtk_spin_button_get_value(self->input_index);

    ecs_entity_t parent = ecs_lookup(world, "extrasprites");

    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            ecs_entity_t entity = it.entities[i];

            char* name_format = gb_str_replace(name, "{0}", gb_strdups("%d", index_name));
            char* filename = gb_path_join(gb_path_dirname(self->filename), gb_strdups("%s.sprite", name_format), NULL);

            char* buffer = ecs_entity_to_json(world, entity, &(ecs_entity_to_json_desc_t){
                .serialize_values = true,
                .serialize_path = true,
                .serialize_id_labels = true
            });
            // gb_fs_write_file(filename, buffer);
            index_name++;

            ecs_os_free(buffer);
            ecs_os_free(filename);
            ecs_os_free(name_format);
        }
    }

    signal_close(widget, self);
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
        self->input_num_cell_x = gapp_widget_input_number(box_exp, "Num Cells X", 1.0f, 99999.0f, 1.0f);
        g_signal_connect(self->input_num_cell_x, "value-changed", G_CALLBACK(input_updates), self);

        self->input_num_cell_y = gapp_widget_input_number(box_exp, "Num Cells Y", 1.0f, 99999.0f, 1.0f);
        g_signal_connect(self->input_num_cell_y, "value-changed", G_CALLBACK(input_updates), self);

        self->input_margin_x = gapp_widget_input_number(box_exp, "Margin X", 0.0f, 99999.0f, 1.0f);
        g_signal_connect(self->input_margin_x, "value-changed", G_CALLBACK(input_updates), self);

        self->input_margin_y = gapp_widget_input_number(box_exp, "Margin Y", 0.0f, 99999.0f, 1.0f);
        g_signal_connect(self->input_margin_y, "value-changed", G_CALLBACK(input_updates), self);

        self->input_spacing_x = gapp_widget_input_number(box_exp, "Spacing X", 0.0f, 99999.0f, 1.0f);
        g_signal_connect(self->input_spacing_x, "value-changed", G_CALLBACK(input_updates), self);

        self->input_spacing_y = gapp_widget_input_number(box_exp, "Spacing Y", 0.0f, 99999.0f, 1.0f);
        g_signal_connect(self->input_spacing_y, "value-changed", G_CALLBACK(input_updates), self);
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
        g_signal_connect(btn_extract, "clicked", G_CALLBACK(signal_extract), self);

        GtkWidget* btn_noextract = gapp_widget_button_new_icon_with_label("application-exit-symbolic", "Cancel");
        gtk_button_set_has_frame(btn_noextract, TRUE);
        gtk_widget_set_hexpand(btn_noextract, TRUE);
        gtk_box_append(button_box, btn_noextract);
        g_signal_connect(btn_noextract, "clicked", G_CALLBACK(signal_close), self);
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

