#include "GobuFnContentBrowserGetIconFile.h"
#include "GobuProjectEditorResource.h"

void GobuFnContentBrowserGetIconFile(GtkWidget *image, GFileInfo *info_file)
{
    GIcon *icon;
    const char *ext_file = g_file_info_get_content_type(info_file);

    if (g_strcmp0(ext_file, ".png") == 0 || g_strcmp0(ext_file, ".jpg") == 0)
    {
        GFile *file = G_FILE(g_file_info_get_attribute_object(info_file, "standard::file"));
        gtk_image_set_from_file(image, g_file_get_path(file));
        return;
    }
    else if (g_strcmp0(ext_file, ".gcomponent") == 0)
    {
        icon = GobuProjectEditorResourceGetIcon(GOBU_RESOURCE_ICON_COMPONENT);
    }
    else if (g_strcmp0(ext_file, ".glevel") == 0)
    {
        icon = GobuProjectEditorResourceGetIcon(GOBU_RESOURCE_ICON_LEVEL);
    }
    else if (g_strcmp0(ext_file, ".gentity") == 0)
    {
        icon = GobuProjectEditorResourceGetIcon(GOBU_RESOURCE_ICON_ENTITY);
    }
    else
    {
        icon = g_file_info_get_icon(info_file);
    }

    gtk_image_set_from_gicon(image, icon);
}
