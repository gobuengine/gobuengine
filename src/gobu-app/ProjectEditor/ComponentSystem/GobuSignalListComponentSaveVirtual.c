#include "GobuSignalListComponentSaveVirtual.h"

void GobuSignalListComponentSaveVirtual(GtkTextBuffer *self, GobuComponentSystem *component)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(component->sobuffer, &start, &end);

    gchar *buffer = gtk_text_buffer_get_text(component->sobuffer, &start, &end, TRUE);

    gchar *key = gtk_string_list_get_string(component->components, component->position);
    g_hash_table_replace(component->componentTable, key, buffer);
}
