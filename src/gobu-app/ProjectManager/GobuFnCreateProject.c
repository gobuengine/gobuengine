#include "GobuFnCreateProject.h"

gboolean GobuFnCreateProject(const gchar *name, const gchar *path)
{
    gboolean is_created = FALSE;

    gchar *project = g_build_filename(path, name, NULL);

    /*NOTE  aqui solo es necesario saber si existe un folder con este nombre */
    if (!g_file_test(project, G_FILE_TEST_IS_DIR))
    {
        gchar *config_file = g_strdup_printf("{\"setting\":{\"name\":\"%s\", \"width\":1280, \"height\":720}}", name);

        g_mkdir(project, 0);
        g_mkdir(g_build_filename(project, "Content", NULL), 0); // Resource
        g_mkdir(g_build_filename(project, "Saved", NULL), 0); // Saved
        g_file_set_contents(g_build_filename(project, "game.gobuproject", NULL), config_file, strlen(config_file), NULL);

        g_free(config_file);

        is_created = TRUE;
    }

    g_free(project);

    g_info("INFO: Proyecto Creado [%d]\n", is_created);
    return is_created;
}
