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
    GtkWidget* input_cell_width;
    GtkWidget* input_cell_height;
    gint cell_width;
    gint cell_height;
    gb_texture_t texture;
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

static void signal_viewport_start(GtkWidget* viewport, GbAppExtraSprites* self)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);

    float x = width / 2;
    float y = height / 2;

    gb_world_t* world = gapp_gobu_embed_get_world(viewport);

    char* key = gb_resource_set(world, self->filename);
    self->entity = gb_ecs_entity_new(world, 0, "extrasprites", gb_ecs_transform(x, y));
    gb_ecs_entity_set(world, self->entity, gb_sprite_t, { .resource = key });

    GdkTexture* texture = gdk_texture_new_from_filename(self->filename, NULL);
    int w = gdk_texture_get_width(texture);
    int h = gdk_texture_get_height(texture);

    gtk_spin_button_set_value (self->input_cell_width, w);
    gtk_spin_button_set_value (self->input_cell_height, h);

    g_object_ref(texture);
}

static void gbapp_extrasprites_fini(GtkWidget* widget, int response, GbAppExtraSprites* self)
{
    if (response == GTK_RESPONSE_OK)
    {

    }

    g_free(self);
    gtk_window_destroy(widget);
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

        spin_button = gapp_widget_input_str(box_exp, "Naming Template", gb_strdups("%s_{0}", name));
        spin_button = gapp_widget_input_number(box_exp, "Naming Start Index", 0.0f, 99999.0f, 1.0f);
    }

    expander = gtk_expander_new("Grid");
    gtk_widget_add_css_class(expander, "expander_n");
    gtk_expander_set_expanded(expander, TRUE);
    gtk_box_append(box, expander);
    {
        GtkWidget* box_exp = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_expander_set_child(expander, box_exp);

        self->input_cell_width = gapp_widget_input_number(box_exp, "Cell Width", 0.0f, 99999.0f, 1.0f);
        self->input_cell_height = gapp_widget_input_number(box_exp, "Cell Height", 0.0f, 99999.0f, 1.0f);
        spin_button = gapp_widget_input_number(box_exp, "Num Cells X", 0.0f, 99999.0f, 1.0f);
        spin_button = gapp_widget_input_number(box_exp, "Num Cells Y", 0.0f, 99999.0f, 1.0f);
        spin_button = gapp_widget_input_number(box_exp, "Margin X", 0.0f, 99999.0f, 1.0f);
        spin_button = gapp_widget_input_number(box_exp, "Margin Y", 0.0f, 99999.0f, 1.0f);
        spin_button = gapp_widget_input_number(box_exp, "Spacing X", 0.0f, 99999.0f, 1.0f);
        spin_button = gapp_widget_input_number(box_exp, "Spacing Y", 0.0f, 99999.0f, 1.0f);

    }

    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gapp_widget_set_margin(button_box, 3);
    gtk_widget_set_hexpand(button_box, TRUE);
    gtk_widget_set_vexpand(button_box, TRUE);
    gtk_widget_set_valign(button_box, GTK_ALIGN_END);
    gtk_box_append(box, button_box);
    {
        GtkWidget* btn_extract = gapp_widget_button_new_icon_with_label("system-file-manager-symbolic", "Extract...");
        gtk_button_set_has_frame(btn_extract, TRUE);
        gtk_widget_set_hexpand(btn_extract, TRUE);
        gtk_box_append(button_box, btn_extract);

        GtkWidget* btn_noextract = gapp_widget_button_new_icon_with_label("application-exit-symbolic", "Cancel");
        gtk_button_set_has_frame(btn_noextract, TRUE);
        gtk_widget_set_hexpand(btn_noextract, TRUE);
        gtk_box_append(button_box, btn_noextract);
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

