#ifndef GB_SETTING_PROJECT_H
#define GB_SETTING_PROJECT_H

#include <stdbool.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

    bool gb_setting_project_load_from_file(const char* filename);

    GKeyFile* gb_setting_project_setting(void);

    void gb_setting_project_unload(void);
    void gb_setting_project_save(void);

    // --
    // SetAPI: Setting Project
    // --
    void gb_setting_project_set_name(const char* name);
    void gb_setting_project_set_description(const char* description);
    void gb_setting_project_set_author(const char* author);
    void gb_setting_project_set_license(const char* license);
    void gb_setting_project_set_version(const char* version);
    void gb_setting_project_set_thumbnail(const char* thumbnail);
    void gb_setting_project_set_width(int width);
    void gb_setting_project_set_height(int height);
    void gb_setting_project_set_mode(int mode);
    void gb_setting_project_set_resizable(bool resizable);
    void gb_setting_project_set_borderless(bool borderless);
    void gb_setting_project_set_alwayontop(bool alwayontop);
    void gb_setting_project_set_transparent(bool transparent);
    void gb_setting_project_set_nofocus(bool nofocus);
    void gb_setting_project_set_highdpi(bool highdpi);
    void gb_setting_project_set_vsync(bool vsync);
    void gb_setting_project_set_aspectration(bool aspectration);

    // --
    // GetAPI: Setting Project
    // --
    const char* gb_setting_project_name(void);
    const char* gb_setting_project_description(void);
    const char* gb_setting_project_author(void);
    const char* gb_setting_project_license(void);
    const char* gb_setting_project_version(void);
    const char* gb_setting_project_thumbnail(void);
    int gb_setting_project_width(void);
    int gb_setting_project_height(void);
    int gb_setting_project_mode(void);
    bool gb_setting_project_resizable(void);
    bool gb_setting_project_borderless(void);
    bool gb_setting_project_alwayontop(void);
    bool gb_setting_project_transparent(void);
    bool gb_setting_project_nofocus(void);
    bool gb_setting_project_highdpi(void);
    bool gb_setting_project_vsync(void);
    bool gb_setting_project_aspectration(void);

#ifdef __cplusplus
}
#endif

#endif // GB_SETTING_PROJECT_H

