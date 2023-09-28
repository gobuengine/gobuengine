#include "GobuFnContentBrowserHistoryPath.h"
#include "GobuFnContentBrowserOpenFolder.h"

void GobuFnContentBrowserHistoryPathForwardClear(GobuBrowserContent *browser)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(browser);

    if (private->path_back->len == 0 && private->path_forward != 0)
    {
        g_ptr_array_remove_range(private->path_forward, 0, private->path_forward->len);
    }
}

void GobuFnContentBrowserHistoryPathBack(GobuBrowserContent *browser)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(browser);

    guint index_last = (private->path_back->len - 1);
    const gchar *path = g_ptr_array_index(private->path_back, index_last);
    g_ptr_array_remove_index(private->path_back, index_last);
    g_ptr_array_add(private->path_forward, g_strdup(private->path_current));

    GobuFnContentBrowserOpenFolder(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), !(private->path_back->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), TRUE);
}

void GobuFnContentBrowserHistoryPathForward(GobuBrowserContent *browser)
{
    GobuBrowserContentPrivate *private = GobuContentBrowserGet(browser);

    guint index_last = (private->path_forward->len - 1);
    const gchar *path = g_ptr_array_index(private->path_forward, index_last);
    g_ptr_array_remove_index(private->path_forward, index_last);
    g_ptr_array_add(private->path_back, g_strdup(private->path_current));

    GobuFnContentBrowserOpenFolder(browser, path, FALSE);

    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), !(private->path_forward->len == 0));
    gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), TRUE);
}
