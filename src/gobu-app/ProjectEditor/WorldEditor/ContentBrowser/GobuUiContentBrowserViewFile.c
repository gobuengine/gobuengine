#include "GobuUiContentBrowserViewFile.h"
#include "GobuSignalContentBrowserListItemFactory.h"
#include "GobuSignalContentBrowserViewFileSelected.h"
#include "GobuSignalContentBrowserViewFilePopover.h"

GtkWidget *GobuUiContentBrowserViewFile(GobuBrowserContent *browser)
{
    GtkWidget *scroll, *grid;
    GtkSingleSelection *selection;
    GtkGesture *gesture;
    GFile *file;

    GobuBrowserContentPrivate *private = GobuContentBrowserGet(browser);

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
    g_signal_connect(private->factory, "setup", G_CALLBACK(GobuSignalContentBrowserListItemFactorySetup), browser);
    g_signal_connect(private->factory, "bind", G_CALLBACK(GobuSignalContentBrowserListItemFactoryBind), browser);

    // GtkWidget *view = gtk_list_view_new(selection, private->factory);
    grid = gtk_grid_view_new(private->selection, private->factory);
    // gtk_grid_view_set_single_click_activate(grid, TRUE);
    gtk_grid_view_set_max_columns(grid, 15);
    gtk_grid_view_set_min_columns(grid, 2);
    g_signal_connect(grid, "activate", G_CALLBACK(GobuSignalContentBrowserViewFileSelected), browser);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);

    gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    g_signal_connect(gesture, "released", G_CALLBACK(GobuSignalContentBrowserViewFilePopover), browser);
    gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(gesture));

    return scroll;
}
