#include "GobuSignalContentBrowserNavBack.h"
#include "GobuFnContentBrowserHistoryPath.h"

void GobuSignalContentBrowserNavBack(GtkWidget *button, gpointer data)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(data);
    GobuFnContentBrowserHistoryPathBack(data);
}
