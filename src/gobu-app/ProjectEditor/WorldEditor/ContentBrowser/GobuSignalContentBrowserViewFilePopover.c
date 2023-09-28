#include "GobuSignalContentBrowserViewFilePopover.h"
#include "GobuSignalContentBrowserCreateFolder.h"
#include "GobuSignalContentBrowserCreateEntity.h"
#include "GobuSignalContentBrowserCreateLevel.h"
#include "GobuSignalContentBrowserDeleteFile.h"

void GobuSignalContentBrowserViewFilePopover(GtkGesture *gesture, int n_press, double x, double y, gpointer data)
{
    GtkWidget *widget, *child;
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(data);

    widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);

    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    {
        GtkWidget *popover, *box, *item;

        gint p = GPOINTER_TO_UINT(g_object_get_data(child, "position_id"));
        gtk_single_selection_set_selected(private->selection, p);

        private->popover = gtk_popover_new();
        gtk_popover_set_cascade_popdown(GTK_POPOVER(private->popover), TRUE);
        gtk_widget_set_parent(GTK_POPOVER(private->popover), widget);
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
                g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalContentBrowserDeleteFile), data);
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
                g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalContentBrowserCreateFolder), data);

                gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                item = gtk_button_new_with_label("New Entity");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalContentBrowserCreateEntity), data);

                item = gtk_button_new_with_label("New World");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalContentBrowserCreateLevel), data);
            }
        }

        gtk_popover_popup(GTK_POPOVER(private->popover));
    }

    if (child == widget)
    {
        gtk_single_selection_set_selected(private->selection, -1);
    }
}