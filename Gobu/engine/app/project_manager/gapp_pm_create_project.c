#include "gapp_pm_create_project.h"
#include "gapp_pm_check_project_name_availability.h"
#include "gapp_pm_select_folder_for_project.h"
#include "gapp_pm_open_project.h"

#include "gapp_widget.h"
#include "utility/gb_path.h"
#include "utility/gb_fs.h"
#include "utility/gb_string.h"
#include "utility/gb_log.h"
#include "config.h"

static bool fn_create_project(const char *name, const char *path)
{
    bool is_created = false;

    char *project = gb_path_join(path, name, NULL);

    /*NOTE  aqui solo es necesario saber si existe un folder con este nombre */
    if (!gb_path_is_dir(project))
    {
        char *config_file = gb_strdups("[project]\nname=%s\nversion=0.0.1\n\n[display.resolution]\nwidth=1280\nheight=720\n\n[display.window]\nresizable=true\nhighdpi=true\nspectration=true", name, NULL);

        gb_path_create_new(project);
        gb_path_create_new(gb_path_join(project, FOLDER_CONTENT_PROJECT, NULL)); // Resource
        gb_path_create_new(gb_path_join(project, "Saved", NULL));                // Saved
        gb_fs_write_file(gb_path_join(project, "game.gobuproject", NULL), config_file);

        g_free(config_file);

        is_created = true;
    }

    g_free(project);

    gb_log_info("Proyecto Creado [%d]\n", is_created);
    return is_created;
}

static void gapp_pm_signal_create_project(GtkWidget *widget, GobuProjectManager *ctx)
{
    const gchar *name = gtk_editable_get_text(GTK_EDITABLE(ctx->entry_name));
    const gchar *path = gtk_button_get_label(GTK_BUTTON(ctx->btn_file_chooser));

    if (fn_create_project(name, path))
    {
        gchar *path_str = gb_path_join(path, name, "game.gobuproject", NULL);
        if (gapp_pm_open_project(path_str, gtk_window_get_application(ctx->window)))
        {
            gtk_window_destroy(GTK_WINDOW(ctx->window));
        }
        g_free(path_str);
    }
}

void gapp_pm_signal_dialog_create_project(GtkWidget *widget, GobuProjectManager *ctx)
{
    GtkWidget *vbox, *hbox, *label, *btn_other_chooser, *win;

    win = gtk_window_new();
    gtk_window_set_modal(GTK_WINDOW(win), TRUE);
    gtk_window_set_title(GTK_WINDOW(win), "Create new project");
    gtk_window_set_default_size(GTK_WINDOW(win), 500, 200);
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(win), ctx->window);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gapp_widget_set_margin(GTK_WIDGET(vbox), 15);
    gtk_window_set_child(GTK_WINDOW(win), GTK_WIDGET(vbox));

    label = gtk_label_new(lang_projectm_label2);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(label));

    ctx->entry_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_name), lang_projectm_button1);
    gtk_box_append(GTK_BOX(vbox), ctx->entry_name);
    g_signal_connect(ctx->entry_name, "changed",
                     G_CALLBACK(gapp_pm_check_project_name_availability), ctx);

    label = gtk_label_new(lang_projectm_label1);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(label));

    ctx->btn_file_chooser = gtk_button_new_with_label(g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS));
    gtk_box_append(GTK_BOX(vbox), ctx->btn_file_chooser);
    g_signal_connect(ctx->btn_file_chooser, "clicked",
                     G_CALLBACK(gapp_pm_signal_select_folder_for_project), ctx);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_end(hbox, 15);
    gtk_widget_set_margin_top(hbox, 15);
    gtk_widget_set_margin_bottom(hbox, 15);
    gtk_box_append(GTK_BOX(vbox), hbox);

    ctx->btn_create = gtk_button_new_with_label(lang_projectm_button2);
    gtk_widget_set_sensitive(GTK_WIDGET(ctx->btn_create), FALSE);
    gtk_box_append(GTK_BOX(hbox), ctx->btn_create);
    gtk_widget_add_css_class(ctx->btn_create, "suggested-action");
    g_signal_connect(ctx->btn_create, "clicked",
                     G_CALLBACK(gapp_pm_signal_create_project), ctx);

    gtk_window_present(win);
}
