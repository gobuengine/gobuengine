#include "GobuComponentSystem.h"
#include "GobuUiComponentSystem.h"

static GobuComponentSystem *ComponentSystem;

void GobuComponentSystemInit(GobuProjectEditor *ctx)
{
    ComponentSystem = g_new0(GobuComponentSystem, 1);
    ComponentSystem->ctx = ctx;

    // Deseriazamos el Scripts/game.gcomponent
    ComponentSystem->components = gtk_string_list_new(NULL);
    ComponentSystem->componentTable = g_hash_table_new(g_str_hash, g_str_equal);

    int count = binn_count(ctx->components);
    for (int i = 1; i <= count; i++)
    {
        void *cmp = binn_list_object(ctx->components, i);
        char *keys = binn_object_str(cmp, "name");
        char *code = binn_object_str(cmp, "code");

        gtk_string_list_append(ComponentSystem->components, keys);
        g_hash_table_insert(ComponentSystem->componentTable, keys, code);
    }

    GtkWidget *self = GobuUiComponentSystem(ctx);
    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), self, gtk_label_new("Component System"));
}

void GobuComponentSystemFree(void)
{
    g_hash_table_destroy(ComponentSystem->componentTable);
    g_free(ComponentSystem);
}

GobuComponentSystem *GobuComponentSystemGet(void)
{
    return ComponentSystem;
}

// int count = binn_count(ctx->components);
// for (int i = 1; i <= count; i++)
// {
//     void *cmp = binn_list_object(ctx->components, i);
//     printf("JSON: %s\n", binn_object_str(cmp, "name"));
//     printf("JSON: %s\n", binn_object_str(cmp, "code"));
// }