#include "GobuFnListComponentCreate.h"

void GobuFnListComponentCreate(GobuComponentSystem *component, const char *name)
{
    gtk_string_list_append(component->components, name);
    g_hash_table_insert(component->componentTable, name, "");
}
