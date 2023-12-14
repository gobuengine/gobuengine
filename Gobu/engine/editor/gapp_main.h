#ifndef __GOBU_EDITOR_H__
#define __GOBU_EDITOR_H__
#include "config.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "../gobu/gobu.h"

#include "gapp_widget.h"
#include "gapp_tool_console.h"

typedef enum
{
    GOBU_RESOURCE_ICON_COMPONENT = 0,
    GOBU_RESOURCE_ICON_LEVEL,
    GOBU_RESOURCE_ICON_ENTITY,
    GOBU_RESOURCE_ICON_FOLDER,
    GOBU_RESOURCE_ICON_ANIM2D,
    GOBU_RESOURCE_ICONS
} GobuAppResourceIcon;

typedef enum
{
    GAPP_NOTEBOOK_DEFAULT,
    GAPP_NOTEBOOK_BOTTOM,
    GAPP_NOTEBOOK_NUMS
}enumGappNotebook;

typedef struct {
    GtkWidget* window;
    GtkWidget* notebook[GAPP_NOTEBOOK_NUMS];
    GtkWidget* console;
    // ecs_world_t *world[MAX_WORLD_OPEN];

    struct
    {
        GdkPixbuf* icons[GOBU_RESOURCE_ICONS];
    } resource;

    struct
    {
        GtkSourceLanguage* language;
        GtkSourceStyleScheme* scheme;
        binn* data;
    } setting;
}GAPP;

#endif
