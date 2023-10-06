#include "GobuUiContentBrowserNavToolbar.h"
#include "GobuSignalContentBrowserNavHome.h"
#include "GobuSignalContentBrowserNavBack.h"
#include "GobuSignalContentBrowserNavForward.h"
#include "gobu_widget.h"

GtkWidget *GobuUiContentBrowserNavToolbar(GobuBrowserContent *browser)
{
    GtkWidget *toolbar, *button;

    GobuBrowserContentPrivate *private = GobuContentBrowserGet(browser);

    toolbar = GobuWidgetToolbarNew();
    {
        private->btn_nav_home = GobuWidgetButtonIconLabelNew("go-home-symbolic", "Home");
        gtk_button_set_has_frame(private->btn_nav_home, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_home), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_home);
        g_signal_connect(private->btn_nav_home, "clicked", G_CALLBACK(GobuSignalContentBrowserNavHome), browser);

        private->btn_nav_back = GobuWidgetButtonIconLabelNew("go-previous-symbolic", NULL);
        gtk_button_set_has_frame(private->btn_nav_back, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_back), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_back);
        g_signal_connect(private->btn_nav_back, "clicked", G_CALLBACK(GobuSignalContentBrowserNavBack), browser);

        private->btn_nav_forward = GobuWidgetButtonIconLabelNew("go-next-symbolic", NULL);
        gtk_button_set_has_frame(private->btn_nav_forward, FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(private->btn_nav_forward), FALSE);
        gtk_box_append(GTK_BOX(toolbar), private->btn_nav_forward);
        g_signal_connect(private->btn_nav_forward, "clicked", G_CALLBACK(GobuSignalContentBrowserNavForward), browser);
    }

    return toolbar;
}
