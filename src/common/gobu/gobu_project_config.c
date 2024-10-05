#include "gobu_project_config.h"
#include <glib.h>

#define DEFAULT_VIEWPORT_WIDTH 1024
#define DEFAULT_VIEWPORT_HEIGHT 720
#define DEFAULT_SAMPLING 1
#define DEFAULT_GRAVITY 980
#define DEFAULT_WINDOW_MODE 0

// Inicialización correcta de variables globales
static gchar *path_project = NULL;
// static gchar *path_project_file = NULL;
static GKeyFile *setting = NULL;
static gobuProjectConfig *project_config = NULL;
static gdouble vec4_type[] = {0.0, 0.0, 0.0, 1.0};
static gint vec2_type[] = {0, -1};
static gsize vec2_type_length = G_N_ELEMENTS(vec2_type);
static gsize vec4_type_length = G_N_ELEMENTS(vec4_type);

/**
 * Inicializa la configuración del proyecto.
 *
 * Esta función inicializa la configuración del proyecto, opcionalmente cargándola
 * desde un archivo. Si la carga falla, la función retorna FALSE sin establecer
 * una configuración predeterminada.
 *
 * @param project_path La ruta al archivo de configuración del proyecto.
 * @param load_from_file Si es TRUE, intenta cargar la configuración desde el archivo.
 * @return gboolean TRUE si la inicialización fue exitosa, FALSE en caso contrario.
 */
gboolean gobu_project_config_init(const gchar *project_path, gboolean load_from_file)
{
    g_return_val_if_fail(project_path != NULL, FALSE);

    // Liberar la memoria existente y asignar la nueva ruta
    g_free(path_project);
    path_project = g_strdup(project_path);

    if (path_project == NULL)
    {
        g_critical("Failed to allocate memory for project path");
        return FALSE;
    }

    if (load_from_file)
    {
        if (g_file_test(path_project, G_FILE_TEST_EXISTS))
        {
            if (!gobu_project_config_load_from_file())
            {
                g_warning("Failed to load project config from file: %s", path_project);
                return FALSE;
            }
            g_info("Project configuration loaded successfully from: %s", path_project);
        }
        else
        {
            g_warning("Project file does not exist: %s", path_project);
            return FALSE;
        }
    }
    else
    {
        g_info("Skipping file load. Using existing configuration.");
    }

    return TRUE;
}

/**
 *
 */
gboolean gobu_project_config_load_from_file(void)
{
    g_return_val_if_fail(path_project != NULL, FALSE);

    if (project_config)
    {
        g_free(project_config);
    }
    project_config = g_new0(gobuProjectConfig, 1);

    if (setting)
    {
        g_key_file_free(setting);
    }
    setting = g_key_file_new();

    GError *error = NULL;
    if (!g_key_file_load_from_file(setting, path_project, G_KEY_FILE_NONE, &error))
    {
        g_warning("Error loading project config: %s", error->message);
        g_error_free(error);
        return FALSE;
    }

// Uso de macros para simplificar la lectura de valores
#define D(value, default) (value != NULL ? value : default)
#define GET_VEC2(section, key)                                                                     \
    ({                                                                                             \
        gint *value = g_key_file_get_integer_list(setting, section, key, &vec2_type_length, NULL); \
        value == NULL ? &vec2_type : value;                                                        \
    })
#define GET_VEC4(section, key)                                                                       \
    ({                                                                                               \
        gdouble *value = g_key_file_get_double_list(setting, section, key, &vec4_type_length, NULL); \
        value == NULL ? &vec4_type : value;                                                          \
    })
#define GET_STRING(section, key) g_key_file_get_string(setting, section, key, NULL)
#define GET_INT(section, key) g_key_file_get_integer(setting, section, key, NULL)
#define GET_BOOL(section, key) g_key_file_get_boolean(setting, section, key, NULL)
#define GET_DOUBLE(section, key) g_key_file_get_double(setting, section, key, NULL)

    project_config->name = D(GET_STRING("application.about", "name"), "");
    project_config->version = D(GET_STRING("application.about", "version"), "0.0.0.1");
    project_config->description = D(GET_STRING("application.about", "description"), "");
    project_config->id = D(GET_STRING("application.about", "id"), "");
    project_config->author = D(GET_STRING("application.about", "author"), "");
    project_config->email = D(GET_STRING("application.about", "email"), "");
    project_config->icon = D(GET_STRING("application.about", "icon"), "");
    project_config->first_scene = D(GET_STRING("application.about", "first_scene"), "");

    project_config->viewport_width = GET_INT("display.viewport", "width");
    project_config->viewport_height = GET_INT("display.viewport", "height");
    project_config->sampling = GET_INT("display.viewport", "sampling");

    project_config->background_color = GET_VEC4("window", "background-color");
    project_config->window_width = GET_INT("window", "width");
    project_config->window_height = GET_INT("window", "height");
    project_config->window_mode = GET_INT("window", "mode");
    project_config->window_resizable = GET_BOOL("window", "resizable");
    project_config->window_borderless = GET_BOOL("window", "borderless");
    project_config->window_alway_on_top = GET_BOOL("window", "alway_on_top");
    project_config->window_transparent = GET_BOOL("window", "transparent");
    project_config->window_no_focus = GET_BOOL("window", "no_focus");
    project_config->window_high_dpi = GET_BOOL("window", "highdpi");
    project_config->window_vsync = GET_BOOL("window", "vsync");
    project_config->window_aspect_ratio = GET_DOUBLE("window", "aspect_ratio");
    project_config->physics_gravity = GET_INT("physics", "gravity");
    project_config->physics_gravity_direction = GET_VEC2("physics", "gravity_direction");

#undef GET_VECT2
#undef GET_VEC4
#undef GET_STRING
#undef GET_INT
#undef GET_BOOL
#undef GET_DOUBLE

    g_print("GOBU: Project config loaded successfully\n");

    return TRUE;
}

