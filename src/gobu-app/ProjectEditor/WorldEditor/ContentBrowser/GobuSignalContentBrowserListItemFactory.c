#include "GobuSignalContentBrowserListItemFactory.h"
#include "GobuSignalContentBrowserDragSource.h"
#include "GobuFnContentBrowserGetIconFile.h"

void GobuSignalContentBrowserListItemFactorySetup(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
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
    g_signal_connect(source, "prepare", G_CALLBACK(GobuSignalContentBrowserDragSourcePrepare), list_item);
    // g_signal_connect(source, "drag-begin", G_CALLBACK(drag_begin), image);
    // g_signal_connect(source, "drag-end", G_CALLBACK(drag_end), image);
    gtk_widget_add_controller(box, GTK_EVENT_CONTROLLER(source));
    // gtk_expression_unref(expression);
}

void GobuSignalContentBrowserListItemFactoryBind(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
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

    GobuFnContentBrowserGetIconFile(image, info_file);

    g_object_set_data(image, "position_id", GINT_TO_POINTER(id));
    g_object_set_data(label_name, "position_id", GINT_TO_POINTER(id));
    g_object_set_data(box, "position_id", GINT_TO_POINTER(id));
}
