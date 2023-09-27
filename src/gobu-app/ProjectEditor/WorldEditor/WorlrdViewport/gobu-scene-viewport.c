#include "WorlrdViewport.h"
#include "Widgets/Widgets.h"

// static ErmineScene *scene_main;

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void on_update(GtkWidget *viewport)
{
    // ermine_scene__update(scene_main);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void on_draw(GtkWidget *viewport)
{
    // ermine_scene__draw(scene_main);
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static void on_start(GtkWidget *viewport)
{
    // scene_main = ermine_scene_new();
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

static gboolean drag_drop(GtkDropTarget *target, const GValue *value, double x, double y)
{
    printf("Drop target: %f, %f\n", x, y);
    return TRUE;
}

/**
 * gtk_list_item_get_position: (attributes org.gtk.Method.get_property=position)
 * @self: a `GtkListItem`
 *
 * Gets the position in the model that @self currently displays.
 *
 * If @self is unbound, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of this item
 */

GtkWidget *gobu_scene_viewport_new(void)
{
    GtkWidget *box, *toolbar, *btn_save, *btn_play;
    GtkDropTarget *target;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    {
        toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
        gtk_box_append(box, toolbar);
        gtk_widget_set_margin_start(toolbar, 2);
        gtk_widget_set_margin_end(toolbar, 2);
        gtk_widget_set_margin_top(toolbar, 2);
        gtk_widget_set_margin_bottom(toolbar, 2);
        {
            // btn_save = gobu_widget_button_icon_label_new("media-removable-symbolic", "Save");
            // gtk_box_append(GTK_BOX(toolbar), btn_save);

            // gtk_box_append(toolbar, gtk_separator_new(GTK_ORIENTATION_VERTICAL));

            // btn_play = gtk_button_new_from_icon_name("media-playback-start");
            // gtk_button_set_has_frame(btn_play, FALSE);
            // gtk_box_append(GTK_BOX(toolbar), btn_play);
        }

        GtkWidget *viewport = gtk_gobu_embed_new();
        // ermine_embed_set_color(viewport, RAYWHITE);
        g_signal_connect(viewport, "ermine-start", on_start, NULL);
        g_signal_connect(viewport, "ermine-draw", on_draw, NULL);
        g_signal_connect(viewport, "ermine-update", on_update, NULL);
        gtk_box_append(box, viewport);

        target = gtk_drop_target_new(G_TYPE_STRING, GDK_ACTION_MOVE);
        g_signal_connect(target, "drop", G_CALLBACK(drag_drop), viewport);
        gtk_widget_add_controller(viewport, GTK_EVENT_CONTROLLER(target));
    }

    return box;
}