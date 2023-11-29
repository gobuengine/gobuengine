#include "gapp_settings.h"
#include "gapp_main.h"
#include "gapp_widget.h"
#include "gobu_project.h"
#include "gobu_utility.h"

extern GAPP* EditorCore;

struct _GappSettings
{
    GtkWidget parent;
    GtkWidget* paned;
    GtkSizeGroup* group_size;
    GtkWidget* setting_props;

    GtkWidget* thumbanil;
};

typedef enum _GappSettingsType
{
    GAPP_SETTINGS_PROJECTS = 0,
    GAPP_SETTINGS_GAME_INFO,
    GAPP_SETTINGS_DISPLAY
} GappSettingsType;

typedef enum _GappSettingsGameInfo
{
    GAPP_SETTINGS_GAME_INFO_NAME = 0,
    GAPP_SETTINGS_GAME_INFO_DESCRIPTION,
    GAPP_SETTINGS_GAME_INFO_AUTHOR,
    GAPP_SETTINGS_GAME_INFO_LICENSE,
    GAPP_SETTINGS_GAME_INFO_VERSION,
    GAPP_SETTINGS_GAME_INFO_THUMBNAIL
} GappSettingsGameInfo;

G_DEFINE_TYPE_WITH_PRIVATE(GappSettings, gapp_settings, GTK_TYPE_WIDGET);

static GtkWidget* gapp_setting_game_info_new(GappSettings* self);
static GtkWidget* gapp_setting_display_new(void);

static void gapp_settings_class_finalize(GObject* object)
{
    GappSettings* setting = GAPP_SETTINGS(object);

    g_clear_pointer(&setting->paned, gtk_widget_unparent);

    G_OBJECT_CLASS(gapp_settings_parent_class)->finalize(object);
}


static void gapp_settings_class_init(GappSettingsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(klass);

    object_class->finalize = gapp_settings_class_finalize;

    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
}

//  -------------------------

static GtkWidget* gapp_setting_group_title_new(const gchar* title, GtkWidget* box)
{
    GtkWidget* row = gtk_list_box_row_new();
    gtk_list_box_append(GTK_LIST_BOX(box), row);

    GtkWidget* label = gtk_label_new(gb_strdups("<b><big>%s</big></b>", title));
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(label, 10);
    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_list_box_row_set_child(row, label);

    return row;
}

static GtkWidget* gapp_setting_group_item_child_new(const gchar* title)
{
    GtkWidget* row = gtk_list_box_row_new();
    {
        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_list_box_row_set_child(row, box);

        GtkWidget* image = gtk_image_new_from_icon_name("view-list-symbolic");
        gtk_box_append(GTK_BOX(box), image);

        GtkWidget* label = gtk_label_new(title);
        gtk_widget_set_margin_start(label, 10);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(box), label);
    }

    return row;
}

static void signal_selected_setting(GtkListBox* listbox, GtkListBoxRow* row, GappSettings* self)
{
    GtkListBox* list = GTK_LIST_BOX(self->setting_props);
    gtk_list_box_remove_all(GTK_LIST_BOX(list));

    if (gtk_list_box_row_get_index(row) == GAPP_SETTINGS_GAME_INFO)
    {
        GtkWidget* gameInfo = gapp_setting_game_info_new(self);
        gtk_list_box_append(GTK_LIST_BOX(list), gameInfo);
    }
    else if (gtk_list_box_row_get_index(row) == GAPP_SETTINGS_DISPLAY)
    {
        GtkWidget* display = gapp_setting_display_new();
        gtk_list_box_append(GTK_LIST_BOX(list), display);
    }
    else if (gtk_list_box_row_get_index(row) == GAPP_SETTINGS_PROJECTS)
    {
        GtkWidget* gameInfo = gapp_setting_game_info_new(self);
        gtk_list_box_append(GTK_LIST_BOX(list), gameInfo);

        GtkWidget* display = gapp_setting_display_new();
        gtk_list_box_append(GTK_LIST_BOX(list), display);
    }
}

static void response_selected_thumbnail_ready(GtkFileDialog* source, GAsyncResult* res, GappSettings* ctx)
{
    GFile* file;
    GError* error = NULL;

    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, &error);

    if (error != NULL)
        g_error_free(error);
    g_return_if_fail(file);

    char* path_str = g_file_get_path(file);
    gtk_image_set_from_file(GTK_IMAGE(ctx->thumbanil), path_str);

    GError* err = NULL;
    GFile* file_src = g_file_new_for_path(path_str);
    GFile* file_dest = g_file_new_build_filename(gb_project_get_path(), "Saved", "thumbnail.png", NULL);

    if (gb_fs_copyc(file_src, file_dest, &err))
        g_info("GOBU-APP: Thumbnail copied\n");
    else
        g_error("%s", err->message);

    g_object_unref(file_src);
    g_object_unref(file_dest);

    g_free(path_str);
}

