#include "gobu_project.h"
#include "binn/binn_json.h"
#include "gobu_utility.h"
#include <glib.h>

typedef struct GProject {
    char* path;
    char* name;
    int width;
    int height;
}GProject;

static GProject project = { 0 };

bool gb_project_load(const char* filename)
{
    bool test = gb_fs_file_exist(filename);

    if (test)
    {
        project.path = gb_path_dirname(filename);
        // project.name = gb_path_basename(project.path);

        char* file_gobuproject = gb_path_join(project.path, "game.gobuproject", NULL);
        binn* game_setting = binn_serialize_from_file(file_gobuproject);
        g_free(file_gobuproject);

        binn* setting = binn_object_object(game_setting, "setting");
        {
            project.width = binn_object_uint32(setting, "width");
            project.height = binn_object_uint32(setting, "height");
            project.name = gb_strdup(binn_object_str(setting, "name"));
        }

        // binn_free(setting);
        binn_free(game_setting);
    }

    return test;
}

const char* gb_project_get_path(void)
{
    const gchar* path = gb_strdup(project.path);
    return path;
}

const char* gb_project_get_name(void)
{
    const gchar* name = gb_strdup(project.name);
    return name;
}
