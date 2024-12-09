
#ifndef GAPP_BROWSER_H
#define GAPP_BROWSER_H
#include "config.h"

G_BEGIN_DECLS

#define GAPP_TYPE_BROWSER (gapp_browser_get_type())
G_DECLARE_FINAL_TYPE(GappBrowser, gapp_browser, GAPP, BROWSER, GtkBox)

GappBrowser *gapp_browser_new(void);

void browserSetFolder(GappBrowser *browser, const gchar *path);

void browserSetFolderContent(GappBrowser *browser);

gchar *gapp_browser_get_current(GappBrowser *browser);

gchar *browserGetContentPath(GappBrowser *browser);

G_END_DECLS

#endif // GAPP_BROWSER_H