void signal_file_dialog_selected_thumbnail(GtkWidget* widget, GappSettings* ctx)
{
    GtkFileDialog* file_dialog;
    file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(GTK_FILE_DIALOG(file_dialog),
        GTK_WINDOW(EditorCore->window), NULL, response_selected_thumbnail_ready, ctx);
}

static void signal_changed_input_str(GObject* object, GappSettingsGameInfo type)
{
    const gchar* text;

    if (type == GAPP_SETTINGS_GAME_INFO_DESCRIPTION)
        text = gapp_widget_view_get_text(GTK_TEXT_BUFFER(object));
    else
        text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(object)));

    g_return_if_fail(text);

    switch (type)
    {
    case GAPP_SETTINGS_GAME_INFO_NAME:
        gb_project_config_set_name(text);
        break;
    case GAPP_SETTINGS_GAME_INFO_DESCRIPTION:
        gb_project_config_set_description(text);
        break;
    case GAPP_SETTINGS_GAME_INFO_AUTHOR:
        gb_project_config_set_author(text);
        break;
    case GAPP_SETTINGS_GAME_INFO_LICENSE:
        gb_project_config_set_license(text);
        break;
    case GAPP_SETTINGS_GAME_INFO_VERSION:
        gb_project_config_set_version(text);
        break;
    default:
        break;
    }

    gb_project_config_save();
}

static void gapp_settings_init(GappSettings* self)
{
    GtkWidget* scroll, * vbox, * list_setting, * list_setting_props;

    // paned para la lista de settings y propiedades
    self->paned = gapp_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
    gtk_widget_set_parent(self->paned, self);

    // paned 1 - lista de settings
    {
        // Lista de settings
        list_setting = gtk_list_box_new();
        gtk_widget_set_size_request(GTK_WIDGET(list_setting), 200, -1);
        gapp_widget_set_margin_top(list_setting, 20);
        gtk_widget_add_css_class(list_setting, "navigation-sidebar");
        g_signal_connect(list_setting, "row-activated", G_CALLBACK(signal_selected_setting), self);
        // gtk_widget_set_halign(list_setting, GTK_ALIGN_CENTER);
        gtk_paned_set_start_child(GTK_PANED(self->paned), list_setting);
        {
            gapp_setting_group_title_new("Project", GTK_LIST_BOX(list_setting));
            // g_signal_connect(row, "activate", G_CALLBACK(handler), self);
            {
                GtkWidget* gameInfo = gapp_setting_group_item_child_new("Game Info");
                // g_signal_connect(row, "activate", G_CALLBACK(handler), self);
                gtk_list_box_append(GTK_LIST_BOX(list_setting), gameInfo);

                GtkWidget* display = gapp_setting_group_item_child_new("Display");
                gtk_list_box_append(GTK_LIST_BOX(list_setting), display);
            }
        }
    }

    // paned 2 - lista de propiedades seleccionada
    {
        // Lista de propiedades
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_paned_set_end_child(GTK_PANED(self->paned), scroll);
        {
            // Gtk box para la lista de propiedades
            self->setting_props = gtk_list_box_new();
            gtk_list_box_set_selection_mode(GTK_LIST_BOX(self->setting_props), GTK_SELECTION_NONE);
            gtk_list_box_set_show_separators(GTK_LIST_BOX(self->setting_props), TRUE);
            gtk_widget_set_size_request(GTK_WIDGET(self->setting_props), 800, -1);
            gtk_widget_set_halign(self->setting_props, GTK_ALIGN_CENTER);
            gtk_widget_add_css_class(self->setting_props, "settings-list-active");
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), self->setting_props);

            GtkWidget* gameInfo = gapp_setting_game_info_new(self);
            gtk_list_box_append(self->setting_props, gameInfo);

            GtkWidget* display = gapp_setting_display_new();
            gtk_list_box_append(self->setting_props, display);
        }
    }
}

GappSettings* gapp_settings_new(void)
{
    GappSettings* setting = g_object_new(GAPP_TYPE_SETTINGS, NULL);

    gapp_project_editor_append_page(GAPP_NOTEBOOK_DEFAULT, 1, "World Settings", setting);

    return setting;
}

