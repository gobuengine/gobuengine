/**********************************************************************************
 * gobu.h                                                          *
 **********************************************************************************
 * GOBU ENGINE                                                                    *
 * https://gobuengine.org                                                         *
 **********************************************************************************
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

#ifndef __GOBU_H__
#define __GOBU_H__
#include <glib.h>
#include <stdint.h>
#include "thirdparty/binn/binn.h"
#include "thirdparty/flecs/flecs.h"

#define GobuEcsEntity ecs_entity_t
#define GobuEcsWorld ecs_world_t

typedef struct GobuColor
{
    float r, g, b, a;
} GobuColor;

typedef struct GobuTexture
{
    unsigned int id;
    int width, height;
} GobuTexture;

typedef struct GobuRectangle
{
    float x, y, w, h;
} GobuRectangle;

typedef struct GobuVec2
{
    float x, y;
} GobuVec2;

typedef struct GobuCamera
{
    GobuVec2 scale;
    GobuVec2 pos;
    GobuVec2 target;
    GobuVec2 offset;
    float rotation;
    float zoom;
} GobuCamera;

const char *GobuJsonStringify(binn *b);
binn *GobuJsonParse(char *json_string);
binn *GobuJsonLoadFromFile(const char *filename);
gboolean GobuJsonSaveToFile(binn *b, const char *filename);

gboolean GobuProjectLoad(const gchar *path);
const char *GobuProjectGetPath(void);
const char *GobuProjectGetName(void);

void GobuRenderInit(void);
void GobuRenderShutdown(void);
void GobuRenderFrameBegin(int width, int height, GobuColor color);
void GobuRenderFrameEnd(int width, int height);
void GobuRenderViewport(int x, int y, int width, int height);
void GobuRenderProject(float left, float right, float top, float bottom);
void GobuRenderClearColor(GobuColor color);
void GobuRenderClear(void);
void GobuRenderSetColor(GobuColor color);
void GobuRenderResetColor(void);
void GobuRenderSetRotate(float theta, float x, float y);
void GobuRenderSetScale(float sx, float sy, float x, float y);
void GobuRenderSetTranslate(float x, float y);

void GobuShapeDrawFilledRect(GobuRectangle rect, GobuVec2 scale, GobuVec2 origin, float rotation, GobuColor color);
void GobuShapeDrawCheckboard(int width, int height, int screen_width, int screen_height);

void GobuTextureDrawTexture(GobuTexture texture, GobuRectangle source, GobuRectangle dest, GobuColor tint);
GobuTexture GobuTextureLoadFile(const char *filename);
void GobuTextureFree(GobuTexture texture);

GobuColor GobuColorRGBToFloat(uint8_t r, uint8_t g, uint8_t b);

GobuEcsWorld *GobuEcsWorldInit(void);
void GobuEcsWorldFree(GobuEcsWorld *world);
GobuEcsEntity GobuEcsEntityNew(const char *name);

#endif // __GOBU_H__