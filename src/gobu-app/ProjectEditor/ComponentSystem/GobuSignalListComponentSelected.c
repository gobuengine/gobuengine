#include "GobuSignalListComponentSelected.h"

void GobuSignalListComponentSelected(GtkListView *self, guint position, GobuComponentSystem *component)
{
    component->position = position;

    gchar *key = gtk_string_list_get_string(component->components, position);
    gchar *buffer = g_hash_table_lookup(component->componentTable, key);
    gtk_text_buffer_set_text(component->sobuffer, buffer, strlen(buffer));
}