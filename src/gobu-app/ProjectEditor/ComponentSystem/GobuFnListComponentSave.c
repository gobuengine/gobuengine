#include "GobuFnListComponentSave.h"

void GobuFnListComponentSave(GobuComponentSystem *component)
{
    binn *list = binn_list();

    for(int i = 0; ;i++) {
        gchar *keys = gtk_string_list_get_string(component->components, i);
        if(keys == NULL) break;

        binn *obj = binn_object();
        binn_object_set_str(obj, "name", keys);
        binn_object_set_str(obj, "code", g_hash_table_lookup(component->componentTable, keys));
        binn_list_add_object(list, obj);
        binn_free(obj);
    }

    gchar *gcomponent = g_build_filename(GobuProjectGetPath(), "Scripts", "game.gcomponent", NULL);
    GobuJsonSaveToFile(list, gcomponent);
    g_free(gcomponent);
    
    binn_free(list);    
}
