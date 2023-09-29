#include "GobuContentBrowser.h"
#include "gobu/gobu-gobu.h"

struct _GobuBrowserContent
{
    GtkWidget parent;
};

G_DEFINE_TYPE_WITH_PRIVATE(GobuBrowserContent, gobu_browser_content, GTK_TYPE_BOX);

static void gobu_browser_content_class_init(GobuBrowserContentClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
}

static void gobu_browser_content_init(GobuBrowserContent *self)
{
    GobuBrowserContentPrivate *private = gobu_browser_content_get_instance_private(self);

    private->path_back = g_ptr_array_new();
    private->path_forward = g_ptr_array_new();
    private->path_current = g_strdup(g_build_filename(GobuProjectGetPath(),"Content", NULL));
    private->path_default = g_strdup(private->path_current);
}

GobuBrowserContentPrivate *GobuContentBrowserGet(GobuBrowserContent *browser)
{
    return gobu_browser_content_get_instance_private(browser);
}
