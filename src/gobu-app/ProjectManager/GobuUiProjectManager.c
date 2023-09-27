#include <gtk/gtk.h>
#include "GobuWidgets.h"
#include "GobuSignalCheckNameProject.h"
#include "GobuSignalChangePath.h"
#include "GobuSignalCreateProject.h"
#include "GobuSignalOpenProject.h"

void GobuUiProjectManager(GtkApplication *app, GobuProjectManager *ctx)
{
    GtkWidget *vbox, *hbox, *btn_other_chooser, *label;

    ctx->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(ctx->window), "GobuEngine - ProjectManager");
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), 500, 200);
    gtk_window_set_resizable(ctx->window, FALSE);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(ctx->window), vbox);
    GobuWidgetSetMargin(vbox, 15);

    label = gtk_label_new("Project Name:");
    gtk_label_set_xalign(label, 0.0);
    gtk_box_append(GTK_BOX(vbox), label);

    ctx->entry_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_name), "New Project");
    gtk_box_append(GTK_BOX(vbox), ctx->entry_name);
    g_signal_connect(ctx->entry_name, "changed", G_CALLBACK(GobuSignalCheckNameProject), ctx);

    label = gtk_label_new("Project Path:");
    gtk_label_set_xalign(label, 0.0);
    gtk_box_append(GTK_BOX(vbox), label);

    ctx->btn_file_chooser = gtk_button_new_with_label(g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS));
    gtk_box_append(GTK_BOX(vbox), ctx->btn_file_chooser);
    g_signal_connect(ctx->btn_file_chooser, "clicked", G_CALLBACK(GobuSignalChangePath), ctx);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_end(hbox, 15);
    gtk_widget_set_margin_top(hbox, 15);
    gtk_widget_set_margin_bottom(hbox, 15);
    gtk_box_append(GTK_BOX(vbox), hbox);

    ctx->btn_create = gtk_button_new_with_label("Create");
    gtk_widget_set_sensitive(GTK_BUTTON(ctx->btn_create), FALSE);
    gtk_box_append(GTK_BOX(hbox), ctx->btn_create);
    g_signal_connect(ctx->btn_create, "clicked", G_CALLBACK(GobuSignalCreateProject), ctx);

    btn_other_chooser = gtk_button_new_with_label("Open other");
    gtk_box_append(GTK_BOX(hbox), btn_other_chooser);
    g_signal_connect(btn_other_chooser, "clicked", G_CALLBACK(GobuSignalOpenProject), ctx);

    gtk_window_present(GTK_WINDOW(ctx->window));
}