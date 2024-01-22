#include "gapp-browser-view-file-menupopup.h"
#include "gapp-browser-view-file-delete.h"
#include "gapp-browser-view-file-selected.h"

/**
 * Abrimos la herramienta para separar un sprites sheets.
 *
 */
static void gapp_browser_view_file_signal_extrasprites(GtkWidget *widget, GappBrowser *browser)
{
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);
    
    GFileInfo *file_info = gapp_browser_view_file_get_first_file_selected(browser);
    GFile *file = G_FILE(g_file_info_get_attribute_object(file_info, "standard::file"));
    gchar *filename = g_file_get_path(file);

    // gbapp_extrasprites_new(filename);

    gtk_popover_popdown(GTK_POPOVER(private->popover));
}

/**
 * Maneja la señal de activación de un popover en el explorador de mundos del editor en Gobu.
 *
 * @param button  El botón que desencadenó la activación del popover.
 * @param data    Datos relacionados con la activación del popover en el explorador de mundos.
 */
void gapp_browser_view_file_signal_menupopup(GtkGesture *gesture, int n_press, double x, double y, GappBrowser *browser)
{
    GtkWidget *widget, *child;

    widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);

    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
    {
        GtkWidget *popover, *box, *item;

        gint position = GPOINTER_TO_UINT(g_object_get_data(child, "position_id"));
        bool is_selected = gtk_selection_model_is_selected(GTK_SELECTION_MODEL(private->selection), position);
        // Si tenemos seleccionado el item aun que tengamos mas de uno no se deseleccionaran, pero
        // si seleccionamos otro item se deseleccionaran todos los que esten seleccionados.
        gtk_selection_model_select_item(GTK_SELECTION_MODEL(private->selection), position, !is_selected);

        private->popover = gtk_popover_new();
        gtk_popover_set_cascade_popdown(GTK_POPOVER(private->popover), TRUE);
        gtk_widget_set_parent(GTK_POPOVER(private->popover), widget);
        // gtk_popover_set_has_arrow(GTK_POPOVER(private->popover), FALSE);
        gtk_popover_set_pointing_to(GTK_POPOVER(private->popover), &(GdkRectangle){x, y, 1, 1});

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_popover_set_child(GTK_POPOVER(private->popover), box);
        {
            if (child != NULL && child != widget)
            {
                GFileInfo *info_file = gapp_browser_view_file_get_first_file_selected(browser);
                gb_return_if_fail(info_file);
                
                GFile *file = G_FILE(g_file_info_get_attribute_object(info_file, "standard::file"));
                gb_return_if_fail(file);

                gchar *filename = g_file_get_path(file);

                item = gtk_button_new_with_label(gb_strdups("Delete [ %s ]", gb_path_basename(filename)));
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_label_set_xalign(gtk_button_get_child(GTK_BUTTON(item)), 0.0f);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_view_file_signal_delete_file), browser);

                if (gb_fs_is_extension(filename, ".png") || gb_fs_is_extension(filename, ".jpg"))
                {
                    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                    item = gtk_button_new_with_label("Extract Sprites");
                    gtk_label_set_xalign(gtk_button_get_child(GTK_BUTTON(item)), 0.0f);
                    gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                    gtk_box_append(GTK_BOX(box), item);
                    g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_view_file_signal_extrasprites), browser);
                }
            }
            else
            {
                item = gtk_button_new_with_label("New Folder");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_label_set_xalign(gtk_button_get_child(GTK_BUTTON(item)), 0.0f);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_signal_create_item_action),
                                 gapp_browser_fn_new_action("New Folder", "Folder name", "", ACTION_CREATE_FOLDER, browser));

                gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

                item = gtk_button_new_with_label("New Level");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_label_set_xalign(gtk_button_get_child(GTK_BUTTON(item)), 0.0f);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_signal_create_item_action),
                                 gapp_browser_fn_new_action("New Level", "Level name", "", ACTION_CREATE_LEVEL, browser));

                item = gtk_button_new_with_label("New Animation Sprite");
                gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
                gtk_label_set_xalign(gtk_button_get_child(GTK_BUTTON(item)), 0.0f);
                gtk_box_append(GTK_BOX(box), item);
                g_signal_connect(item, "clicked", G_CALLBACK(gapp_browser_signal_create_item_action),
                                 gapp_browser_fn_new_action("New AnimationSprite", "Animation Sprite name", "", ACTION_CREATE_ASPRITES, browser));
            }
        }

        gtk_popover_popup(GTK_POPOVER(private->popover));
    }

    if (child == widget)
    {
        gtk_selection_model_unselect_all(GTK_SELECTION_MODEL(private->selection));
    }
}
