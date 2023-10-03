#include "config.h"
#include "GobuUiWorldEditor.h"
#include "GobuUiMenuBar.h"
#include "GobuWidgets.h"

#include "ContentBrowser/GobuUiContentBrowser.h"
#include "WorlrdViewport/gobu_world_viewport.h"
#include "WorldOutliner/gobu_world_outliner.h"

/* Local function */

static GtkWidget *ToolbarInit(void);
static GtkWidget *DockedModuleInit(void);

GtkWidget *GobuUiWorldEditor(void)
{
    GtkWidget *vbox, *toolbar, *docked;

    // BOX 1
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    {
        toolbar = ToolbarInit();
        gtk_box_append(vbox, toolbar);
    }

    gtk_box_append(vbox, GobuWidgetSeparatorH());

    // BOX 2
    {
        docked = DockedModuleInit();
        gtk_box_append(vbox, docked);
    }

    return vbox;
}

static GtkWidget *ToolbarInit(void)
{
    GtkWidget *toolbar, *btn_toolbar_public, *btn_toolbar_playgame, *btn_toolbar_save, *combo_toolbar_mode;

    toolbar = GobuWidgetToolbarNew();
    {
        btn_toolbar_save = GobuWidgetButtonIconLabelNew("media-removable-symbolic", "Save");
        gtk_box_append(toolbar, btn_toolbar_save);

        GobuWidgetToolbarSeparatorW(toolbar);

        combo_toolbar_mode = gtk_combo_box_text_new();
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_toolbar_mode), "selected", "Selection Mode");
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_toolbar_mode), "tiled", "Tiled Editor");
        gtk_combo_box_set_active(combo_toolbar_mode, 0);
        gtk_box_append(toolbar, combo_toolbar_mode);

        GobuWidgetToolbarSeparatorW(toolbar);

        btn_toolbar_playgame = GobuWidgetButtonIconLabelNew("input-gaming-symbolic", "Preview");
        gtk_box_append(toolbar, btn_toolbar_playgame);

        btn_toolbar_public = GobuWidgetButtonIconLabelNew("send-to-symbolic", "Publish");
        gtk_box_append(toolbar, btn_toolbar_public);
    }

    return toolbar;
}

static GtkWidget *DockedModuleInit(void)
{
    GtkWidget *notebook_docked_left_bottom, *notebook_docked_center, *notebook_docked_left_top, *notebook_docked_right_top;
    GtkWidget *docked, *docked_right, *docked_right_top, *docked_left_top;

    docked = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_resize_start_child(GTK_PANED(docked), FALSE);
    gtk_paned_set_shrink_start_child(GTK_PANED(docked), FALSE);
    gtk_widget_set_vexpand(GTK_WIDGET(docked), TRUE);
    gtk_paned_set_wide_handle(GTK_PANED(docked), TRUE);
    {
        // -------------------------------
        // DOCKED PANEL 1 - Left
        // -------------------------------
        docked_left_top = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
        gtk_paned_set_start_child(GTK_PANED(docked), docked_left_top);

        notebook_docked_left_top = gtk_notebook_new();
        gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_docked_left_top), FALSE);
        gtk_widget_set_size_request(GTK_WIDGET(notebook_docked_left_top), DOCK_R_MIN_SIZE, -1);
        gtk_paned_set_start_child(GTK_PANED(docked_left_top), notebook_docked_left_top);
        {
            GtkWidget *outliner = gobu_world_outliner_new();
            gtk_notebook_append_page(GTK_NOTEBOOK(notebook_docked_left_top), outliner, gtk_label_new("World Outliner"));
        }

        notebook_docked_left_bottom = gtk_notebook_new();
        gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_docked_left_bottom), FALSE);
        gtk_widget_set_size_request(GTK_WIDGET(notebook_docked_left_bottom), DOCK_L_MIN_SIZE, -1);
        gtk_paned_set_end_child(GTK_PANED(docked_left_top), notebook_docked_left_bottom);
        {
            GtkWidget *viewFile = GobuUiContentBrowserNew();
            gtk_notebook_append_page(GTK_NOTEBOOK(notebook_docked_left_bottom), viewFile, gtk_label_new("Content Browser"));
        }

        // -------------------------------
        // Creamos un paned para los otros Docked
        // -------------------------------
        docked_right = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_paned_set_end_child(GTK_PANED(docked), docked_right);
        gtk_paned_set_resize_end_child(GTK_PANED(docked_right), FALSE);
        gtk_paned_set_shrink_end_child(GTK_PANED(docked_right), FALSE);
        gtk_paned_set_wide_handle(GTK_PANED(docked_right), TRUE);
        {
            // -------------------------------
            // DOCKED PANEL 2 - Centro
            // -------------------------------
            notebook_docked_center = gtk_notebook_new();
            gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_docked_center), FALSE);
            gtk_paned_set_start_child(GTK_PANED(docked_right), notebook_docked_center);
            {
                GtkWidget *scene_viewport = gobu_world_viewport_new();
                gtk_notebook_append_page(GTK_NOTEBOOK(notebook_docked_center), scene_viewport, gtk_label_new("Viewport"));
            }

            // -------------------------------
            // DOCKED PANEL 3 - Right
            // -------------------------------
            docked_right_top = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
            gtk_paned_set_end_child(GTK_PANED(docked_right), docked_right_top);

            notebook_docked_right_top = gtk_notebook_new();
            gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook_docked_right_top), FALSE);
            gtk_widget_set_size_request(GTK_WIDGET(notebook_docked_right_top), DOCK_R_MIN_SIZE, -1);
            gtk_paned_set_start_child(GTK_PANED(docked_right_top), notebook_docked_right_top);
            {
                GtkWidget *inspector = gtk_label_new("Inspector");
                gtk_notebook_append_page(GTK_NOTEBOOK(notebook_docked_right_top), inspector, gtk_label_new("Inspector"));
            }
        }
    }

    return docked;
}

/* Public function */

// gboolean gobu_project_editor_load(const gchar *path)
// {
//     GobuEditorContext *ctx = gobu_editor_get_context();
//     g_return_if_fail(ctx);

//     const gchar *folder = g_dirname(path);

//     ctx->project.path = g_strdup(folder);
//     ctx->project.name = g_strdup(g_basename(folder));
//     ctx->browser.path_current = g_strdup(g_build_filename(ctx->project.path, "Content", NULL));
//     ctx->browser.path_default = g_strdup(ctx->browser.path_current);

//     GtkWidget *main = editor_project_init(ctx);
//     gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), main, gtk_label_new(ctx->project.name));

//     GtkWidget *components = gobu_component_editor_new();
//     gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), components, gtk_label_new("Components"));

//     return TRUE;
// }
