#include "GobuSignalCreateComponent.h"
#include "GobuWidgets.h"

static void GobuSignalCreateComponentResponse(GtkDialog *dialog, int response, gpointer data)
{
    if (response == GTK_RESPONSE_OK)
    {
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void GobuSignalCreateComponent(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog = GobuWidgetDialogInput(button, "Create new Component", "New");
    g_signal_connect(dialog, "response", G_CALLBACK(GobuSignalCreateComponentResponse), NULL);
}