static GtkWidget* gapp_setting_game_info_new(GappSettings* self)
{
    GtkWidget* name, * description, * author,
        * text_view_frame, * box, * expand, * iframe, * button;

    expand = gapp_widget_expander_with_widget("<b>Game Info</b>", NULL);
    gapp_widget_set_margin_start(expand, 20);
    gapp_widget_set_margin_end(expand, 20);
    gapp_widget_set_margin_bottom(expand, 20);
    {
        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_expander_set_child(GTK_EXPANDER(expand), box);

        // project thumbnail
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_halign(hbox, GTK_ALIGN_START);
        gapp_widget_set_margin_top(hbox, 10);
        gtk_box_append(GTK_BOX(box), hbox);
        {
            iframe = gtk_frame_new("Thumbnail");
            gtk_widget_set_size_request(GTK_WIDGET(iframe), 128, 128);
            gtk_widget_set_halign(iframe, GTK_ALIGN_CENTER);
            gtk_widget_set_margin_end(iframe, 10);
            gtk_box_append(GTK_BOX(hbox), iframe);
            {
                self->thumbanil = gtk_image_new_from_file(gb_path_join(gb_project_get_path(), "Saved", "thumbnail.png", NULL));
                gtk_widget_set_size_request(self->thumbanil, 128, 128);
                gtk_widget_set_halign(self->thumbanil, GTK_ALIGN_CENTER);
                gapp_widget_set_margin_top(self->thumbanil, 10);
                gtk_frame_set_child(GTK_FRAME(iframe), self->thumbanil);
            }

            button = gapp_widget_button_new_icon_with_label("camera-photo-symbolic", "Change");
            gapp_widget_set_margin_top(button, 10);
            gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
            gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(hbox), button);
            g_signal_connect(button, "clicked", G_CALLBACK(signal_file_dialog_selected_thumbnail), self);
        }

        // Nombre del juego
        name = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(name), "Game Name");
        gapp_widget_entry_set_text(name, gb_project_config_get_name());
        gapp_widget_set_margin_top(name, 10);
        gtk_widget_set_hexpand(name, TRUE);
        gtk_box_append(GTK_BOX(box), name);
        g_signal_connect(name, "changed", G_CALLBACK(signal_changed_input_str), GAPP_SETTINGS_GAME_INFO_NAME);

        // Description del juego
        text_view_frame = gtk_frame_new("Description");
        gtk_widget_set_size_request(GTK_WIDGET(text_view_frame), -1, 100);
        gapp_widget_set_margin_top(text_view_frame, 10);
        gtk_box_append(GTK_BOX(box), text_view_frame);
        {
            description = gtk_text_view_new();
            gtk_frame_set_child(GTK_FRAME(text_view_frame), description);
            gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(description), GTK_WRAP_WORD_CHAR);
            gtk_text_view_set_monospace(GTK_TEXT_VIEW(description), TRUE);
            gapp_widget_view_set_text(description, gb_project_config_get_description());
            // gapp_widget_set_margin(description, 10);
            gtk_text_view_set_right_margin (GTK_TEXT_VIEW(description), 10);
            gtk_text_view_set_left_margin (GTK_TEXT_VIEW(description), 10);
            gtk_text_view_set_top_margin (GTK_TEXT_VIEW(description), 10);
            g_signal_connect(gtk_text_view_get_buffer(description), "changed", G_CALLBACK(signal_changed_input_str), GAPP_SETTINGS_GAME_INFO_DESCRIPTION);
        }

        // Author del juego
        author = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(author), "Author");
        gapp_widget_entry_set_text(author, gb_project_config_get_author());
        gapp_widget_set_margin_top(author, 10);
        gtk_widget_set_hexpand(author, TRUE);
        gtk_box_append(GTK_BOX(box), author);
        g_signal_connect(author, "changed", G_CALLBACK(signal_changed_input_str), GAPP_SETTINGS_GAME_INFO_AUTHOR);
    }

    return expand;
}

