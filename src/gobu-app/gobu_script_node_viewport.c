#include "gobu_script_node_viewport.h"
#include "gobu_gobu_embed.h"

GtkWidget *gobu_script_node_viewport_new(void)
{
    GtkWidget *embed;

    embed = gobu_gobu_embed_new();

    return embed;
}

