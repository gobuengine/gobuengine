#include "gapp-ps-view-gameinfo.h"

#include "gapp_widget.h"
#include "gb_setting_project.h"
#include "gb_project.h"
#include "utility/gb_path.h"
#include "utility/gb_string.h"

typedef enum _GappProjectSettingsGameInfo
{
    GAPP_PROJECT_SETTINGS_GAME_INFO_NAME = 0,
    GAPP_PROJECT_SETTINGS_GAME_INFO_DESCRIPTION,
    GAPP_PROJECT_SETTINGS_GAME_INFO_AUTHOR,
    GAPP_PROJECT_SETTINGS_GAME_INFO_LICENSE,
    GAPP_PROJECT_SETTINGS_GAME_INFO_VERSION,
    GAPP_PROJECT_SETTINGS_GAME_INFO_THUMBNAIL
} GappProjectSettingsGameInfo;

extern ProjectEditor editor;

static void signal_changed_input_str(GObject *object, GappProjectSettingsGameInfo type);
static void signal_file_dialog_selected_thumbnail(GtkWidget *widget, GappProjectSettingsPrivate *ctx);

GtkWidget *gapp_project_setting_game_info_new(GappProjectSettingsPrivate *self)
{
    GtkWidget *entry, *description, *author,
        *box, *iframe, *button, *label, *listbox, *listboxrow,
        *scroll, *viewport;

    scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);

    viewport = gtk_viewport_new(NULL, NULL);
    gtk_scrolled_window_set_child(scroll, viewport);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gapp_widget_set_margin_start(box, 60);
    gapp_widget_set_margin_end(box, 60);
    gapp_widget_set_margin_top(box, 30);
    gapp_widget_set_margin_bottom(box, 30);
    gtk_viewport_set_child(viewport, box);

    label = gtk_label_new("<b>Game Info</b>");
    gtk_label_set_use_markup(label, TRUE);
    gtk_label_set_xalign(label, 0);
    gapp_widget_set_margin_bottom(label, 10);
    gtk_widget_add_css_class(label, "title-2");
    gtk_box_append(GTK_BOX(box), label);

    listbox = gtk_list_box_new();
    gtk_list_box_set_selection_mode(listbox, GTK_SELECTION_NONE);
    gtk_widget_add_css_class(listbox, "rich-list");
    gtk_widget_add_css_class(listbox, "boxed-list");
    gtk_box_append(GTK_BOX(box), listbox);

    // project thumbnail
    listboxrow = gtk_list_box_row_new();
    gtk_list_box_append(listbox, listboxrow);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_list_box_row_set_child(listboxrow, hbox);
    {
        const gchar *path_thumbnail = gb_path_join(gb_project_get_path(), "Saved", "thumbnail.png", NULL);
        iframe = gtk_frame_new("Thumbnail");
        gtk_widget_set_size_request(GTK_WIDGET(iframe), 128, 128);
        gtk_widget_set_halign(iframe, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_end(iframe, 10);
        gtk_box_append(GTK_BOX(hbox), iframe);
        gtk_widget_set_tooltip_text(iframe, gb_strdups("Target image (%s) Dimensions: 128x128", path_thumbnail));
        {
            self->thumbanil = gtk_image_new_from_file(path_thumbnail);
            gtk_widget_set_size_request(self->thumbanil, 128, 128);
            gtk_widget_set_halign(self->thumbanil, GTK_ALIGN_CENTER);
            gapp_widget_set_margin_top(self->thumbanil, 10);
            gtk_frame_set_child(GTK_FRAME(iframe), self->thumbanil);
        }

        button = gapp_widget_button_new_icon_with_label("camera-photo-symbolic", "Change");
        gapp_widget_set_margin_top(button, 10);
        gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
        gtk_widget_set_tooltip_text(button, "Choose a file from this computer");
        gtk_box_append(GTK_BOX(hbox), button);
        g_signal_connect(button, "clicked", G_CALLBACK(signal_file_dialog_selected_thumbnail), self);
    }

    // Description del juego
    description = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(description), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(description), TRUE);
    gapp_widget_view_set_text(description, gb_setting_project_description());
    gtk_widget_set_hexpand(description, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(description), -1, 100);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(description), 10);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(description), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(description), 10);
    g_signal_connect(gtk_text_view_get_buffer(description), "changed", G_CALLBACK(signal_changed_input_str), GAPP_PROJECT_SETTINGS_GAME_INFO_DESCRIPTION);
    project_setting_group_child_append("Description", listbox, description, GTK_ORIENTATION_VERTICAL);

    // Nombre del juego
    entry = project_setting_group_input_entry("Game entry", "The project's entry");
    gapp_widget_entry_set_text(entry, gb_setting_project_name());
    project_setting_group_child_append("Name", listbox, entry, GTK_ORIENTATION_HORIZONTAL);
    g_signal_connect(entry, "changed", G_CALLBACK(signal_changed_input_str), GAPP_PROJECT_SETTINGS_GAME_INFO_NAME);

    // Version
    entry = project_setting_group_input_entry("Version", "The project's version number.");
    gapp_widget_entry_set_text(entry, gb_setting_project_version());
    project_setting_group_child_append("Version", listbox, entry, GTK_ORIENTATION_HORIZONTAL);
    g_signal_connect(entry, "changed", G_CALLBACK(signal_changed_input_str), GAPP_PROJECT_SETTINGS_GAME_INFO_VERSION);

    // Author del juego
    author = project_setting_group_input_entry("Author", "The project's author");
    gapp_widget_entry_set_text(author, gb_setting_project_author());
    project_setting_group_child_append("Author", listbox, author, GTK_ORIENTATION_HORIZONTAL);
    g_signal_connect(author, "changed", G_CALLBACK(signal_changed_input_str), GAPP_PROJECT_SETTINGS_GAME_INFO_AUTHOR);

    return scroll;
}

