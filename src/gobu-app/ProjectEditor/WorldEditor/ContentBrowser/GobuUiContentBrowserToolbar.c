#include "GobuUiContentBrowserToolbar.h"
#include "GobuSignalContentBrowserPopover.h"
#include "gobu_widget.h"

GtkWidget *GobuUiContentBrowserToolbar(GobuBrowserContent *browser)
{
    GtkWidget *toolbar, *button;

    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GobuWidgetSetMargin(toolbar, 5);
    {
        button = GobuWidgetButtonIconLabelNew("list-add-symbolic", "Add");
        gtk_button_set_has_frame(button, FALSE);
        gtk_box_append(GTK_BOX(toolbar), button);
        g_signal_connect(button, "clicked", G_CALLBACK(GobuSignalContentBrowserPopover), browser);

        button = GobuWidgetButtonIconLabelNew("insert-image-symbolic", "Import");
        gtk_button_set_has_frame(button, FALSE);
        gtk_box_append(GTK_BOX(toolbar), button);
        // g_signal_connect(button, "clicked", G_CALLBACK(signal_popover_open_add_content), NULL);

        button = GobuWidgetButtonIconLabelNew("media-flash-symbolic", "Save All");
        gtk_button_set_has_frame(button, FALSE);
        gtk_box_append(GTK_BOX(toolbar), button);
        // g_signal_connect(button, "clicked", G_CALLBACK(signal_popover_open_add_content), NULL);
    }

    return toolbar;
}