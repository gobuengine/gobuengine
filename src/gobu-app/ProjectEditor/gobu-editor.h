#ifndef __GOBU_EDITOR_H__
#define __GOBU_EDITOR_H__
#include <stdio.h>
#include <gtk/gtk.h>

typedef enum
{
    GOBU_EDITOR_ICON_COMPONENT = 0,
    GOBU_EDITOR_ICON_WORLD,
    GOBU_EDITOR_ICON_ENTITY,
    GOBU_EDITOR_ICON_FOLDER,
    GOBU_EDITOR_ICON_ANIM2D,
    GOBU_EDITOR_ICONS
}GobuEditorIconDefault;

typedef struct _GobuEditorContext
{
    GtkWidget *notebook; // notebook main

    struct
    {
        gchar *path;
        gchar *name;
    } project;

    struct
    {
        gchar *path_default;
        gchar *path_current;
        void (*dir)(const gchar *path);
    } browser;

    struct
    {
        GdkPixbuf *icons[GOBU_EDITOR_ICONS];
    } resource;

    struct
    {
        gchar *path;
    }component;

} GobuEditorContext;

GobuEditorContext *gobu_editor_get_context(void);
const GdkPixbuf *gobu_editor_get_icons(GobuEditorIconDefault icon);

#endif
