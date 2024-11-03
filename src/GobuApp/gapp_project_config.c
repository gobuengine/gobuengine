#include "gapp_project_config.h"
// #include "binn/binn_json.h"

// --------------------
// MARK:BASE CLASS
// --------------------

#define PROJECT_CONFIG_KEY "project.config"

enum
{
    PROP_0,
    PROP_NAME,
    PROP_VERSION,
    PROP_DESCRIPTION,
    PROP_ID,
    PROP_AUTHOR,
    PROP_EMAIL,
    PROP_ICON,
    PROP_FIRST_SCENE,
    PROP_VIEWPORT_WIDTH,
    PROP_VIEWPORT_HEIGHT,
    PROP_TEXTURE_FILTER,
    PROP_BACKGROUND_COLOR,
    PROP_WINDOW_WIDTH,
    PROP_WINDOW_HEIGHT,
    PROP_WINDOW_MODE,
    PROP_WINDOW_RESIZABLE,
    PROP_WINDOW_BORDERLESS,
    PROP_WINDOW_ALWAY_ON_TOP,
    PROP_WINDOW_TRANSPARENT,
    PROP_WINDOW_NO_FOCUS,
    PROP_WINDOW_HIGH_DPI,
    PROP_WINDOW_VSYNC,
    PROP_WINDOW_ASPECT_RATIO,
    PROP_PHYSICS_GRAVITY,
    PROP_PHYSICS_GRAVITY_DIRECTIONX,
    PROP_PHYSICS_GRAVITY_DIRECTIONY,
    N_PROPS
};

static GParamSpec *properties[N_PROPS];

struct _GappProjectConfig
{
    GtkBox parent_instance;
    gchar *name;
    gchar *version;
    gchar *description;
    gchar *id;
    gchar *author;
    gchar *email;
    gchar *icon;
    gchar *first_scene;
    gint viewport_width;
    gint viewport_height;
    gint texture_filter;
    GdkRGBA *background_color;
    gint window_width;
    gint window_height;
    gint window_mode;
    gboolean window_resizable;
    gboolean window_borderless;
    gboolean window_alway_on_top;
    gboolean window_transparent;
    gboolean window_no_focus;
    gboolean window_high_dpi;
    gboolean window_vsync;
    gboolean window_aspect_ratio;
    gint physics_gravity;
    gint physics_gravity_directionx;
    gint physics_gravity_directiony;

    // serialize data
    GKeyFile *keyfile;
    gchar *filename;

    // private
    gboolean allowSave;
};

G_DEFINE_TYPE(GappProjectConfig, gapp_project_config, G_TYPE_OBJECT)

