#include "gobu_editor_world_viewport.h"
#include "gobu_gobu_embed.h"

static void viewport_render()
{
    static float rotation;
    rotation += 0.5f;

    DrawText("Congrats! You created your first window!", 190, 200, 20, BLUE);
    gobu_shape_draw_filled_rect((Rectangle){100, 100, 100, 100}, (Vector2){1.0f, 1.0f}, (Vector2){0.5f, 0.5f}, rotation, RED);
}

GtkWidget *gobu_editor_world_viewport_new(void)
{
    GtkWidget *embed = gobu_gobu_embed_new();
    g_signal_connect(embed, "gobu-embed-render", G_CALLBACK(viewport_render), NULL);

    return embed;
}

