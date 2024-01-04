#include "gb_setting.h"
#include "gb_string.h"

struct gb_setting_project_t {
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
    GKeyFile* setting; // glib.h
}project;


bool gb_setting_project_load_from_file(const char* filename)
{
    project.setting = g_key_file_new();

    bool r = g_key_file_load_from_file(project.setting, filename, G_KEY_FILE_NONE, NULL);
    if (r)
    {
        project.name = g_key_file_get_string(project.setting, "project", "name", NULL);
        project.description = g_key_file_get_string(project.setting, "project", "description", NULL);
        project.author = g_key_file_get_string(project.setting, "project", "author", NULL);

        project.width = g_key_file_get_integer(project.setting, "display.resolution", "width", NULL);
        project.height = g_key_file_get_integer(project.setting, "display.resolution", "height", NULL);
        project.mode = g_key_file_get_integer(project.setting, "display.resolution", "mode", NULL);
        project.resizable = g_key_file_get_boolean(project.setting, "display.window", "resizable", NULL);
        project.borderless = g_key_file_get_boolean(project.setting, "display.window", "borderless", NULL);
        project.alwayontop = g_key_file_get_boolean(project.setting, "display.window", "alwayontop", NULL);
        project.transparent = g_key_file_get_boolean(project.setting, "display.window", "transparent", NULL);
        project.nofocus = g_key_file_get_boolean(project.setting, "display.window", "nofocus", NULL);
        project.highdpi = g_key_file_get_boolean(project.setting, "display.window", "highdpi", NULL);
        project.vsync = g_key_file_get_boolean(project.setting, "display.window", "vsync", NULL);
        project.aspectration = g_key_file_get_boolean(project.setting, "display.window", "aspectration", NULL);

        printf("Load project setting: %s\n", project.name);
    }

    return r;
}

GKeyFile* gb_setting_project_setting(void)
{
    return project.setting;
}

void gb_setting_project_unload(void)
{
    g_free(project.name);
    g_free(project.description);
    g_free(project.author);
    g_free(project.license);
    g_free(project.version);
    g_free(project.thumbnail);
    g_key_file_free(project.setting);
}

void gb_setting_project_save(void)
{
    char* file_gobuproject = gb_path_join(gb_project_get_path(), "game.gobuproject", NULL);
    g_key_file_save_to_file(project.setting, file_gobuproject, NULL);
    g_free(file_gobuproject);
}

// --
// SetAPI: Setting Project
// --
void gb_setting_project_set_name(const char* name)
{
    g_key_file_set_string(project.setting, "project", "name", name);
    project.name = g_key_file_get_string(project.setting, "project", "name", NULL);
}

void gb_setting_project_set_description(const char* description)
{
    g_key_file_set_string(project.setting, "project", "description", description);
    project.description = g_key_file_get_string(project.setting, "project", "description", NULL);
}

void gb_setting_project_set_author(const char* author)
{
    g_key_file_set_string(project.setting, "project", "author", author);
    project.author = g_key_file_get_string(project.setting, "project", "author", NULL);
}

void gb_setting_project_set_license(const char* license)
{
    g_key_file_set_string(project.setting, "project", "license", license);
    project.license = g_key_file_get_string(project.setting, "project", "license", NULL);
}

void gb_setting_project_set_version(const char* version)
{
    g_key_file_set_string(project.setting, "project", "version", version);
    project.version = g_key_file_get_string(project.setting, "project", "version", NULL);
}

void gb_setting_project_set_thumbnail(const char* thumbnail)
{
    g_key_file_set_string(project.setting, "project", "thumbnail", thumbnail);
    project.thumbnail = g_key_file_get_string(project.setting, "project", "thumbnail", NULL);
}

void gb_setting_project_set_width(int width)
{
    g_key_file_set_integer(project.setting, "display.resolution", "width", width);
    project.width = g_key_file_get_integer(project.setting, "display.resolution", "width", NULL);
}