static void gapp_project_config_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    GappProjectConfig *self = GAPP_PROJECT_CONFIG(object);

    switch (property_id)
    {
    case PROP_NAME:
        g_free(self->name);
        self->name = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "name", self->name);
        break;

    case PROP_VERSION:
        g_free(self->version);
        self->version = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "version", self->version);
        break;

    case PROP_DESCRIPTION:
        g_free(self->description);
        self->description = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "description", self->description);
        break;

    case PROP_ID:
        g_free(self->id);
        self->id = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "id", self->id);
        break;

    case PROP_AUTHOR:
        g_free(self->author);
        self->author = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "author", self->author);
        break;

    case PROP_EMAIL:
        g_free(self->email);
        self->email = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "email", self->email);
        break;

    case PROP_ICON:
        g_free(self->icon);
        self->icon = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "icon", self->icon);
        break;

    case PROP_FIRST_SCENE:
        g_free(self->first_scene);
        self->first_scene = g_value_dup_string(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "first_scene", self->first_scene);
        break;

    case PROP_VIEWPORT_WIDTH:
        self->viewport_width = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "viewport_width", self->viewport_width);
        break;

    case PROP_VIEWPORT_HEIGHT:
        self->viewport_height = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "viewport_height", self->viewport_height);
        break;

    case PROP_TEXTURE_FILTER:
        self->texture_filter = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "texture_filter", self->texture_filter);
        break;

    case PROP_BACKGROUND_COLOR:
        self->background_color = g_value_dup_boxed(value);
        g_key_file_set_string(self->keyfile, PROJECT_CONFIG_KEY, "background_color", gdk_rgba_to_string(self->background_color));
        break;

    case PROP_WINDOW_WIDTH:
        self->window_width = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "window_width", self->window_width);
        break;

    case PROP_WINDOW_HEIGHT:
        self->window_height = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "window_height", self->window_height);
        break;

    case PROP_WINDOW_MODE:
        self->window_mode = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "window_mode", self->window_mode);
        break;

    case PROP_WINDOW_RESIZABLE:
        self->window_resizable = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_resizable", self->window_resizable);
        break;

    case PROP_WINDOW_BORDERLESS:
        self->window_borderless = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_borderless", self->window_borderless);
        break;

    case PROP_WINDOW_ALWAY_ON_TOP:
        self->window_alway_on_top = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_alway_on_top", self->window_alway_on_top);
        break;

    case PROP_WINDOW_TRANSPARENT:
        self->window_transparent = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_transparent", self->window_transparent);
        break;

    case PROP_WINDOW_NO_FOCUS:
        self->window_no_focus = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_no_focus", self->window_no_focus);
        break;

    case PROP_WINDOW_HIGH_DPI:
        self->window_high_dpi = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_high_dpi", self->window_high_dpi);
        break;

    case PROP_WINDOW_VSYNC:
        self->window_vsync = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_vsync", self->window_vsync);
        break;

    case PROP_WINDOW_ASPECT_RATIO:
        self->window_aspect_ratio = g_value_get_boolean(value);
        g_key_file_set_boolean(self->keyfile, PROJECT_CONFIG_KEY, "window_aspect_ratio", self->window_aspect_ratio);
        break;

    case PROP_PHYSICS_GRAVITY:
        self->physics_gravity = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "physics_gravity", self->physics_gravity);
        break;

    case PROP_PHYSICS_GRAVITY_DIRECTIONX:
        self->physics_gravity_directionx = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "physics_gravity_directionx", self->physics_gravity_directionx);
        break;

    case PROP_PHYSICS_GRAVITY_DIRECTIONY:
        self->physics_gravity_directiony = g_value_get_int(value);
        g_key_file_set_integer(self->keyfile, PROJECT_CONFIG_KEY, "physics_gravity_directiony", self->physics_gravity_directiony);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }

    // TODO: Buscar un lugar mejor para no sobrecargar el disco.
    gapp_project_config_save(self);
}

