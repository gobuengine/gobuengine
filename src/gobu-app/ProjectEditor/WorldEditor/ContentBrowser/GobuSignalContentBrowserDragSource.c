#include "GobuSignalContentBrowserDragSource.h"

GdkContentProvider *GobuSignalContentBrowserDragSourcePrepare(GtkDragSource *source, double x, double y, GtkListItem *list_item)
{
    GtkWidget *box = gtk_list_item_get_child(list_item);

    GtkWidget *image = gtk_widget_get_first_child(box);

    GobuBrowserContentPrivate *private = GobuContentBrowserGet(g_object_get_data(box, "BrowserContent"));

    gint pos = GPOINTER_TO_UINT(g_object_get_data(box, "position_id"));

    gtk_single_selection_set_selected(private->selection, pos);

    GtkWidget *paintable = gtk_widget_paintable_new(image);
    gtk_drag_source_set_icon(source, paintable, x, y);
    return gdk_content_provider_new_typed(G_TYPE_STRING, "");
}