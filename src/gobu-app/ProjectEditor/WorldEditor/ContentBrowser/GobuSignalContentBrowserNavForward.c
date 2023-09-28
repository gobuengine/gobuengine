#include "GobuSignalContentBrowserNavForward.h"
#include "GobuFnContentBrowserHistoryPath.h"

void GobuSignalContentBrowserNavForward(GtkWidget *button, gpointer data)
{
    GobuFnContentBrowserHistoryPathForward(data);
}