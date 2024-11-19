
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>

#ifndef CONFIG_H
#define CONFIG_H

#define APPLICATION_ID                  "com.gobuengine.app"
#define GOBU_VERSION_STR                "Gobu2D "
#define GOBU_FILE_PROJECTS_NAME         "projects.bin"
#define GOBU_PROJECT_CONTENT            "Content"
#define GAPP_SCRIPT_LENGUAGE            "lua"
#define GAPP_GAME_FILE_PROJECT          "game.gobuproject"
#define GAPP_ROOT_STR                   "Root"
#define GAPP_TOOLBAR_LEFT_ICON_BROWSER  "system-file-manager-symbolic"
#define GAPP_TOOLBAR_LEFT_ICON_ROOMS    "preferences-desktop-wallpaper-symbolic"
#define GAPP_TOOLBAR_LEFT_ICON_ACTOR    "applications-games-symbolic"
#define GAPP_TOOLBAR_LEFT_ICON_TILE     "x-office-spreadsheet-symbolic"

#define GAPP_COMPS_EMPTY                "entity.empty"
#define GAPP_COMPS_SPRITE_RENDER        "pixio_type_module.pixio_sprite_t"
#define GAPP_COMPS_TILINGS_SPRITE       ""
#define GAPP_COMPS_ANIMATED             ""
#define GAPP_COMPS_AUDIO_LISTENER       ""
#define GAPP_COMPS_SOUND                ""
#define GAPP_COMPS_CAMERA               ""
#define GAPP_COMPS_LIGHT                ""
#define GAPP_COMPS_PARTICLE_SYSTEM      ""
#define GAPP_COMPS_LIGHT                ""
#define GAPP_COMPS_GUI_PANEL            ""
#define GAPP_COMPS_GUI_TEXT             "pixio_type_module.pixio_text_t"
#define GAPP_COMPS_GUI_SHAPE_RECTANGLE  "pixio_type_module.pixio_shape_rec_t"

#define BROWSER_FILE_COMPONENT  ".gcomponent"
#define BROWSER_FILE_SCRIPT     ".lua"
#define BROWSER_FILE_SCENE      ".gscene"
#define BROWSER_FILE_TILEMAP    ".gtilemap"
#define BROWSER_FILE_SPRITE     ".gsprite"
#define BROWSER_FILE_PREFAB     ".gprefab"
#define BROWSER_FILE_ANIMATION  ".ganimation"
#define BROWSER_FILE_TILESET    ".gtileset"
#define BROWSER_FILE_IMAGE      ".png"
#define BROWSER_FILE_AUDIO      ".wav"
#define BROWSER_FILE_FONT       ".ttf"
#define BROWSER_FILE_SHADER     ".glsl"

#endif // CONFIG_H

