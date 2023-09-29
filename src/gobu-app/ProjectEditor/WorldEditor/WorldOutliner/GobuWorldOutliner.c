#include "GobuWorldOutliner.h"

GtkWidget *GobuWorldOutliner(void)
{
    GtkWidget *scroll;
    GtkWidget *view;
    GtkListItemFactory *factory;

    scroll = gtk_scrolled_window_new();

    factory = gtk_signal_list_item_factory_new ();

    GListModel *list_model = G_LIST_MODEL(g_list_model_new(NULL, NULL, NULL, NULL));

    view = gtk_column_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(model)), factory);
    gtk_scrolled_window_set_child(GTK_SCROLLED(scroll), view);

    return scroll;
}