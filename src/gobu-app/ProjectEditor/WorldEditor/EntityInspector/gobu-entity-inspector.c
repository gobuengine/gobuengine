#include "EntityInspector.h"

GtkWidget *gobu_entity_inspector_new(void)
{
    GtkWidget *inspector = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    return inspector;
}