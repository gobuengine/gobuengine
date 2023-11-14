#include "project_manager_fn.h"

bool project_manager_fn_create_project(const char* name, const char* path)
{
    bool is_created = false;

    char* project = g_build_filename(path, name, NULL);

    /*NOTE  aqui solo es necesario saber si existe un folder con este nombre */
    if (!g_file_test(project, G_FILE_TEST_IS_DIR))
    {
        char* config_file = g_strdup_printf("{\"setting\":{\"name\":\"%s\", \"width\":1280, \"height\":720}}", name);

        gb_fs_mkdir(project);
        gb_fs_mkdir(g_build_filename(project, "Content", NULL)); // Resource
        gb_fs_mkdir(g_build_filename(project, "Saved", NULL));   // Saved
        gb_fs_write_file(g_build_filename(project, "game.gobuproject", NULL), config_file);

        g_free(config_file);

        is_created = true;
    }

    g_free(project);

    g_info("INFO: Proyecto Creado [%d]\n", is_created);
    return is_created;
}
