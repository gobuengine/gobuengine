
#include "raylib.h"

// Enable waiting for events on pixi_render_end(), no automatic event polling
void EnableEventWaiting(void)
{
    CORE.Render.eventWaiting = true;
}

// Disable waiting for events on pixi_render_end(), automatic events polling
void DisableEventWaiting(void)
{
    CORE.Render.eventWaiting = false;
}