void gb_setting_project_set_height(int height)
{
    g_key_file_set_integer(project.setting, "display.resolution", "height", height);
    project.height = g_key_file_get_integer(project.setting, "display.resolution", "height", NULL);
}

void gb_setting_project_set_mode(int mode)
{
    g_key_file_set_integer(project.setting, "display.window", "mode", mode);
    project.mode = g_key_file_get_integer(project.setting, "display.window", "mode", NULL);
}

void gb_setting_project_set_resizable(bool resizable)
{
    g_key_file_set_boolean(project.setting, "display.window", "resizable", resizable);
    project.resizable = g_key_file_get_boolean(project.setting, "display.window", "resizable", NULL);
}

void gb_setting_project_set_borderless(bool borderless)
{
    g_key_file_set_boolean(project.setting, "display.window", "borderless", borderless);
    project.borderless = g_key_file_get_boolean(project.setting, "display.window", "borderless", NULL);
}

void gb_setting_project_set_alwayontop(bool alwayontop)
{
    g_key_file_set_boolean(project.setting, "display.window", "alwayontop", alwayontop);
    project.alwayontop = g_key_file_get_boolean(project.setting, "display.window", "alwayontop", NULL);
}

void gb_setting_project_set_transparent(bool transparent)
{
    g_key_file_set_boolean(project.setting, "display.window", "transparent", transparent);
    project.transparent = g_key_file_get_boolean(project.setting, "display.window", "transparent", NULL);
}

void gb_setting_project_set_nofocus(bool nofocus)
{
    g_key_file_set_boolean(project.setting, "display.window", "nofocus", nofocus);
    project.nofocus = g_key_file_get_boolean(project.setting, "display.window", "nofocus", NULL);
}

void gb_setting_project_set_highdpi(bool highdpi)
{
    g_key_file_set_boolean(project.setting, "display.window", "highdpi", highdpi);
    project.highdpi = g_key_file_get_boolean(project.setting, "display.window", "highdpi", NULL);
}

void gb_setting_project_set_vsync(bool vsync)
{
    g_key_file_set_boolean(project.setting, "display.window", "vsync", vsync);
    project.vsync = g_key_file_get_boolean(project.setting, "display.window", "vsync", NULL);
}

void gb_setting_project_set_aspectration(bool aspectration)
{
    g_key_file_set_boolean(project.setting, "display.window", "aspectration", aspectration);
    project.aspectration = g_key_file_get_boolean(project.setting, "display.window", "aspectration", NULL);
}

// --
// GetAPI: Setting Project
// --
const char* gb_setting_project_name(void)
{
    return gb_strdup(project.name);
}

const char* gb_setting_project_description(void)
{
    return project.description == NULL ? "" : gb_strdup(project.description);
}

const char* gb_setting_project_author(void)
{
    return project.author == NULL ? "" : gb_strdup(project.author);
}

const char* gb_setting_project_license(void)
{
    return project.license == NULL ? "" : gb_strdup(project.license);
}

const char* gb_setting_project_version(void)
{
    return project.version == NULL ? "0.0.1" : gb_strdup(project.version);
}

const char* gb_setting_project_thumbnail(void)
{
    return project.thumbnail == NULL ? "" : gb_strdup(project.thumbnail);
}

int gb_setting_project_width(void)
{
    return project.width;
}

int gb_setting_project_height(void)
{
    return project.height;
}

int gb_setting_project_mode(void)
{
    return project.mode;
}

bool gb_setting_project_resizable(void)
{
    return project.resizable;
}

bool gb_setting_project_borderless(void)
{
    return project.borderless;
}

bool gb_setting_project_alwayontop(void)
{
    return project.alwayontop;
}

bool gb_setting_project_transparent(void)
{
    return project.transparent;
}

bool gb_setting_project_nofocus(void)
{
    return project.nofocus;
}

bool gb_setting_project_highdpi(void)
{
    return project.highdpi;
}

bool gb_setting_project_vsync(void)
{
    return project.vsync;
}

bool gb_setting_project_aspectration(void)
{
    return project.aspectration;
}