/**
 * Libera la memoria asociada con la configuración del proyecto.
 *
 * Esta función se encarga de liberar la memoria asignada para la configuración
 * del proyecto, incluyendo el objeto 'setting', todos los campos de cadena en
 * 'project_config', y opcionalmente 'path_project'.
 *
 * @param full_free Si es TRUE, también libera la memoria de 'path_project'.
 */
void gobu_project_config_free(gboolean full_free)
{
    if (setting)
    {
        g_key_file_free(setting);
        setting = NULL;
    }

    if (project_config)
    {
        // Liberar memoria de strings individuales
        g_free(project_config->name);
        g_free(project_config->version);
        g_free(project_config->description);
        g_free(project_config->id);
        g_free(project_config->author);
        g_free(project_config->email);
        g_free(project_config->icon);
        g_free(project_config->first_scene);
        // g_free(project_config->background_color);
        g_free(project_config->physics_gravity_direction);

        g_free(project_config);
        project_config = NULL;
    }

    if (full_free)
    {
        g_free(path_project);
        path_project = NULL;
    }
}

/**
 * Guarda la configuración actual del proyecto en un archivo.
 *
 * Esta función guarda la configuración almacenada en la variable global 'setting'
 * en el archivo especificado por 'path_project'.
 *
 * @return gboolean TRUE si la configuración se guardó correctamente, FALSE en caso contrario.
 */
gboolean gobu_project_config_save(void)
{
    g_return_val_if_fail(setting != NULL, FALSE);
    g_return_val_if_fail(path_project != NULL, FALSE);

    GError *error = NULL;
    gboolean success = g_key_file_save_to_file(setting, path_project, &error);

    if (!success)
    {
        g_warning("Error saving project config to '%s': %s", path_project, error->message);
        g_clear_error(&error);
    }
    else
    {
        g_info("GOBU: Project config saved successfully to '%s'", path_project);
    }

    return success;
}

/**
 * Obtiene la configuración actual del proyecto.
 *
 * Esta función devuelve un puntero a la estructura de configuración del proyecto actual.
 * El puntero devuelto no debe ser modificado directamente por el llamante.
 *
 * @return const gobuProjectConfig* Puntero a la configuración actual del proyecto, o NULL si no está inicializada.
 */
gobuProjectConfig *gobu_project_config_get(void)
{
    if (project_config == NULL)
    {
        g_warning("Project configuration has not been initialized.");
    }

    return project_config;
}

/**
 * Establece la configuración predeterminada del proyecto.
 *
 * Esta función libera la configuración existente y establece valores predeterminados
 * para todas las opciones de configuración del proyecto.
 *
 * @param name El nombre del proyecto. Si es NULL, se usará un nombre predeterminado.
 * @return gboolean TRUE si la configuración se estableció correctamente, FALSE en caso contrario.
 */
gboolean gobu_project_config_set_default(const gchar *name)
{
    gobu_project_config_free(FALSE);

    setting = g_key_file_new();
    if (!setting)
    {
        g_critical("Failed to create new GKeyFile for project configuration");
        return FALSE;
    }

// Uso de macros para simplificar la escritura de valores
#define SET_STRING(section, key, value) g_key_file_set_string(setting, section, key, value)
#define SET_INT(section, key, value) g_key_file_set_integer(setting, section, key, value)
#define SET_BOOL(section, key, value) g_key_file_set_boolean(setting, section, key, value)
#define SET_DOUBLE(section, key, value) g_key_file_set_double(setting, section, key, value)

    SET_STRING("application.about", "name", name);
    SET_STRING("application.about", "version", "1.0.0.0");
    SET_STRING("application.about", "description", "");
    SET_STRING("application.about", "id", "");
    SET_STRING("application.about", "author", "");
    SET_STRING("application.about", "email", "");
    SET_STRING("application.about", "icon", "");
    SET_STRING("application.startup", "first_scene", "");

    SET_INT("display.viewport", "width", DEFAULT_VIEWPORT_WIDTH);
    SET_INT("display.viewport", "height", DEFAULT_VIEWPORT_HEIGHT);
    SET_INT("display.viewport", "sampling", DEFAULT_SAMPLING);

    g_key_file_set_double_list(setting, "window", "background-color", vec4_type, vec4_type_length);
    SET_INT("window", "width", DEFAULT_VIEWPORT_WIDTH);
    SET_INT("window", "height", DEFAULT_VIEWPORT_HEIGHT);
    SET_INT("window", "mode", DEFAULT_WINDOW_MODE);

    SET_BOOL("window", "resizable", FALSE);
    SET_BOOL("window", "borderless", FALSE);
    SET_BOOL("window", "alway_on_top", FALSE);
    SET_BOOL("window", "transparent", FALSE);
    SET_BOOL("window", "no_focus", FALSE);
    SET_BOOL("window", "highdpi", TRUE);
    SET_BOOL("window", "vsync", TRUE);
    SET_DOUBLE("window", "aspect_ratio", 1.0);

    SET_INT("physics", "gravity", DEFAULT_GRAVITY);
    g_key_file_set_integer_list(setting, "physics", "gravity_direction", vec2_type, vec2_type_length);

#undef SET_STRING
#undef SET_INT
#undef SET_BOOL
#undef SET_DOUBLE

    g_info("GOBU: Default project config set");
    return TRUE;
}
