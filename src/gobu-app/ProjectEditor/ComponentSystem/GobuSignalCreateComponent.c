#include "GobuSignalCreateComponent.h"
#include "gobu_widget.h"
#include "GobuFnListComponentCreate.h"

static void GobuSignalCreateComponentResponse(GtkDialog *dialog, int response, GobuComponentSystem *component)
{
    if (response == GTK_RESPONSE_OK)
    {
        GobuFnListComponentCreate(component, GobuWidgetDialogInputGetText(dialog));
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void GobuSignalCreateComponent(GtkWidget *button, GobuComponentSystem *component)
{
    GtkWidget *dialog = GobuWidgetDialogInput(button, "Create new Component", "New");
    g_signal_connect(dialog, "response", G_CALLBACK(GobuSignalCreateComponentResponse), component);
}
