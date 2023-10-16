#include "gobu_editor_world.h"
#include "gobu_editor_world_browser.h"
#include "gobu_editor_world_inspector.h"
#include "gobu_editor_world_outliner.h"
#include "gobu_editor_world_spriteanim.h"
#include "gobu_editor_world_tileeditor.h"
#include "gobu_editor_world_timeline.h"
#include "gobu_editor_world_viewport.h"
#include "gobu_widget.h"
#include "config.h"

typedef enum
{
    DOCKED_LEFT_TOP,
    DOCKED_LEFT_BOTTOM,
    DOCKED_RIGHT_TOP,
    DOCKED_RIGHT_BOTTOM,
    DOCKED_LEFT,
    DOCKED_RIGHT,
    DOCKED_CENTER,
    DOCKED_MAIN,
    DOCKED_ORIENTATION_LAST
} DOCKED_ORIENTATION;

GtkWidget* gobu_editor_world_new(void)
{
    GtkWidget* vbox, * hbox;
    GtkWidget* toolbar, * item;
    GtkWidget* notebook_docked[DOCKED_ORIENTATION_LAST];
    GtkWidget* paned_docked[DOCKED_ORIENTATION_LAST];

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    // TOOLBAR
    {
        toolbar = gobu_widget_toolbar_new();
        gtk_box_append(vbox, toolbar);
        {
            item = gobu_widget_button_new_icon_with_label("media-removable-symbolic", "Save");
            gtk_box_append(toolbar, item);
            gobu_widget_toolbar_separator_new(toolbar);

            item = gtk_combo_box_text_new();
            gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(item), "selected", "Selection Mode");
            gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(item), "tiled", "Tiled Editor");
            gtk_combo_box_set_active(item, 0);
            gtk_box_append(toolbar, item);
            gobu_widget_toolbar_separator_new(toolbar);

            item = gobu_widget_button_new_icon_with_label("input-gaming-symbolic", "Preview");
            gtk_box_append(toolbar, item);
            gobu_widget_toolbar_separator_new(toolbar);

            item = gobu_widget_button_new_icon_with_label("send-to-symbolic", "Publish");
            gtk_box_append(toolbar, item);
            gobu_widget_toolbar_separator_new(toolbar);
        }
    }

    gtk_box_append(vbox, gobu_widget_separator_h());

    // MAIN DOCKED
    {
        paned_docked[DOCKED_MAIN] = gobu_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
        gtk_box_append(vbox, paned_docked[DOCKED_MAIN]);
        {
            paned_docked[DOCKED_LEFT] = gobu_widget_paned_new_with_notebook(GTK_ORIENTATION_VERTICAL, TRUE,
                gtk_label_new("World Outliner"), gobu_editor_world_outliner_new(),
                gtk_label_new("Content Browser"), gobu_editor_world_browser_new());
            gtk_paned_set_start_child(GTK_PANED(paned_docked[DOCKED_MAIN]), paned_docked[DOCKED_LEFT]);

            paned_docked[DOCKED_CENTER] = gobu_widget_paned_new(GTK_ORIENTATION_HORIZONTAL, FALSE);
            gtk_paned_set_end_child(GTK_PANED(paned_docked[DOCKED_MAIN]), paned_docked[DOCKED_CENTER]);

            gtk_paned_set_start_child(GTK_PANED(paned_docked[DOCKED_CENTER]), gobu_editor_world_viewport_new());

            paned_docked[DOCKED_RIGHT] = gobu_widget_paned_new_with_notebook(GTK_ORIENTATION_VERTICAL, TRUE,
                gtk_label_new("Inspector"), gtk_label_new("Entity Inspector"),
                NULL, NULL);
            gtk_paned_set_end_child(GTK_PANED(paned_docked[DOCKED_CENTER]), paned_docked[DOCKED_RIGHT]);
        }
    }

    return vbox;
}
