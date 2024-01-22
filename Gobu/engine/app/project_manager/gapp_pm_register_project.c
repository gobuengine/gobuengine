#include "gapp_pm_register_project.h"
#include "utility/gb_path.h"
#include "utility/gb_fs.h"

static void register_project_save(binn *list);
static binn *register_project_load_valid(binn *list);
static bool register_project_exist(binn *list, const char *path);

binn *gapp_pm_register_project_get_list(void)
{
    char *path_config = gb_path_join(gb_path_user_config_dir(), "gobu-engine", "project-list.json", NULL);
    binn *list = binn_serialize_from_file(path_config);
    g_free(path_config);

    if (binn_count(list) == 0)
        list = binn_list();
    else
        list = register_project_load_valid(list);

    return list;
}

void gapp_pm_register_project_set(const char *path)
{
    binn *list = gapp_pm_register_project_get_list();
    if (!register_project_exist(list, path))
    {
        binn *obj = binn_object();
        binn_object_set_str(obj, "path", path);
        binn_list_add_object(list, obj);
        binn_free(obj);
    }

    register_project_save(list);
}

static bool register_project_exist(binn *list, const char *path)
{
    int count = binn_count(list);
    for (int i = 1; i <= count; i++)
    {
        void *obj = binn_list_object(list, i);
        char *path_s = binn_object_str(obj, "path");

        if (strcmp(path_s, path) == 0)
            return TRUE;
    }
    // binn_free(list);

    return FALSE;
}

static void register_project_save(binn *list)
{
    char *path_config = gb_path_join(gb_path_user_config_dir(), "gobu-engine", "project-list.json", NULL);
    binn_deserialize_from_file(list, path_config);
    g_free(path_config);
}

static binn *register_project_load_valid(binn *list)
{
    binn *list_new = binn_list();

    int count = binn_count(list);
    for (int i = 1; i <= count; i++)
    {
        void *obj = binn_list_object(list, i);
        char *path_s = binn_object_str(obj, "path");

        if (gb_fs_file_exist(path_s))
            binn_list_add_object(list_new, obj);

        // binn_free(obj);
    }

    return list_new;
}