static void signal_changed_input_str(GObject *object, GappProjectSettingsGameInfo type)
{
    const gchar *text;

    if (type == GAPP_PROJECT_SETTINGS_GAME_INFO_DESCRIPTION)
        text = gapp_widget_view_get_text(GTK_TEXT_BUFFER(object));
    else
        text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(object)));

    g_return_if_fail(text);

    switch (type)
    {
    case GAPP_PROJECT_SETTINGS_GAME_INFO_NAME:
        gb_setting_project_set_name(text);
        break;
    case GAPP_PROJECT_SETTINGS_GAME_INFO_DESCRIPTION:
        gb_setting_project_set_description(text);
        break;
    case GAPP_PROJECT_SETTINGS_GAME_INFO_AUTHOR:
        gb_setting_project_set_author(text);
        break;
    case GAPP_PROJECT_SETTINGS_GAME_INFO_LICENSE:
        gb_setting_project_set_license(text);
        break;
    case GAPP_PROJECT_SETTINGS_GAME_INFO_VERSION:
        gb_setting_project_set_version(text);
        break;
    default:
        break;
    }

    gb_setting_project_save();
}

static void response_selected_thumbnail_ready(GtkFileDialog *source, GAsyncResult *res, GappProjectSettingsPrivate *ctx)
{
    GFile *file;
    GError *error = NULL;

    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, &error);

    if (error != NULL)
        g_error_free(error);

    g_return_if_fail(file);

    char *path_str = g_file_get_path(file);
    gtk_image_set_from_file(GTK_IMAGE(ctx->thumbanil), path_str);

    GError *err = NULL;
    GFile *file_src = g_file_new_for_path(path_str);
    GFile *file_dest = g_file_new_build_filename(gb_project_get_path(), "Saved", "thumbnail.png", NULL);

    if (gb_fs_copyc(file_src, file_dest, &err))
        g_info("GOBU-APP: Thumbnail copied\n");
    else
        g_error("%s", err->message);

    g_object_unref(file_src);
    g_object_unref(file_dest);

    g_free(path_str);
}

static void signal_file_dialog_selected_thumbnail(GtkWidget *widget, GappProjectSettingsPrivate *ctx)
{
    GtkFileDialog *file_dialog;
    file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(GTK_FILE_DIALOG(file_dialog),
                         GTK_WINDOW(editor.window), NULL, response_selected_thumbnail_ready, ctx);
}
