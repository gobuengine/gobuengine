#include "GobuProjectEditorResource.h"

static GobuProjectEditorResource *resource;

void GobuProjectEditorResourceInit(void)
{
    resource = g_new0(GobuProjectEditorResource, 1);

    const char *const *icon_key[] = {"component.png", "level.png", "entity.png", "folder.png", "anim2d.png"};
    gchar *path_r = g_get_current_dir();

    for (size_t i = 0; i < GOBU_RESOURCE_ICONS; i++)
    {
        gchar *full_path = g_build_filename(path_r, "resource", "icons", icon_key[i], NULL);
        resource->icons[i] = gdk_pixbuf_new_from_file_at_size(full_path, 24, 32, NULL);
        g_assert(resource->icons[i]);
        g_free(full_path);
    }

    g_free(path_r);
}

void GobuProjectEditorResourceFree(void)
{
    for (size_t i = 0; i < GOBU_RESOURCE_ICONS; i++)
    {
        g_assert(resource->icons[i]);
        g_free(resource->icons[i]);
    }

    g_free(resource);
}

const GdkPixbuf *GobuProjectEditorResourceGetIcon(GobuProjectEditorResourceIcon icon)
{
    return resource->icons[icon];
}
