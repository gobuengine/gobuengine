#include "gb_project.h"
#include "gb_setting_project.h"
#include "utility/gb_path.h"
#include "utility/gb_fs.h"
#include "utility/gb_string.h"

static char *project_path = NULL;

bool gb_project_load_from_file_from_file(const char *filename)
{
    bool test = gb_fs_file_exist(filename);
    if (test)
    {
        project_path = gb_strdup(gb_path_dirname(filename));
        char *file_gobuproject = gb_path_join(project_path, "game.gobuproject", NULL);
        gb_setting_project_load_from_file(file_gobuproject);
        g_free(file_gobuproject);
    }

    return test;
}

const char *gb_project_get_path(void)
{
    return project_path;
}

void gb_project_unload(void)
{
    g_free(project_path);
    project_path = NULL;

    gb_setting_project_unload();
}
