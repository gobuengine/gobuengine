#include "config.h"
#include "project_manager.h"
#include "project_manager_signal.h"

static GobuProjectManager project_manager = { 0 };

void gapp_project_manager_window_new(GtkApplication* app)
{
    GtkWidget* vbox, * hbox, * btn_other_chooser, * label;

    project_manager.window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(project_manager.window), lang_projectm_title);
    gtk_window_set_default_size(GTK_WINDOW(project_manager.window), 500, 200);
    gtk_window_set_resizable(project_manager.window, FALSE);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(project_manager.window), vbox);
    gapp_widget_set_margin(vbox, 15);

    label = gtk_label_new(lang_projectm_label2);
    gtk_label_set_xalign(label, 0.0);
    gtk_box_append(GTK_BOX(vbox), label);

    project_manager.entry_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(project_manager.entry_name), lang_projectm_button1);
    gtk_box_append(GTK_BOX(vbox), project_manager.entry_name);
    g_signal_connect(project_manager.entry_name, "changed",
        G_CALLBACK(project_manager_signal_change_entry_check_name_project), &project_manager);

    label = gtk_label_new(lang_projectm_label1);
    gtk_label_set_xalign(label, 0.0);
    gtk_box_append(GTK_BOX(vbox), label);

    project_manager.btn_file_chooser = gtk_button_new_with_label(g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS));
    gtk_box_append(GTK_BOX(vbox), project_manager.btn_file_chooser);
    g_signal_connect(project_manager.btn_file_chooser, "clicked",
        G_CALLBACK(project_manager_signal_open_dialog_file_chooser_folder), &project_manager);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_end(hbox, 15);
    gtk_widget_set_margin_top(hbox, 15);
    gtk_widget_set_margin_bottom(hbox, 15);
    gtk_box_append(GTK_BOX(vbox), hbox);

    project_manager.btn_create = gtk_button_new_with_label(lang_projectm_button2);
    gtk_widget_set_sensitive(GTK_BUTTON(project_manager.btn_create), FALSE);
    gtk_box_append(GTK_BOX(hbox), project_manager.btn_create);
    g_signal_connect(project_manager.btn_create, "clicked",
        G_CALLBACK(project_manager_signal_create_project), &project_manager);

    btn_other_chooser = gtk_button_new_with_label(lang_projectm_button3);
    gtk_box_append(GTK_BOX(hbox), btn_other_chooser);
    g_signal_connect(btn_other_chooser, "clicked",
        G_CALLBACK(project_manager_signal_file_dialog_open_project), &project_manager);

    gtk_window_present(GTK_WINDOW(project_manager.window));
}
