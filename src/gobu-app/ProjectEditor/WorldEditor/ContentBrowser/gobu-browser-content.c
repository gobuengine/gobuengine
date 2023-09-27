/* GOBU - Game Engine
 * Copyright (C) 2023 Jhonson Ozuna <hbiblia@gmail.com>
 *
 * gobu-browser-content.c
 *
 */

#include "ContentBrowser.h"
#include "gobu-editor.h"
#include "gobu-gtk-widget.h"
#include "gobu-dialog.h"

static void browser_view_file_set_folder_open(GobuBrowserContent *browser, const gchar *path, gboolean history_saved);

enum
{
    COL_PATH = 0,
    COL_DISPLAY_NAME,
    COL_PIXBUF,
    COL_IS_DIRECTORY,
    NUM_COLS
};

struct _GobuBrowserContent
{
    GtkWidget parent;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuBrowserContent, gobu_browser_content, GTK_TYPE_BOX);

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void gobu_browser_content_class_init(GobuBrowserContentClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void gobu_browser_content_init(GobuBrowserContent *self)
{
    GobuEditorContext *ctx = gobu_editor_get_context();
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(self);

    private->path_back = g_ptr_array_new();
    private->path_forward = g_ptr_array_new();
    private->path_current = g_strdup(ctx->browser.path_current);
    private->path_default = g_strdup(ctx->browser.path_current);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_dialog_browser_new_folder(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(data);
        const gchar *result = gobu_dialog_input_get(dialog);
        gchar *path = g_build_filename(private->path_current, result, NULL);
        g_file_make_directory_async(g_file_new_for_path(path), G_PRIORITY_LOW, NULL, NULL, NULL);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_action_browser_new_folder(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = gobu_dialog_input_show(button, "Create New Folder", "New Folder");
    g_signal_connect(dialog, "response", G_CALLBACK(signal_dialog_browser_new_folder), data);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_dialog_browser_new_entity_on_response(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(data);
        const gchar *result = gobu_dialog_input_get(dialog);
        gchar *path = g_build_filename(private->path_current, g_strdup_printf("%s.gentity", result), NULL);

        g_file_create_readwrite(g_file_new_for_path(path), G_FILE_CREATE_NONE, NULL, NULL);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void sginal_action_browser_new_entity(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = gobu_dialog_input_show(button, "Create New Entity", "EntityEmpty");
    g_signal_connect(dialog, "response", G_CALLBACK(signal_dialog_browser_new_entity_on_response), data);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_dialog_browser_new_level_on_response(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(data);
        const gchar *result = gobu_dialog_input_get(dialog);
        gchar *path = g_build_filename(private->path_current, g_strdup_printf("%s.gworld", result), NULL);

        g_file_create_readwrite(g_file_new_for_path(path), G_FILE_CREATE_NONE, NULL, NULL);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void sginal_action_browser_new_level(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = gobu_dialog_input_show(button, "Create New Gobu World", "NewWorld");
    g_signal_connect(dialog, "response", G_CALLBACK(signal_dialog_browser_new_level_on_response), data);
    gtk_popover_popdown(GTK_POPOVER(gtk_widget_get_ancestor(button, GTK_TYPE_POPOVER)));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_selected_delete_on_response(GtkWidget *widget, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
        GError *error = NULL;
        GFile *file = G_FILE(data);
        if (!g_file_delete(file, NULL, &error) && !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        {
            gobu_dialog_new_message_show(widget, error->message);
            g_warning("Failed to delete %s: %s [%d]\n", g_file_peek_path(file), error->message, error->code);
            return;
        }
    }

    gtk_window_destroy(widget);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_selected_delete(GtkWidget *widget, gpointer data)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(data);

    GFileInfo *file_selected = G_FILE_INFO(gtk_single_selection_get_selected_item(private->selection));
    GFile *file = G_FILE(g_file_info_get_attribute_object(file_selected, "standard::file"));

    gobu_dialog_new_confirm_delete(widget, file, browser_view_file_selected_delete_on_response);
    gtk_popover_popdown(private->popover);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_menu_popover_click_pressed(GtkGesture *gesture, int n_press, double x, double y, gpointer data)
{
    GtkWidget *widget, *child;
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(data);

    widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);

    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    {
        GtkWidget *popover, *box, *item;

        gint p = GPOINTER_TO_UINT(g_object_get_data(child, "position_id"));
        gtk_single_selection_set_selected(private->selection, p);

        private->popover = gtk_popover_new();
        gtk_popover_set_cascade_popdown(private->popover, TRUE);
        gtk_widget_set_parent(private->popover, widget);
        gtk_popover_set_has_arrow(GTK_POPOVER(private->popover), FALSE);
        gtk_popover_set_pointing_to(GTK_POPOVER(private->popover), &(GdkRectangle){x, y, 1, 1});

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_popover_set_child(GTK_POPOVER(private->popover), box);
        {
            if (child != NULL && child != widget)
            {
                item = gtk_button_new_with_label("Delete");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(browser_view_file_selected_delete), data);
            }
            else
            {
                item = gtk_button_new_with_label("Import to /Game");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);

                gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                item = gtk_button_new_with_label("New Folder");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(signal_action_browser_new_folder), data);

                gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                item = gtk_button_new_with_label("New Entity");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(sginal_action_browser_new_entity), data);

                item = gtk_button_new_with_label("New World");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(sginal_action_browser_new_level), data);

                item = gtk_button_new_with_label("New Component");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(sginal_action_browser_new_level), data);
            }
        }

        gtk_popover_popup(GTK_POPOVER(private->popover));
    }

    if (child == widget)
    {
        gtk_single_selection_set_selected(private->selection, -1);
    }
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_menu_popover_click_released(GtkGesture *gesture, int n_press, double x, double y, gpointer data)
{
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_set_file_icon(GFileInfo *info_file, GtkWidget *image)
{
    const char *ext_file = g_file_info_get_content_type(info_file);

    if (g_strcmp0(ext_file, ".png") == 0 || g_strcmp0(ext_file, ".jpg") == 0)
    {
        GFile *file = G_FILE(g_file_info_get_attribute_object(info_file, "standard::file"));
        gtk_image_set_from_file(image, g_file_get_path(file));
        // g_object_unref(file);
    }
    else if (g_strcmp0(ext_file, ".gcomponent") == 0)
    {
        gtk_image_set_from_pixbuf(image, gobu_editor_get_icons(GOBU_EDITOR_ICON_COMPONENT));
    }
    else if (g_strcmp0(ext_file, ".gworld") == 0)
    {
        gtk_image_set_from_pixbuf(image, gobu_editor_get_icons(GOBU_EDITOR_ICON_WORLD));
    }
    else if (g_strcmp0(ext_file, ".gentity") == 0)
    {
        gtk_image_set_from_pixbuf(image, gobu_editor_get_icons(GOBU_EDITOR_ICON_ENTITY));
    }
    else
    {
        gtk_image_set_from_gicon(image, g_file_info_get_icon(info_file));
    }
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_history_forward_clear(GobuBrowserContent *browser)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(browser);

    if (private->path_back->len == 0 && private->path_forward != 0)
    {
        g_ptr_array_remove_range(private->path_forward, 0, private->path_forward->len);
    }
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_history_path_back(GobuBrowserContent *browser)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(browser);

    guint index_last = (private->path_back->len - 1);
    const gchar *path = g_ptr_array_index(private->path_back, index_last);
    g_ptr_array_remove_index(private->path_back, index_last);
    g_ptr_array_add(private->path_forward, g_strdup(private->path_current));

    browser_view_file_set_folder_open(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), !(private->path_back->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), TRUE);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_history_path_forward(GobuBrowserContent *browser)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(browser);

    guint index_last = (private->path_forward->len - 1);
    const gchar *path = g_ptr_array_index(private->path_forward, index_last);
    g_ptr_array_remove_index(private->path_forward, index_last);
    g_ptr_array_add(private->path_back, g_strdup(private->path_current));

    browser_view_file_set_folder_open(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), !(private->path_forward->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static GdkContentProvider *drag_prepare(GtkDragSource *source, double x, double y, GtkListItem *list_item)
{
    GtkWidget *box = gtk_list_item_get_child(list_item);

    GtkWidget *image = gtk_widget_get_first_child(box);

    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(g_object_get_data(box, "BrowserContent"));

    gint pos = GPOINTER_TO_UINT(g_object_get_data(box, "position_id"));

    gtk_single_selection_set_selected(private->selection, pos);

    GtkWidget *paintable = gtk_widget_paintable_new(image);
    gtk_drag_source_set_icon(source, paintable, x, y);
    return gdk_content_provider_new_typed(G_TYPE_STRING, "");
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_list_item_setup_default(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    GtkWidget *box, *imagen, *label, *entry;
    GtkExpression *expression;
    GtkDragSource *source;
    // expression = gtk_constant_expression_new(GTK_TYPE_LIST_ITEM, list_item);

    GFileInfo *info_file = gtk_list_item_get_item(list_item);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_list_item_set_child(list_item, box);
    gtk_widget_set_size_request(box, 64, 84);

    imagen = gtk_image_new();
    gtk_image_set_icon_size(imagen, GTK_ICON_SIZE_LARGE);
    gtk_box_append(GTK_BOX(box), imagen);

    label = gtk_label_new(NULL);
    gtk_label_set_wrap_mode(label, PANGO_WRAP_CHAR);
    gtk_label_set_justify(label, GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(box), label);

    g_object_set_data(box, "BrowserContent", user_data);

    source = gtk_drag_source_new();
    gtk_drag_source_set_actions(source, GDK_ACTION_MOVE);
    g_signal_connect(source, "prepare", G_CALLBACK(drag_prepare), list_item);
    // g_signal_connect(source, "drag-begin", G_CALLBACK(drag_begin), image);
    // g_signal_connect(source, "drag-end", G_CALLBACK(drag_end), image);
    gtk_widget_add_controller(box, GTK_EVENT_CONTROLLER(source));
    // gtk_expression_unref(expression);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_list_item_bind(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    GtkWidget *box, *label_name, *image;

    box = gtk_list_item_get_child(list_item);
    g_return_if_fail(box);

    GFileInfo *info_file = gtk_list_item_get_item(list_item);
    gint id = gtk_list_item_get_position(list_item);

    image = gtk_widget_get_first_child(box);
    label_name = gtk_widget_get_next_sibling(image);

    const char *name = g_file_info_get_name(info_file);
    gtk_label_set_label(GTK_LABEL(label_name), name);
    gtk_widget_set_tooltip_text(box, name);

    browser_view_file_set_file_icon(info_file, image);

    g_object_set_data(image, "position_id", GINT_TO_POINTER(id));
    g_object_set_data(label_name, "position_id", GINT_TO_POINTER(id));
    g_object_set_data(box, "position_id", GINT_TO_POINTER(id));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_set_folder_open(GobuBrowserContent *browser, const gchar *path, gboolean history_saved)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(browser);

    // HISTORIAL DE PATH
    if (history_saved)
        g_ptr_array_add(private->path_back, g_strdup(private->path_current));

    g_free(private->path_current);
    private->path_current = g_strdup(path);

    GFile *file = g_file_new_for_path(private->path_current);
    gboolean is_home = (g_strcmp0(g_file_get_basename(file), "Content") != 0);
    gtk_directory_list_set_file(private->directory_list, file);
    // g_object_unref(file);

    // EL BOTON DEL NAV (NEXT)RIGHT SE DESHABILITA SIEMPRE QUE CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
    //  EL BOTON DE NAV (PREV)LEFT SE HABILITA CUANDO CAMBIAMOS DE CARPETA.
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
    //  EL BOTON DE NAV (HOME) SE HABILITA SI NO ESTAMOS EN CONTENT
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), is_home);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_on_selected(GtkGridView *self, guint position, gpointer user_data)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(user_data);

    /* OBTENEMOS LA INFO DEL ARCHIVO SELECCIONADO CON DOUBLE-CLIC */
    GFileInfo *file_info = g_list_model_get_item(G_LIST_MODEL(gtk_grid_view_get_model(self)), position);

    /* COMPROBAMOS SI ES UN DIRECTORIO Y ENTRAMOS */
    if (g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY)
    {
        GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));

        browser_view_file_history_forward_clear(user_data);
        browser_view_file_set_folder_open(user_data, g_file_get_path(file), TRUE);

        // g_object_unref(file);
    }
    g_object_unref(file_info);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static GtkWidget *browser_view_file_list_init(GobuBrowserContent *browser)
{
    GtkWidget *scroll, *grid;
    GtkSingleSelection *selection;
    GtkGesture *gesture;
    GFile *file;

    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(browser);

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);

    file = g_file_new_for_path(private->path_default);
    private->directory_list = gtk_directory_list_new("standard::*", file);
    // gtk_directory_list_set_monitored(private->directory_list, TRUE);
    // gtk_directory_list_set_io_priority(private->directory_list, G_PRIORITY_LOW);
    g_object_unref(file);

    private->selection = gtk_single_selection_new(G_LIST_MODEL(private->directory_list));
    gtk_single_selection_set_can_unselect(private->selection, TRUE);
    gtk_single_selection_set_autoselect(private->selection, FALSE);

    private->factory = gtk_signal_list_item_factory_new();
    g_signal_connect(private->factory, "setup", G_CALLBACK(signal_list_item_setup_default), browser);
    g_signal_connect(private->factory, "bind", G_CALLBACK(signal_list_item_bind), browser);

    // GtkWidget *view = gtk_list_view_new(selection, private->factory);
    grid = gtk_grid_view_new(private->selection, private->factory);
    // gtk_grid_view_set_single_click_activate(grid, TRUE);
    gtk_grid_view_set_max_columns(grid, 15);
    gtk_grid_view_set_min_columns(grid, 2);
    g_signal_connect(grid, "activate", G_CALLBACK(browser_view_file_on_selected), browser);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);

    gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    g_signal_connect(gesture, "released", G_CALLBACK(browser_view_file_menu_popover_click_pressed), browser);
    gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(gesture));

    return scroll;
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_set_nav_home(GtkWidget *button, gpointer data)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(data);
    browser_view_file_set_folder_open(data, private->path_default, TRUE);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_on_nav_back(GtkWidget *button, gpointer data)
{
    browser_view_file_history_path_back(data);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void browser_view_file_on_nav_forward(GtkWidget *button, gpointer data)
{
    browser_view_file_history_path_forward(data);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void signal_popover_open_add_content(GtkWidget *button, gpointer data)
{
    GtkWidget *popover, *box, *btn_item;

    popover = gtk_popover_new();
    gtk_widget_set_parent(popover, button);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_popover_set_child(GTK_POPOVER(popover), box);
    {
        btn_item = gtk_button_new_with_label("Import to /Game");
        gtk_button_set_has_frame(GTK_BUTTON(btn_item), FALSE);
        gtk_box_append(GTK_BOX(box), btn_item);

        gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

        btn_item = gtk_button_new_with_label("New Folder");
        gtk_button_set_has_frame(GTK_BUTTON(btn_item), FALSE);
        gtk_box_append(GTK_BOX(box), btn_item);
        g_signal_connect(btn_item, "clicked", G_CALLBACK(signal_action_browser_new_folder), data);

        gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

        btn_item = gtk_button_new_with_label("New Entity");
        gtk_button_set_has_frame(GTK_BUTTON(btn_item), FALSE);
        gtk_box_append(GTK_BOX(box), btn_item);
        g_signal_connect(btn_item, "clicked", G_CALLBACK(sginal_action_browser_new_entity), data);

        btn_item = gtk_button_new_with_label("New World");
        gtk_button_set_has_frame(GTK_BUTTON(btn_item), FALSE);
        gtk_box_append(GTK_BOX(box), btn_item);
        g_signal_connect(btn_item, "clicked", G_CALLBACK(sginal_action_browser_new_level), data);

        btn_item = gtk_button_new_with_label("New Component");
        gtk_button_set_has_frame(GTK_BUTTON(btn_item), FALSE);
        gtk_box_append(GTK_BOX(box), btn_item);
        g_signal_connect(btn_item, "clicked", G_CALLBACK(sginal_action_browser_new_level), data);
    }

    gtk_popover_popup(GTK_POPOVER(popover));
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static GtkWidget *browser_toolbar_init(GobuBrowserContent *browser)
{
    GtkWidget *toolbar, *button;

    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GobuWidgetSetMargin(toolbar, 5);
    {
        button = gobu_widget_button_icon_label_new("list-add-symbolic", "Add");
        gtk_button_set_has_frame(button, FALSE);
        gtk_box_append(GTK_BOX(toolbar), button);
        g_signal_connect(button, "clicked", G_CALLBACK(signal_popover_open_add_content), browser);

        button = gobu_widget_button_icon_label_new("insert-image-symbolic", "Import");
        gtk_button_set_has_frame(button, FALSE);
        gtk_box_append(GTK_BOX(toolbar), button);
        // g_signal_connect(button, "clicked", G_CALLBACK(signal_popover_open_add_content), NULL);

        button = gobu_widget_button_icon_label_new("media-flash-symbolic", "Save All");
        gtk_button_set_has_frame(button, FALSE);
        gtk_box_append(GTK_BOX(toolbar), button);
        // g_signal_connect(button, "clicked", G_CALLBACK(signal_popover_open_add_content), NULL);
    }

    return toolbar;
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static GtkWidget *browser_toolbar_navegator_init(GobuBrowserContent *browser)
{
    GtkWidget *toolbar, *button;

    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(browser);

    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GobuWidgetSetMargin(toolbar, 5);
    {
        private->btn_nav_home = gobu_widget_button_icon_label_new("go-home-symbolic", "Home");
        gtk_button_set_has_frame(private->btn_nav_home, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_home);
        g_signal_connect(private->btn_nav_home, "clicked", G_CALLBACK(browser_view_file_set_nav_home), browser);

        private->btn_nav_back = gobu_widget_button_icon_label_new("go-previous-symbolic", NULL);
        gtk_button_set_has_frame(private->btn_nav_back, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_back);
        g_signal_connect(private->btn_nav_back, "clicked", G_CALLBACK(browser_view_file_on_nav_back), browser);

        private->btn_nav_forward = gobu_widget_button_icon_label_new("go-next-symbolic", NULL);
        gtk_button_set_has_frame(private->btn_nav_forward, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_forward);
        g_signal_connect(private->btn_nav_forward, "clicked", G_CALLBACK(browser_view_file_on_nav_forward), browser);
    }

    return toolbar;
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

GobuBrowserContent *gobu_browser_content_new(void)
{
    GtkWidget *toolbar, *view_file, *toolbar_nav;
    GobuBrowserContent *browser_content;

    browser_content = g_object_new(GTK_TYPE_GOBU_BROWSER_CONTENT,
                                   "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    toolbar = browser_toolbar_init(browser_content);
    gtk_box_append(browser_content, toolbar);

    gtk_box_append(browser_content, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    toolbar_nav = browser_toolbar_navegator_init(browser_content);
    gtk_box_append(browser_content, toolbar_nav);

    gtk_box_append(browser_content, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    view_file = browser_view_file_list_init(browser_content);
    gtk_box_append(browser_content, view_file);

    return browser_content;
}
