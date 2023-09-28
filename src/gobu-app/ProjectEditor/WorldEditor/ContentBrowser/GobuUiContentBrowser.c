#include "GobuUiContentBrowser.h"
#include "GobuUiContentBrowserViewFile.h"
#include "GobuUiContentBrowserToolbar.h"
#include "GobuUiContentBrowserNavToolbar.h"

GtkWidget *GobuUiContentBrowserNew(void)
{
    GtkWidget *toolbar, *view_file, *toolbar_nav;
    GtkWidget *browser_content;

    browser_content = g_object_new(GTK_TYPE_GOBU_BROWSER_CONTENT,
                                   "orientation", GTK_ORIENTATION_VERTICAL, NULL);

    toolbar = GobuUiContentBrowserToolbar(browser_content);
    gtk_box_append(browser_content, toolbar);

    gtk_box_append(browser_content, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    toolbar_nav = GobuUiContentBrowserNavToolbar(browser_content);
    gtk_box_append(browser_content, toolbar_nav);

    gtk_box_append(browser_content, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    view_file = GobuUiContentBrowserViewFile(browser_content);
    gtk_box_append(browser_content, view_file);

    return browser_content;
}
