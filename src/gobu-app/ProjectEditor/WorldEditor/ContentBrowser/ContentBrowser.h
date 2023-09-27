#ifndef __BROWSER_BROWSER_H__
#define __BROWSER_BROWSER_H__
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GobuBrowserContentPrivate GobuBrowserContentPrivate;

#define GTK_TYPE_GOBU_BROWSER_CONTENT (gobu_browser_content_get_type())
G_DECLARE_FINAL_TYPE(GobuBrowserContent, gobu_browser_content, GTK, GOBU_BROWSER_CONTENT, GtkBox)

typedef enum
{
    BROWSER_CONTENT_SPRITE_FILE,
    BROWSER_CONTENT_TILED_FILE
} BrowserContentTypeFile;

struct _GobuBrowserContentPrivate
{
    gchar *path_default;
    gchar *path_current;

    GPtrArray *path_back;
    GPtrArray *path_forward;

    GtkWidget *btn_nav_home;
    GtkWidget *btn_nav_back;
    GtkWidget *btn_nav_forward;

    GtkListItemFactory *factory;
    GtkDirectoryList *directory_list;
    GtkSingleSelection *selection;
    GtkWidget *popover;
};

GobuBrowserContent *gobu_browser_content_new(void);
void gobu_browser_content_set_folder_dir(GobuBrowserContent *self, const gchar *path);

G_END_DECLS

#endif // __BROWSER_BROWSER_H__
