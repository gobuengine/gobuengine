#include "GobuSignalListComponent.h"

void GobuSignalListComponentSetup(GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data)
{
    GtkWidget *box, *icon, *label;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_list_item_set_child(listitem, box);

    icon = gtk_image_new_from_icon_name("application-x-addon-symbolic");
    gtk_box_append(box, icon);

    label = gtk_label_new(NULL);
    gtk_box_append(box, label);
}

void GobuSignalListComponentBind(GtkSignalListItemFactory *self, GtkListItem *listitem, gpointer user_data)
{
    GtkWidget *box = gtk_list_item_get_child(listitem);
    GtkStringObject *strobj = gtk_list_item_get_item(listitem);

    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_next_sibling(icon);

    gtk_label_set_text(GTK_LABEL(label), gtk_string_object_get_string(strobj));
}