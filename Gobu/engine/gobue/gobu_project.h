/**********************************************************************************
 * gb_project.h                                                                 *
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

#ifndef __BUGO_PROJECT_H__
#define __BUGO_PROJECT_H__
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glib.h>

typedef struct GProject {
    char* path;
    char* thumbnail;
    char* name;
    char* description;
    char* author;
    char* license;
    char* version;
    int width;
    int height;
    int mode;
    bool resizable;
    bool borderless;
    bool alwayontop;
    bool transparent;
    bool nofocus;
    bool highdpi;
    bool vsync;
    bool aspectration;

    // file project data
    GKeyFile* setting;
}GProject;

bool gb_project_load(const char* filename);

const char* gb_project_get_path(void);

const char* gb_project_get_name(void);

GKeyFile* gb_project_get_setting(void);

void gb_project_config_unload(void);
void gb_project_config_save(void);
void gb_project_config_set_name(const char* name);
void gb_project_config_set_description(const char* description);
void gb_project_config_set_author(const char* author);
void gb_project_config_set_license(const char* license);
void gb_project_config_set_version(const char* version);
void gb_project_config_set_thumbnail(const char* thumbnail);
void gb_project_config_set_width(int width);
void gb_project_config_set_height(int height);
void gb_project_config_set_mode(int mode);
void gb_project_config_set_resizable(bool resizable);
void gb_project_config_set_borderless(bool borderless);
void gb_project_config_set_alwayontop(bool alwayontop);
void gb_project_config_set_transparent(bool transparent);
void gb_project_config_set_nofocus(bool nofocus);
void gb_project_config_set_highdpi(bool highdpi);
void gb_project_config_set_vsync(bool vsync);
void gb_project_config_set_aspectration(bool aspectration);

const char* gb_project_config_get_name(void);
const char* gb_project_config_get_description(void);
const char* gb_project_config_get_author(void);
const char* gb_project_config_get_license(void);
const char* gb_project_config_get_version(void);
const char* gb_project_config_get_thumbnail(void);
int gb_project_config_get_width(void);
int gb_project_config_get_height(void);
int gb_project_config_get_mode(void);
bool gb_project_config_get_resizable(void);
bool gb_project_config_get_borderless(void);
bool gb_project_config_get_alwayontop(void);
bool gb_project_config_get_transparent(void);
bool gb_project_config_get_nofocus(void);
bool gb_project_config_get_highdpi(void);
bool gb_project_config_get_vsync(void);
bool gb_project_config_get_aspectration(void);


#endif // __BUGO_PROJECT_H__
