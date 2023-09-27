#include <gtk/gtk.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>
#include <glib.h>
#include "config.h"
#include "gobu-editor.h"
#include "gobu-editor-menubar.h"
#include "gobu.h"
#include "ProjectManager/gobu-project-manager-dialog.h"

static GobuEditorContext *context;

GobuEditorContext *gobu_editor_get_context(void)
{
    return context;
}

const GdkPixbuf *gobu_editor_get_icons(GobuEditorIconDefault icon)
{
    return context->resource.icons[icon];
}

static void _load_default_resource(void)
{
    const char *const *icons[] = {"component.png", "world.png", "entity.png", "folder.png", "anim2d.png"};
    gchar *path_r = g_get_current_dir();

    for (size_t i = 0; i < GOBU_EDITOR_ICONS; i++)
    {
        gchar *full_path = g_build_filename(path_r, "resource", "icons", icons[i], NULL);
        context->resource.icons[i] = gdk_pixbuf_new_from_file_at_size(full_path, 24, 32, NULL);
        g_assert(context->resource.icons[i]);
        g_free(full_path);
    }
    g_free(path_r);
}

static void gobu_editor_init(void)
{
    context = g_new0(GobuEditorContext, 1);

    gobuc_init();
    _load_default_resource();
}

static void on_activate(GtkApplication *app)
{
    GtkWidget *win = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(win), "GobuEditor");
    gtk_window_set_default_size(GTK_WINDOW(win), 1280, 720);

    gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(gobu_editor_menubar_new()));
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(win), TRUE);

    context->notebook = gtk_notebook_new();
    gtk_window_set_child(GTK_WINDOW(win), context->notebook);

    gtk_window_present(GTK_WINDOW(win));
}