/**********************************************************************************
 * gapp-project-settings.h                                                        *
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

#ifndef GAPP_PE_PROJECT_SETTINGS_H
#define GAPP_PE_PROJECT_SETTINGS_H
#include "../project-editor.h"

G_BEGIN_DECLS

typedef struct _GappProjectSettingsPrivate GappProjectSettingsPrivate;

#define GAPP_TYPE_PROJECT_SETTINGS (gapp_project_settings_get_type())
G_DECLARE_FINAL_TYPE(GappProjectSettings, gapp_project_settings, GAPP, PROJECT_SETTINGS, GtkWidget)

struct _GappProjectSettingsPrivate
{
    GtkWidget parent_instance;

    GtkWidget *paned;
    GtkSizeGroup *group_size;
    GtkWidget *setting_props;

    GtkWidget *thumbanil;
    GtkWidget *display_width;
    GtkWidget *display_height;
};

GappProjectSettings *gapp_pe_project_settings_new(void);

GtkWidget *project_setting_group_child_append(const char *label_str, GtkWidget *listbox, GtkWidget *input, GtkOrientation orientation);

GtkWidget *project_setting_group_input_entry(const char *placeholder, const char *tooltip);

G_END_DECLS

#endif // GAPP_PE_PROJECT_SETTINGS_H

