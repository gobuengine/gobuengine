/**********************************************************************************
 * browser.h                                                                      *
 * ********************************************************************************
 * GOBU ENGINE                                                                    *
 * https://gobuengine.org                                                         *
 * ********************************************************************************
 * Copyright (c) 2023 Jhonson Ozuna Mejia                                         *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                                *
 *                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
 * SOFTWARE.                                                                      *
 **********************************************************************************/

#ifndef GAPP_BROWSER_H
#define GAPP_BROWSER_H
#include "config.h"

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

G_BEGIN_DECLS

#define GAPP_TYPE_BROWSER (gapp_browser_get_type())
G_DECLARE_FINAL_TYPE(GappBrowser, gapp_browser, GAPP, BROWSER, GtkBox)

GappBrowser *gapp_browser_new(void);

void gapp_browser_set_folder(GappBrowser *browser, const gchar *path);

gchar *gapp_browser_get_current_folder(GappBrowser *browser);

gchar *gapp_browser_get_content_path(GappBrowser *browser);

G_END_DECLS

#endif // GAPP_BROWSER_H
