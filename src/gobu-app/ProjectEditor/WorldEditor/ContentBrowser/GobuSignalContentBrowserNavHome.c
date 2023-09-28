#include "GobuSignalContentBrowserNavHome.h"
#include "GobuFnContentBrowserOpenFolder.h"

void GobuSignalContentBrowserNavHome(GtkWidget *button, gpointer data)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(data);
    GobuFnContentBrowserOpenFolder(data, private->path_default, TRUE);
}
