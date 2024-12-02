
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>
#include "gapp_common.h"

#ifndef CONFIG_H
#define CONFIG_H

#define APPLICATION_ID                  "com.gobuengine.app"
#define GAPP_VERSION_STR                "Gobu2D "
#define GAPP_FILE_PROJECTS_NAME         "projects.bin"
#define GAPP_PROJECT_CONTENT            "Content"
#define GAPP_SCRIPT_LENGUAGE            "lua"
#define GAPP_ROOT_STR                   "Root"
#define GAPP_TOOLBAR_LEFT_ICON_BROWSER  "system-file-manager-symbolic"
#define GAPP_TOOLBAR_LEFT_ICON_ROOMS    "preferences-desktop-wallpaper-symbolic"
#define GAPP_TOOLBAR_LEFT_ICON_ACTOR    "applications-games-symbolic"
#define GAPP_TOOLBAR_LEFT_ICON_TILE     "x-office-spreadsheet-symbolic"

#define GAPP_COMPS_EMPTY                "entity.empty"
#define GAPP_COMPS_SPRITE_RENDER        "pixio_type_module.pixio_sprite_t"
#define GAPP_COMPS_TILINGS_SPRITE       ""
#define GAPP_COMPS_ANIMATED             "pixio_type_module.pixio_sprite_t pixio_type_module.pixio_animated_t"
#define GAPP_COMPS_AUDIO_LISTENER       ""
#define GAPP_COMPS_SOUND                ""
#define GAPP_COMPS_CAMERA               ""
#define GAPP_COMPS_LIGHT                ""
#define GAPP_COMPS_PARTICLE_SYSTEM      ""
#define GAPP_COMPS_LIGHT                ""
#define GAPP_COMPS_GUI_PANEL            ""
#define GAPP_COMPS_GUI_TEXT             "pixio_type_module.pixio_text_t"
#define GAPP_COMPS_GUI_SHAPE_RECTANGLE  "pixio_type_module.pixio_shape_rec_t"

#define GAPP_BROWSER_FILE_COMPONENT  ".component"
#define GAPP_BROWSER_FILE_SCRIPT     ".lua"
#define GAPP_BROWSER_FILE_SCENE      ".scene"
#define GAPP_BROWSER_FILE_TILEMAP    ".tilemap"
#define GAPP_BROWSER_FILE_SPRITE     ".sprite"
#define GAPP_BROWSER_FILE_PREFAB     ".prefab"
#define GAPP_BROWSER_FILE_ANIMATION  ".anim"
#define GAPP_BROWSER_FILE_TILESET    ".tileset"
#define GAPP_BROWSER_FILE_IMAGE      ".png"
#define GAPP_BROWSER_FILE_IMAGE2     ".jpg"
#define GAPP_BROWSER_FILE_AUDIO      ".wav"
#define GAPP_BROWSER_FILE_FONT       ".ttf"
#define GAPP_BROWSER_FILE_SHADER     ".glsl"

#define GAPP_PROJECT_MANAGER_PREVIEW "sample.png"
#define GAPP_PROJECT_MANAGER_ICON    "image-missing-symbolic"
#define GAPP_PROJECT_MANAGER_FILE    "game.gobuproject"

#endif // CONFIG_H