static void gapp_project_config_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    GappProjectConfig *self = GAPP_PROJECT_CONFIG(object);

    switch (property_id)
    {
    case PROP_NAME:
        g_value_set_string(value, self->name);
        break;

    case PROP_VERSION:
        g_value_set_string(value, self->version);
        break;

    case PROP_DESCRIPTION:
        g_value_set_string(value, self->description);
        break;

    case PROP_ID:
        g_value_set_string(value, self->id);
        break;

    case PROP_AUTHOR:
        g_value_set_string(value, self->author);
        break;

    case PROP_EMAIL:
        g_value_set_string(value, self->email);
        break;

    case PROP_ICON:
        g_value_set_string(value, self->icon);
        break;

    case PROP_FIRST_SCENE:
        g_value_set_string(value, self->first_scene);
        break;

    case PROP_VIEWPORT_WIDTH:
        g_value_set_int(value, self->viewport_width);
        break;

    case PROP_VIEWPORT_HEIGHT:
        g_value_set_int(value, self->viewport_height);
        break;

    case PROP_TEXTURE_FILTER:
        g_value_set_int(value, self->texture_filter);
        break;

    case PROP_BACKGROUND_COLOR:
        g_value_set_boxed(value, self->background_color);
        break;

    case PROP_WINDOW_WIDTH:
        g_value_set_int(value, self->window_width);
        break;

    case PROP_WINDOW_HEIGHT:
        g_value_set_int(value, self->window_height);
        break;

    case PROP_WINDOW_MODE:
        g_value_set_int(value, self->window_mode);
        break;

    case PROP_WINDOW_RESIZABLE:
        g_value_set_boolean(value, self->window_resizable);
        break;

    case PROP_WINDOW_BORDERLESS:
        g_value_set_boolean(value, self->window_borderless);
        break;

    case PROP_WINDOW_ALWAY_ON_TOP:
        g_value_set_boolean(value, self->window_alway_on_top);
        break;

    case PROP_WINDOW_TRANSPARENT:
        g_value_set_boolean(value, self->window_transparent);
        break;

    case PROP_WINDOW_NO_FOCUS:
        g_value_set_boolean(value, self->window_no_focus);
        break;

    case PROP_WINDOW_HIGH_DPI:
        g_value_set_boolean(value, self->window_high_dpi);
        break;

    case PROP_WINDOW_VSYNC:
        g_value_set_boolean(value, self->window_vsync);
        break;

    case PROP_WINDOW_ASPECT_RATIO:
        g_value_set_boolean(value, self->window_aspect_ratio);
        break;

    case PROP_PHYSICS_GRAVITY:
        g_value_set_int(value, self->physics_gravity);
        break;

    case PROP_PHYSICS_GRAVITY_DIRECTIONX:
        g_value_set_int(value, self->physics_gravity_directionx);
        break;

    case PROP_PHYSICS_GRAVITY_DIRECTIONY:
        g_value_set_int(value, self->physics_gravity_directiony);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void gapp_project_config_dispose(GObject *object)
{
    GappProjectConfig *self = GAPP_PROJECT_CONFIG(object);

    // Liberar recursos adicionales aquí, si es necesario
    g_free(self->name);
    g_free(self->version);
    g_free(self->description);
    g_free(self->id);
    g_free(self->author);
    g_free(self->email);
    g_free(self->icon);
    g_free(self->first_scene);
    gdk_rgba_free(self->background_color);

    g_free(self->filename);
    g_key_file_free(self->keyfile);

    G_OBJECT_CLASS(gapp_project_config_parent_class)->dispose(object);
}

static void gapp_project_config_class_init(GappProjectConfigClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = gapp_project_config_dispose;
    object_class->set_property = gapp_project_config_set_property;
    object_class->get_property = gapp_project_config_get_property;

    // Registrar las propiedades del objeto
    properties[PROP_NAME] = g_param_spec_string("name", "Name", "", NULL, G_PARAM_READWRITE);
    properties[PROP_VERSION] = g_param_spec_string("version", "VERSION", "The version project", NULL, G_PARAM_READWRITE);
    properties[PROP_DESCRIPTION] = g_param_spec_string("description", "Description", "", NULL, G_PARAM_READWRITE);
    properties[PROP_ID] = g_param_spec_string("id", "Id", "", NULL, G_PARAM_READWRITE);
    properties[PROP_AUTHOR] = g_param_spec_string("author", "Author", "", NULL, G_PARAM_READWRITE);
    properties[PROP_EMAIL] = g_param_spec_string("email", "Email", "", NULL, G_PARAM_READWRITE);
    properties[PROP_ICON] = g_param_spec_string("icon", "Icon", "", NULL, G_PARAM_READWRITE);
    properties[PROP_FIRST_SCENE] = g_param_spec_string("first_scene", "firstScene", "", NULL, G_PARAM_READWRITE);
    properties[PROP_VIEWPORT_WIDTH] = g_param_spec_int("viewport_width", "Viewport Width", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_VIEWPORT_HEIGHT] = g_param_spec_int("viewport_height", "Viewport Height", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_TEXTURE_FILTER] = g_param_spec_int("texture_filter", "Texture-Filter", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_BACKGROUND_COLOR] = g_param_spec_boxed("background_color", "Background Color", "", GDK_TYPE_RGBA, G_PARAM_READWRITE);
    properties[PROP_WINDOW_WIDTH] = g_param_spec_int("window_width", "Window Width", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_WINDOW_HEIGHT] = g_param_spec_int("window_height", "Window Height", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_WINDOW_MODE] = g_param_spec_int("window_mode", "Window Mode", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_WINDOW_RESIZABLE] = g_param_spec_boolean("window_resizable", "Window Resizable", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_BORDERLESS] = g_param_spec_boolean("window_borderless", "Window Borderless", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_ALWAY_ON_TOP] = g_param_spec_boolean("window_alway_on_top", "Window Alway On Top", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_TRANSPARENT] = g_param_spec_boolean("window_transparent", "Window Transparent", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_NO_FOCUS] = g_param_spec_boolean("window_no_focus", "Window No Focus", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_HIGH_DPI] = g_param_spec_boolean("window_high_dpi", "Window High DPI", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_VSYNC] = g_param_spec_boolean("window_vsync", "Window VSync", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_WINDOW_ASPECT_RATIO] = g_param_spec_boolean("window_aspect_ratio", "Window Aspect Ratio", "", FALSE, G_PARAM_READWRITE);
    properties[PROP_PHYSICS_GRAVITY] = g_param_spec_int("physics_gravity", "Physics Gravity", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_PHYSICS_GRAVITY_DIRECTIONX] = g_param_spec_int("physics_gravity_directionx", "Physics Gravity DirectionX", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);
    properties[PROP_PHYSICS_GRAVITY_DIRECTIONY] = g_param_spec_int("physics_gravity_directiony", "Physics Gravity DirectionY", "", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void gapp_project_config_init(GappProjectConfig *self)
{
    self->filename = NULL;
}

static void gapp_project_config_set_property_default_init(GappProjectConfig *self, const gchar *name)
{
    // Creamos datos predeterminados para el proyecto.
    // Si estos datos ya existen en el archivo gobuproject,
    // se actualizarán; si no, se generarán nuevos datos predeterminados.
    g_object_set(self,
                 "name", g_strdup(name),
                 "version", "0.0.0.1",
                 "first_scene", "main",
                 "viewport_width", 1280,
                 "viewport_height", 720,
                 "window_width", 1280,
                 "window_height", 720,
                 "window_vsync", TRUE,
                 "window_high_dpi", TRUE,
                 "window_resizable", FALSE,
                 "window_aspect_ratio", TRUE,
                 "physics_gravity", 980,
                 "physics_gravity_directiony", -1, NULL);
}

// --------------------
// MARK:API
// --------------------

/**
 * Crea una nueva instancia de GappProjectConfig y la inicializa con los datos de un archivo.
 *
 * @param filename El nombre del archivo de configuración a cargar.
 * @return Una nueva instancia de GappProjectConfig, o NULL si ocurre un error.
 */
GappProjectConfig *gapp_project_config_new(void)
{
    GappProjectConfig *config = g_object_new(GAPP_TYPE_PROJECT_CONFIG, NULL);
    config->keyfile = g_key_file_new();
    return config;
}

gboolean gapp_project_config_create_file_default(GappProjectConfig *self, const gchar *filename, const gchar *name)
{
    // para que no se active el save que esta interno.
    self->allowSave = FALSE;
    {
        // Creamos datos predeterminados para el proyecto.
        // Si estos datos ya existen en el archivo gobuproject,
        // se actualizarán; si no, se generarán nuevos datos predeterminados.
        gapp_project_config_set_property_default_init(self, name);
    }
    self->allowSave = TRUE;

    // registramos manual el archivo.
    self->filename = g_strdup(filename);

    // guardamos.
    return gapp_project_config_save(self);
}

/**
 * Carga la configuración del proyecto desde un archivo.
 *
 * Esta función lee un archivo de configuración en formato binn y establece
 * las propiedades correspondientes en el objeto GappProjectConfig utilizando g_object_set_data().
 *
 * @param self El objeto GappProjectConfig que se va a configurar.
 * @param filename El nombre del archivo de configuración a cargar.
 */
gboolean gapp_project_config_load(GappProjectConfig *self, const gchar *filename)
{
    g_return_val_if_fail(GAPP_IS_PROJECT_CONFIG(self) && filename != NULL, FALSE);

    gboolean result = FALSE;
    self->allowSave = FALSE;

    // Creamos datos predeterminados para el proyecto.
    // Si estos datos ya existen en el archivo gobuproject,
    // se actualizarán; si no, se generarán nuevos datos predeterminados.
    gapp_project_config_set_property_default_init(self, "");

    //
    //
    if (g_key_file_load_from_file(self->keyfile, filename, G_KEY_FILE_NONE, NULL))
    {
        g_free(self->filename);
        self->filename = g_strdup(filename);

        gsize n_keys = 0;
        gchar **keys = g_key_file_get_keys(self->keyfile, PROJECT_CONFIG_KEY, &n_keys, NULL);

        for (gsize i = 0; i < n_keys; i++)
        {
            gchar *prop_name = keys[i];

            GParamSpec *pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(self), prop_name);
            if (pspec != NULL)
            {
                gpointer value = NULL;
                GType tipo = pspec->value_type;

                if (g_type_is_a(tipo, G_TYPE_STRING))
                {
                    value = g_key_file_get_string(self->keyfile, PROJECT_CONFIG_KEY, prop_name, NULL);
                }
                else if (g_type_is_a(tipo, G_TYPE_INT))
                {
                    value = GINT_TO_POINTER(g_key_file_get_integer(self->keyfile, PROJECT_CONFIG_KEY, prop_name, NULL));
                }
                else if (g_type_is_a(tipo, G_TYPE_DOUBLE))
                {
                    value = GINT_TO_POINTER(g_key_file_get_double(self->keyfile, PROJECT_CONFIG_KEY, prop_name, NULL));
                }
                else if (g_type_is_a(tipo, G_TYPE_BOOLEAN))
                {
                    value = GINT_TO_POINTER(g_key_file_get_boolean(self->keyfile, PROJECT_CONFIG_KEY, prop_name, NULL));
                }
                else if (g_type_is_a(tipo, GDK_TYPE_RGBA))
                {
                    GdkRGBA parse_color;
                    gdk_rgba_parse(&parse_color, g_key_file_get_string(self->keyfile, PROJECT_CONFIG_KEY, prop_name, NULL));
                    value = gdk_rgba_copy(&parse_color);
                }

                if (value != NULL)
                {
                    g_object_set(G_OBJECT(self), prop_name, value, NULL);
                }
            }

            g_free(prop_name);
        }

        result = TRUE;
    }

    self->allowSave = TRUE;

    return result;
}

/**
 * Guarda la configuración del proyecto en un archivo.
 *
 * Esta función escribe la configuración actual del proyecto en un archivo
 * utilizando el formato binn.
 *
 * @param self El objeto GappProjectConfig que contiene la configuración a guardar.
 * @param filename El nombre del archivo donde se guardará la configuración.
 * @return TRUE si el guardado fue exitoso, FALSE en caso contrario.
 */
gboolean gapp_project_config_save(GappProjectConfig *self)
{
    g_return_val_if_fail(GAPP_IS_PROJECT_CONFIG(self), FALSE);
    if (self->filename == NULL)
        return FALSE; // No se puede guardar si no se ha cargado un archivo antes

    if (!self->allowSave)
        return FALSE;

    if (!g_key_file_save_to_file(self->keyfile, self->filename, NULL))
    {
        g_warning("GappProjectConfig: Failed to save configuration to file: %s", self->filename);
        return FALSE;
    }

    return TRUE;
}

const gchar *gapp_project_config_get_name(GappProjectConfig *self)
{
    g_return_val_if_fail(GAPP_IS_PROJECT_CONFIG(self), NULL);
    return self->name;
}