static GtkWidget* gapp_setting_display_new(void)
{
        // Resolucion common
    const gchar* resolution[] = { "Common Resolutions", "640x480", "800x600", "1024x768", "1280x720", "1280x1024", "1366x768 ( Generic 14-15.6\" Notebook )", "1440x900 (19\" Monitor:16:10 )", "1600x900 (20\" Monitor:16:9 )", "1680x1050 (22\" Monitor:16:10 )", "1920x1080 (21.5-24\" Monitor:16:9 )", "1920x1200", "2560x1440 (27\" Monitor:16:9 )", "2560x1600", "3840x2160", "4096x2160", NULL };

    GtkWidget* expand, * vbox_main, * combo, * button, * check, * hbox, * spin, * label, * vbox, * bbox;

    bbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

    expand = gapp_widget_expander_with_widget("<b>Resolution</b>", NULL);
    gapp_widget_set_margin_start(expand, 20);
    gapp_widget_set_margin_end(expand, 20);
    gtk_box_append(GTK_BOX(bbox), expand);
    {
        vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_hexpand(vbox_main, TRUE);
        gtk_expander_set_child(GTK_EXPANDER(expand), vbox_main);

        hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
        gtk_box_append(GTK_BOX(vbox_main), hbox);
        {
            combo = gtk_combo_box_text_new();
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
            gtk_widget_set_hexpand(combo, TRUE);
            gapp_widget_set_margin_top(combo, 10);
            gtk_widget_set_tooltip_text(combo, "Pick from a list of common screen resolutions");
            gtk_box_append(GTK_BOX(hbox), combo);

            // leemos el array de resoluciones
            for (int i = 0; resolution[i] != NULL; i++)
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), resolution[i]);

            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

            // button para invertir la resolucion
            button = gapp_widget_button_new_icon_with_label("go-up-symbolic", NULL);
            gtk_button_set_has_frame(button, TRUE);
            gapp_widget_set_margin_top(button, 10);
            gtk_widget_set_tooltip_text(button, "Swap between portrait and landscape orientation");
            gtk_box_append(GTK_BOX(hbox), button);
        }

        // Resolucion custom
        hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_widget_set_hexpand(hbox, TRUE);
        gapp_widget_set_margin_top(hbox, 10);
        gtk_box_append(GTK_BOX(vbox_main), hbox);
        {
            // vbox para un label y el spin button
            vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_set_tooltip_text(vbox, "The width of the new view port window in pixels (0 = use the desktop resolution)");
            gtk_box_append(GTK_BOX(hbox), vbox);
            {
                label = gtk_label_new("Viewport width");
                gtk_widget_set_halign(label, GTK_ALIGN_START);
                gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
                gtk_widget_set_margin_end(label, 10);
                gtk_widget_set_margin_bottom(label, 5);
                gtk_box_append(GTK_BOX(vbox), label);

                spin = gtk_spin_button_new_with_range(0, 10000, 1);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), gb_project_config_get_width());
                gtk_widget_set_hexpand(spin, TRUE);
                gtk_box_append(GTK_BOX(vbox), spin);
            }

            vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_set_tooltip_text(vbox, "The height of the new view port window in pixels (0 = use the desktop resolution)");
            gtk_box_append(GTK_BOX(hbox), vbox);
            {
                label = gtk_label_new("Viewport height");
                gtk_widget_set_halign(label, GTK_ALIGN_START);
                gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
                gtk_widget_set_margin_end(label, 10);
                gtk_widget_set_margin_bottom(label, 5);
                gtk_box_append(GTK_BOX(vbox), label);

                spin = gtk_spin_button_new_with_range(0, 10000, 1);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), gb_project_config_get_height());
                gtk_widget_set_hexpand(spin, TRUE);
                gtk_box_append(GTK_BOX(vbox), spin);
            }
        }
    }

    expand = gapp_widget_expander_with_widget("<b>Window</b>", NULL);
    gapp_widget_set_margin(expand, 20);
    gtk_box_append(GTK_BOX(bbox), expand);
    {
        vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_hexpand(vbox_main, TRUE);
        gtk_expander_set_child(GTK_EXPANDER(expand), vbox_main);

        // Mode Window combo box [Windowed, Minimized, Maximized, Fullscreen]
        combo = gtk_combo_box_text_new();
        gapp_widget_set_margin_top(combo, 10);
        gtk_widget_set_tooltip_text(combo, "The initial window mode");
        gtk_box_append(GTK_BOX(vbox_main), combo);
        {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Windowed");
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Minimized");
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Maximized");
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Fullscreen");
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), gb_project_config_get_mode());

        check = gtk_check_button_new_with_label("Resizable");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_resizable());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("Borderless");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_borderless());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("Always on Top");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_alwayontop());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("Transparent");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_transparent());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("No Focus");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_nofocus());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("Allow high DPI");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_highdpi());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("VSync");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_vsync());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);

        check = gtk_check_button_new_with_label("Should Window Preserve Aspect Ratio");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), gb_project_config_get_aspectration());
        gapp_widget_set_margin_top(check, 10);
        gtk_box_append(GTK_BOX(vbox_main), check);
    }

    return bbox;
}
