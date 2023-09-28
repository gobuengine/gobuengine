#include "GobuSignalContentBrowserPopover.h"
#include "GobuSignalContentBrowserCreateFolder.h"
#include "GobuSignalContentBrowserCreateEntity.h"
#include "GobuSignalContentBrowserCreateLevel.h"

void GobuSignalContentBrowserPopover(GtkWidget *button, gpointer data)
{
    GtkWidget *popover, *box, *item;

    popover = gtk_popover_new();
    gtk_widget_set_parent(popover, button);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_popover_set_child(GTK_POPOVER(popover), box);
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

    gtk_popover_popup(GTK_POPOVER(popover));
}