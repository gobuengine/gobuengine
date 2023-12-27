#include "gobu.h"
#include "thirdparty/goburender/rlgl.h"
#include "config.h"


// VERSION 2.0.0
// #ifdef GOBU_LIB_APP_IMPL
// #define SOKOL_NO_ENTRY
// #include "thirdparty/sokol/sokol_app.h"
// #endif
// // #ifdef GOBU_LIB_IMPL
// #include "thirdparty/sokol/sokol_gfx.h"
// // #include "thirdparty/sokol/sokol_log.h"
// #include "thirdparty/sokol/sokol_glue.h"
// #define SOKOL_GL_IMPL
// #include "thirdparty/sokol/sokol_gl.h"
// // #endif

// --------------------------------------------------------------
// Cree el archivo gobu-engine para implementar la idea
// de tener todo o casi todo en un solo lugar se que esto
// seria mas dificil de mantener y es una idea que loca.
// En una version futura se podria dividir en varios archivos.
// --------------------------------------------------------------

ECS_COMPONENT_DECLARE(enumCameraMode);
ECS_COMPONENT_DECLARE(gb_render_phases_t);
ECS_COMPONENT_DECLARE(gb_app_t);
ECS_COMPONENT_DECLARE(gb_color_t);
ECS_COMPONENT_DECLARE(gb_rect_t);
ECS_COMPONENT_DECLARE(gb_vec2_t);
ECS_COMPONENT_DECLARE(gb_bounding_t);
ECS_COMPONENT_DECLARE(gb_transform_t);
ECS_COMPONENT_DECLARE(gb_info_t);
ECS_COMPONENT_DECLARE(gb_animated_t);
ECS_COMPONENT_DECLARE(gb_text_t);
ECS_COMPONENT_DECLARE(gb_sprite_t);
ECS_COMPONENT_DECLARE(gb_animate_frame_t);
ECS_COMPONENT_DECLARE(gb_animate_animation_t);
ECS_COMPONENT_DECLARE(gb_animate_sprite_t);
ECS_COMPONENT_DECLARE(gb_shape_rect_t);
ECS_COMPONENT_DECLARE(gb_shape_circle_t);
ECS_COMPONENT_DECLARE(gb_resource_t);
ECS_COMPONENT_DECLARE(gb_camera_t);
ECS_COMPONENT_DECLARE(gb_gizmos_t);

gb_render_phases_t render_phases;
gb_engine_t engine;

struct gb_setting_project_t {
    char* path;
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
    GKeyFile* setting;
}project;


static bool gb_setting_load(const char* filename);
static void gb_input_init(void);

// ########################################
// Project functions
// ########################################
// Description de project functions:
//

bool gb_project_load(const char* filename)
{
    bool test = gb_fs_file_exist(filename);
    if (test)
    {
        project.path = gb_path_dirname(filename);

        char* file_gobuproject = gb_path_join(project.path, "game.gobuproject", NULL);

        gb_setting_load(file_gobuproject);

        g_free(file_gobuproject);
    }

    return test;
}

const char* gb_project_get_path(void)
{
    return gb_strdup(project.path);
}


// ########################################
// Project Setting functions
// ########################################
// Description de project-setting functions: Son funciones que se utilizarán en el código
// de Gobu para cargar y guardar la configuración del proyecto, como por ejemplo, el nombre
// del proyecto, la descripción, el autor, etc.
//
static bool gb_setting_load(const char* filename)
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
    }

    return r;
}

GKeyFile* gb_setting_project_setting(void)
{
    return project.setting;
}

void gb_setting_unload(void)
{
    g_free(project.path);
    g_free(project.name);
    g_free(project.description);
    g_free(project.author);
    g_free(project.license);
    g_free(project.version);
    g_free(project.thumbnail);
    g_key_file_free(project.setting);
}

void gb_setting_save(void)
{
    char* file_gobuproject = gb_path_join(project.path, "game.gobuproject", NULL);
    g_key_file_save_to_file(project.setting, file_gobuproject, NULL);
    g_free(file_gobuproject);
}

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


// ########################################
// Utility functions
// ########################################
// Description de utility functions: Son funciones que se utilizarán en el código
// de Gobu para realizar tareas específicas, como por ejemplo, obtener el nombre
// de un archivo, verificar si existe, etc.
//

/**
 * @brief Retorna el contenido de una ruta relativa.
 *
 * Esta función toma una ruta relativa como entrada y devuelve el contenido de ese archivo.
 *
 * @param path La ruta relativa del archivo.
 * @return El contenido del archivo como una cadena de caracteres.
 */
char* gb_path_relative_content(const char* path)
{
    char* npath = strstr(path, FOLDER_CONTENT_PROJECT);
    if (npath != NULL) {
        npath += strlen(FOLDER_CONTENT_PROJECT);
    }
    else {
        npath = gb_strdup(path);
    }

    return npath;
}

char* gb_path_join_relative_content(const char* path)
{
    return gb_path_normalize(gb_path_join(gb_project_get_path(), "Content", path, NULL));
}

/**
 * @brief Normaliza una ruta de archivo.
 *
 * Esta función toma una ruta de archivo y la normaliza, eliminando cualquier
 * caracter especial o redundante. Devuelve un puntero a la ruta de archivo
 * normalizada.
 *
 * @param path La ruta de archivo a normalizar.
 * @return Un puntero a la ruta de archivo normalizada.
 */
char* gb_path_normalize(const char* path)
{
    return gb_str_replace(path, "\\", "/");
}

/**
 * @brief Combina múltiples rutas en una sola ruta.
 *
 * Esta función toma una ruta inicial y una lista variable de rutas adicionales y las combina en una sola ruta.
 *
 * @param first_path La ruta inicial.
 * @param ... Las rutas adicionales a combinar.
 *
 * @return La ruta combinada.
 */
char* gb_path_join(const char* first_path, ...)
{
    gchar* buffer;
    va_list args;

    va_start(args, first_path);
    buffer = g_build_filename_valist(first_path, &args);
    va_end(args);

    return buffer;
}

/**
 * @brief Obtiene el nombre base de una ruta de archivo.
 *
 * Esta función toma una ruta de archivo como entrada y devuelve el nombre base de la ruta.
 * El nombre base es el último componente de la ruta, sin la extensión del archivo.
 *
 * @param filename La ruta de archivo de la cual se desea obtener el nombre base.
 * @return El nombre base de la ruta de archivo.
 */
char* gb_path_basename(const char* filename)
{
    return g_path_get_basename(filename);
}

/**
 * @brief Retorna el directorio de la ruta de un archivo.
 *
 * Esta función toma una cadena de caracteres que representa la ruta de un archivo y devuelve
 * el directorio de esa ruta. El directorio se determina eliminando el nombre del archivo de la ruta.
 *
 * @param filename La ruta del archivo.
 * @return El directorio de la ruta del archivo.
 */
char* gb_path_dirname(const char* filename)
{
    return g_path_get_dirname(filename);
}

/**
 * Retorna la ruta del directorio actual.
 *
 * @return Un puntero a una cadena que representa la ruta del directorio actual.
 */
char* gb_path_current_dir(void)
{
    return g_get_current_dir();
}

/**
 * @brief Verifica si existe un folder en la ruta especificada.
 *
 * @param filename El nombre del folder a verificar.
 * @return true si el folder existe, false en caso contrario.
 */
bool gb_path_exist(const char* filename)
{
    return gb_fs_test(filename, G_FS_TEST_IS_DIR);
}

/**
 * @brief Copia un archivo desde la ruta de origen a la ruta de destino.
 *
 * Esta función copia el contenido del archivo de origen al archivo de destino.
 *
 * @param src El archivo de origen que se copiará.
 * @param dest El archivo de destino donde se copiará el contenido.
 */
bool gb_fs_copyc(GFile* src, GFile* dest, GError** error)
{
    if (gb_path_exist(g_file_get_path(src))) {
        if (gb_fs_mkdir(g_file_get_path(dest))) {
            GFileEnumerator* enumerator = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);
            GFileInfo* file_info = NULL;
            while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL)
            {
                const gchar* file_name = g_file_info_get_name(file_info);
                GFile* file_src = g_file_new_for_path(gb_path_join(g_file_get_path(src), file_name, NULL));
                GFile* file_dest = g_file_new_for_path(gb_path_join(g_file_get_path(dest), file_name, NULL));
                gb_fs_copyc(file_src, file_dest, error);
            }
        }
        return TRUE;
    }

    return g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, error);
}

/**
 * @brief Escribe el contenido en un archivo.
 *
 * Esta función escribe el contenido proporcionado en un archivo con el nombre especificado.
 *
 * @param filename El nombre del archivo en el que se va a escribir.
 * @param contents El contenido que se va a escribir en el archivo.
 * @return true si se pudo escribir el archivo correctamente, false en caso contrario.
 */
bool gb_fs_write_file(const char* filename, const char* contents)
{
    return g_file_set_contents(filename, contents, strlen(contents), NULL);
}

/**
 * @brief Crea un directorio con la ruta especificada.
 *
 * @param pathname La ruta del directorio a crear.
 * @return true si el directorio se creó correctamente, false en caso contrario.
 */
bool gb_fs_mkdir(const char* pathname)
{
    return g_mkdir_with_parents(pathname, 0) == -1 ? false : true;
}

/**
 * @brief Verifica si un archivo existe en el sistema de archivos.
 *
 * Esta función comprueba si un archivo con el nombre especificado existe en el sistema de archivos.
 *
 * @param filename El nombre del archivo a verificar.
 * @return true si el archivo existe, false en caso contrario.
 */
bool gb_fs_file_exist(const char* filename)
{
    return gb_fs_test(filename, G_FS_TEST_EXISTS);
}

/**
 * @brief Prueba el sistema de archivos para el nombre de archivo dado utilizando la prueba especificada.
 *
 * @param filename El nombre del archivo a probar.
 * @param test El tipo de prueba a realizar en el sistema de archivos.
 * @return true si la prueba es exitosa, false en caso contrario.
 */
bool gb_fs_test(const char* filename, GFSTest test)
{
    return g_file_test(filename, (GFileTest)test);
}

/**
 * @brief Obtiene la extensión de un nombre de archivo.
 *
 * Esta función toma un nombre de archivo como entrada y devuelve la extensión del archivo.
 *
 * @param filename El nombre del archivo del cual se desea obtener la extensión.
 * @return La extensión del archivo.
 */
const char* gb_fs_extname(const char* filename)
{
    if (filename[0] == '.') return filename;
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    return dot;
}

/**
 * @brief Obtiene el nombre de un archivo a partir de su ruta.
 *
 * Esta función toma una ruta de archivo y devuelve el nombre del archivo sin la extensión.
 *
 * @param filename La ruta del archivo.
 * @param with_ext Indica si se debe incluir la extensión en el nombre del archivo.
 * @return El nombre del archivo sin la extensión, si with_ext es false.
 *         Si with_ext es true, devuelve el nombre del archivo con la extensión.
 */
char* gb_fs_get_name(const char* filename, bool with_ext)
{
    gchar* basename = gb_path_basename(filename);
    if (with_ext) {
        gchar** sep_name = gb_str_split(basename, ".");
        gchar* name = gb_strdup(sep_name[0]);
        gb_str_split_free(sep_name);
        return name;
    }
    return basename;
}

/**
 * @brief Verifica si la extensión de un archivo coincide con una extensión dada.
 *
 * @param filename El nombre del archivo.
 * @param ext La extensión a comparar.
 * @return true si la extensión del archivo coincide con la extensión dada, false en caso contrario.
 */

bool gb_fs_is_extension(const char* filename, const char* ext)
{
    return (g_strcmp0(gb_fs_extname(filename), ext) == 0);
}

/**
 * @brief Obtiene el contenido de un archivo.
 *
 * Esta función lee el contenido de un archivo y lo devuelve como una cadena de caracteres.
 *
 * @param filename El nombre del archivo a leer.
 * @param length La longitud máxima del contenido a leer.
 * @return Un puntero a la cadena de caracteres que contiene el contenido del archivo.
 */
gchar* gb_fs_get_contents(const char* filename, gsize* length)
{
    GError* error = NULL;
    gchar* contents = NULL;
    *length = 0;

    if (g_file_get_contents(filename, &contents, length, &error) == FALSE)
    {
        g_error("Error reading file: %s", error->message);
        g_error_free(error);
    }
    return contents;
}

/**
 * @brief Duplica una cadena de texto con formato opcional.
 *
 * Esta función duplica una cadena de texto con formato opcional. Toma una cadena de formato y argumentos adicionales,
 * similares a la función printf, y devuelve una cadena recién asignada que es una duplicación de la cadena formateada.
 *
 * @param format La cadena de formato.
 * @param ... Argumentos adicionales para el formato.
 * @return Una cadena recién asignada que es una duplicación de la cadena formateada.
 */
char* gb_strdups(const char* format, ...)
{
    gchar* buffer;
    va_list args;

    va_start(args, format);
    buffer = g_strdup_vprintf(format, args);
    va_end(args);

    return buffer;
}

/**
 * @brief Duplica una cadena de texto.
 *
 * Esta función crea una copia de la cadena de texto de entrada y devuelve un puntero a la memoria recién asignada.
 *
 * @param str La cadena de texto a duplicar.
 * @return Un puntero a la cadena de texto duplicada, o NULL si falla la asignación de memoria.
 */
char* gb_strdup(const char* str)
{
    return g_strdup(str);
}

/**
 * Convierte una cadena de texto a minúsculas.
 *
 * @param text La cadena de texto a convertir.
 * @return La cadena de texto convertida en minúsculas.
 */
char* gb_str_tolower(const char* text)
{
    return g_utf8_strdown(text, strlen(text));
}

/**
 * Convierte una cadena a mayúsculas.
 *
 * @param text La cadena que se va a convertir.
 * @return La cadena convertida en mayúsculas.
 */
char* gb_str_uppercase(const char* text)
{
    return g_utf8_strup(text, strlen(text));
}

/**
 * @brief Compara dos cadenas de texto y devuelve verdadero si son iguales.
 *
 * @param text1 La primera cadena de texto a comparar.
 * @param text2 La segunda cadena de texto a comparar.
 * @return true si las cadenas de texto son iguales, false en caso contrario.
 */
bool gb_str_isequal(const char* text1, const char* text2)
{
    bool result = false;

    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0) result = true;
    }

    return result;
}

/**
 * Divide una cadena en un array de subcadenas basado en un delimitador.
 *
 * @param string La cadena a dividir.
 * @param delimiter El delimitador utilizado para dividir la cadena.
 * @return Un array de subcadenas.
 */
char** gb_str_split(const char* string, const char* delimiter)
{
    return g_strsplit(string, delimiter, -1);
}

/**
 * @brief Libera la memoria asignada a un array de cadenas de caracteres.
 *
 * @param str_array El array de cadenas de caracteres a liberar.
 */
void gb_str_split_free(char** str_array)
{
    g_strfreev(str_array);
}

/**
 * @brief Elimina los espacios en blanco al principio y al final de una cadena de caracteres.
 *
 * @param str La cadena de caracteres a recortar.
 * @return Un puntero a la cadena de caracteres recortada.
 */
char* gb_str_trim(char* str)
{
    return g_strstrip(str);
}

/**
 * @brief Elimina los espacios en blanco de una cadena de caracteres.
 *
 * @param str La cadena de caracteres de la cual se eliminarán los espacios en blanco.
 * @return Un puntero a la cadena de caracteres resultante sin espacios en blanco.
 */
char* gb_str_remove_spaces(char* str)
{
    return gb_str_replace(g_strdup(str), " ", "");
}

/**
 * @brief Sanitiza una cadena eliminando cualquier carácter especial o símbolo.
 *
 * @param str La cadena que se va a sanitizar.
 * @return Un puntero a la cadena sanitizada.
 */
char* gb_str_sanitize(char* str)
{
    return gb_str_replace(g_strdup(str), G_STR_DELIMITERS, "");
}

/**
 * Reemplaza todas las apariciones de una subcadena en una cadena con otra subcadena.
 *
 * @param str La cadena original.
 * @param find La subcadena que se va a reemplazar.
 * @param replace La subcadena para reemplazar las apariciones de 'find'.
 * @return Una nueva cadena con todas las apariciones de 'find' reemplazadas por 'replace'.
 */
char* gb_str_replace(const char* str, const char* find, const char* replace)
{
    char** split = g_strsplit(str, find, -1);
    char* result = g_strjoinv(replace, split);
    g_strfreev(split);
    return result;
}



// ########################################
// gblib functions
// ########################################
//

void gb_gfx_begin_lines(void) { rlBegin(RL_LINES); }
void gb_gfx_end() { rlEnd(); }
void gb_gfx_color4ub(gb_color_t color) { rlColor4ub(color.r, color.g, color.b, color.a); }
void gb_gfx_vertex2f(float x, float y) { rlVertex2f(x, y); }
void gb_gfx_vertex3f(float x, float y, float z) { rlVertex3f(x, y, z); }
void gb_gfx_translate(float x, float y, float z) { rlTranslatef(x, y, z); }
void gb_gfx_rotate(float angle, float x, float y, float z) { rlRotatef(angle, x, y, z); }
void gb_gfx_scale(float x, float y, float z) { rlScalef(x, y, z); }
void gb_gfx_push_matrix(void) { rlPushMatrix(); }
void gb_gfx_pop_matrix(void) { rlPopMatrix(); }
void gb_gfx_init() {}
void gb_gfx_close() {}


// ########################################
// Rendering functions
// ########################################
//


/**
 * @brief Dibuja una cuadrícula 2D en la pantalla.
 *
 * Esta función dibuja una cuadrícula 2D en la pantalla utilizando el número de divisiones y el espaciado especificados.
 * Los colores de la cuadrícula se pueden personalizar utilizando los parámetros de color1 y color2.
 *
 * @param slices El número de divisiones de la cuadrícula.
 * @param spacing El espaciado entre las líneas de la cuadrícula.
 * @param color1 El color de las líneas de la cuadrícula.
 * @param color2 El color alternativo de las líneas de la cuadrícula.
 */
void gb_rendering_draw_grid_2d(int slices, float spacing, gb_color_t color)
{
    int halfSlices = slices / 2;

    gb_gfx_begin_lines();
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        gb_gfx_color4ub(color);
        gb_gfx_color4ub(color);
        gb_gfx_color4ub(color);
        gb_gfx_color4ub(color);

        gb_gfx_vertex2f((float)i * spacing, (float)-halfSlices * spacing);
        gb_gfx_vertex2f((float)i * spacing, (float)halfSlices * spacing);

        gb_gfx_vertex2f((float)-halfSlices * spacing, (float)i * spacing);
        gb_gfx_vertex2f((float)halfSlices * spacing, (float)i * spacing);
    }
    gb_gfx_end();
}

void gb_rendering_draw_gismos(gb_transform_t transform, gb_bounding_t bonding_box)
{
    Rectangle bonding = (Rectangle){ 0.0f, 0.0f, bonding_box.max.x, bonding_box.max.y };

    gb_gfx_push_matrix();
    {
        DrawRectangleLinesEx(bonding, 2, SKYBLUE);
    }
    gb_gfx_pop_matrix();
}

/**
 * Función que dibuja un rectángulo en la pantalla.
 *
 * @param rect El rectángulo a dibujar.
 */
void gb_rendering_draw_rect(gb_shape_rect_t rect)
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, rect.color);
    if (rect.line_width > 0)
        DrawRectangleLinesEx((gb_rect_t) { rect.x, rect.y, rect.width, rect.height }, rect.line_width, rect.color_line);
}

/**
 * Función que dibuja un círculo en la pantalla.
 *
 * @param circle El círculo a dibujar.
 */
void gb_rendering_draw_circle(gb_shape_circle_t circle)
{
    DrawCircle(circle.x, circle.y, circle.radius, circle.color);
}

/**
 * Función que dibuja un sprite en la pantalla.
 *
 * @param sprite El sprite a dibujar.
 */
void gb_rendering_draw_sprite(gb_sprite_t sprite)
{
    Rectangle src = (Rectangle){ sprite.src.x, sprite.src.y, sprite.src.width, sprite.src.height };
    Rectangle dst = (Rectangle){ sprite.dst.x, sprite.dst.y, sprite.dst.width, sprite.dst.height };
    DrawTexturePro(sprite.texture, src, dst, Vector2Zero(), 0.0f, sprite.tint);
}


/**
 * Función para dibujar texto en la pantalla.
 *
 * @param text El texto a dibujar.
 */
void gb_rendering_draw_text(gb_text_t text)
{
    DrawTextEx(text.font, text.text, Vector2Zero(), text.size, text.spacing, text.color);
}


// ########################################
// Components functions
// ########################################
// Description de components functions: Son funciones que se utilizarán en el código
// de Gobu para crear componentes, como por ejemplo, crear un componente de tipo
// sprite, un componente de tipo texto, etc.
//

static void gb_engine_component_init(gb_world_t* world)
{
    ECS_COMPONENT_DEFINE(world, enumCameraMode);
    ECS_COMPONENT_DEFINE(world, gb_render_phases_t);
    ECS_COMPONENT_DEFINE(world, gb_app_t);
    ECS_COMPONENT_DEFINE(world, gb_color_t);
    ECS_COMPONENT_DEFINE(world, gb_rect_t);
    ECS_COMPONENT_DEFINE(world, gb_vec2_t);
    ECS_COMPONENT_DEFINE(world, gb_bounding_t);
    ECS_COMPONENT_DEFINE(world, gb_transform_t);
    ECS_COMPONENT_DEFINE(world, gb_info_t);
    ECS_COMPONENT_DEFINE(world, gb_animated_t);
    ECS_COMPONENT_DEFINE(world, gb_text_t);
    ECS_COMPONENT_DEFINE(world, gb_sprite_t);
    ECS_COMPONENT_DEFINE(world, gb_animate_frame_t);
    ECS_COMPONENT_DEFINE(world, gb_animate_animation_t);
    ECS_COMPONENT_DEFINE(world, gb_animate_sprite_t);
    ECS_COMPONENT_DEFINE(world, gb_shape_rect_t);
    ECS_COMPONENT_DEFINE(world, gb_shape_circle_t);
    ECS_COMPONENT_DEFINE(world, gb_resource_t);
    ECS_COMPONENT_DEFINE(world, gb_camera_t);
    ECS_COMPONENT_DEFINE(world, gb_gizmos_t);

    ecs_enum(world, {
        .entity = ecs_id(enumCameraMode),
        .constants = {
            {.name = "NONE", .value = GB_CAMERA_NONE },
            {.name = "EDITOR", .value = GB_CAMERA_EDITOR },
            {.name = "FOLLOWING", .value = GB_CAMERA_FOLLOWING }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_vec2_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_camera_t),
        .members = {
            {.name = "offset", .type = ecs_id(gb_vec2_t) },
            {.name = "target", .type = ecs_id(gb_vec2_t) },
            {.name = "rotation", .type = ecs_id(ecs_f32_t) },
            {.name = "zoom", .type = ecs_id(ecs_f32_t) },
            {.name = "mode", .type = ecs_id(enumCameraMode) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_color_t),
        .members = {
            {.name = "r", .type = ecs_id(ecs_byte_t) },
            {.name = "g", .type = ecs_id(ecs_byte_t) },
            {.name = "b", .type = ecs_id(ecs_byte_t) },
            {.name = "a", .type = ecs_id(ecs_byte_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "width", .type = ecs_id(ecs_f32_t) },
            {.name = "height", .type = ecs_id(ecs_f32_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_transform_t),
        .members = {
            {.name = "position", .type = ecs_id(gb_vec2_t) },
            {.name = "scale", .type = ecs_id(gb_vec2_t) },
            {.name = "rotation", .type = ecs_id(ecs_f32_t) },
            {.name = "origin", .type = ecs_id(gb_vec2_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_info_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t) },
            {.name = "type", .type = ecs_id(ecs_u8_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animated_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "animation", .type = ecs_id(ecs_string_t) },
            {.name = "is_playing", .type = ecs_id(ecs_bool_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_text_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "text", .type = ecs_id(ecs_string_t) },
            {.name = "size", .type = ecs_id(ecs_f32_t) },
            {.name = "spacing", .type = ecs_id(ecs_f32_t) },
            {.name = "color", .type = ecs_id(gb_color_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_sprite_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "tint", .type = ecs_id(gb_color_t) },
            {.name = "src", .type = ecs_id(gb_rect_t) },
            {.name = "dst", .type = ecs_id(gb_rect_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_rect_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "width", .type = ecs_id(ecs_f32_t) },
            {.name = "height", .type = ecs_id(ecs_f32_t) },
            {.name = "color", .type = ecs_id(gb_color_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_shape_circle_t),
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t) },
            {.name = "y", .type = ecs_id(ecs_f32_t) },
            {.name = "radius", .type = ecs_id(ecs_f32_t) },
            {.name = "color", .type = ecs_id(gb_color_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_gizmos_t),
        .members = {
            {.name = "selected", .type = ecs_id(ecs_bool_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_resource_t),
        .members = {
            {.name = "path", .type = ecs_id(ecs_string_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animate_frame_t),
        .members = {
            {.name = "duration", .type = ecs_id(ecs_i32_t) },
            {.name = "sprite", .type = ecs_id(gb_sprite_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animate_animation_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t) },
            {.name = "fps", .type = ecs_id(ecs_i16_t) },
            {.name = "loop", .type = ecs_id(ecs_bool_t) },
            {.name = "frames", .type = ecs_vector(world, {.entity = ecs_entity(world, {.name = "frames" }),.type = ecs_id(gb_animate_frame_t)})},
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_animate_sprite_t),
        .members = {
            {.name = "resource", .type = ecs_id(ecs_string_t) },
            {.name = "animation", .type = ecs_id(ecs_string_t) },
            {.name = "animations", .type = ecs_vector(world, {.entity = ecs_entity(world, {.name = "animations" }),.type = ecs_id(gb_animate_animation_t)})},
        }
    });
}


// ########################################
// System functions
// ########################################
// Description de system functions: Son funciones que se utilizarán en el código
// de Gobu para crear sistemas, como por ejemplo, crear un sistema de renderizado,
// un sistema de animación, etc.
//

void gb_animate_sprite_stop(gb_animate_sprite_t* animated)
{
    animated->is_playing = false;
    animated->current_frame = 0;
    animated->counter = 0.0f;
}

void gb_animate_sprite_pause(gb_animate_sprite_t* animated)
{
    animated->is_playing = false;
}

void gb_animate_sprite_play(gb_animate_sprite_t* animated)
{
    animated->is_playing = true;
}

bool gb_animate_sprite_is_playing(gb_animate_sprite_t* animated)
{
    return animated->is_playing;
}

bool gb_animate_sprite_is_looping(gb_animate_sprite_t* animated)
{
    gb_animate_animation_t* animation = ecs_vec_get_t(&animated->animations, gb_animate_animation_t, animated->current);
    return animation->loop;
}

void gb_animate_sprite_set(gb_animate_sprite_t* animated, const char* name)
{
    for (int i = 0; i < ecs_vec_count(&animated->animations); i++)
    {
        gb_animate_animation_t* animation = ecs_vec_get_t(&animated->animations, gb_animate_animation_t, i);

        if (strcmp(animation->name, name) == 0) {
            animated->current = i;
            animated->current_frame = 0;
            animated->counter = 0.0f;
            animated->is_playing = true;
            return;
        }
    }

    gb_animate_sprite_stop(animated);
}

static void gb_ecs_observe_set_gb_animate_sprite_t(ecs_iter_t* it)
{
    gb_animate_sprite_t* animate = ecs_field(it, gb_animate_sprite_t, 1);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 2);

    ecs_entity_t event = it->event;

    for (int i = 0; i < it->count; i++)
    {
        if (event == EcsOnSet) {
            ecs_vec_init(NULL, &animate->animations, ecs_id(gb_animate_animation_t), 200);
        }
        else if (event == EcsOnRemove) {
            ecs_vec_fini(NULL, &animate->animations, ecs_id(gb_animate_animation_t));
        }
    }
}

static void gb_ecs_update_gb_animate_sprite_t(ecs_iter_t* it)
{
    gb_animate_sprite_t* animated = ecs_field(it, gb_animate_sprite_t, 1);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 2);

    for (int i = 0; i < it->count; i++)
    {
        if (ecs_vec_count(&animated[i].animations) > 0)
        {
            gb_animate_animation_t* animation = ecs_vec_get_t(&animated[i].animations, gb_animate_animation_t, animated[i].current);
            int count_frames = ecs_vec_count(&animation->frames);

            if (count_frames > 0)
            {
                gb_animate_frame_t* frame = ecs_vec_get_t(&animation->frames, gb_animate_frame_t, animated[i].current_frame);

                if (animated[i].is_playing)
                {
                    animated[i].counter += GetFrameTime();
                    if ((animated[i].counter) >= (float)(60 / animation->fps) * (float)(frame->duration * 0.02f))
                    {
                        animated[i].counter = 0.0f;
                        animated[i].current_frame++;
                        if (animated[i].current_frame >= count_frames)
                        {
                            animated[i].current_frame = 0;
                            if (!animation->loop)
                            {
                                animated[i].is_playing = false;
                            }
                        }
                    }
                }

                // clip sprite
                float x = frame->sprite.src.x;
                float y = frame->sprite.src.y;
                float w = frame->sprite.dst.width;
                float h = frame->sprite.dst.height;

                float clip_width = (w == 0) ? frame->sprite.texture.width : w;
                float clip_height = (h == 0) ? frame->sprite.texture.height : h;

                sprite[i].texture = frame->sprite.texture;
                sprite[i].src = (gb_rect_t){ x, y, frame->sprite.texture.width, frame->sprite.texture.height };
                sprite[i].dst = (gb_rect_t){ 0.0f, 0.0f, clip_width, clip_height };
                sprite[i].tint = (frame->sprite.tint.a == 0) ? (gb_color_t) { 255, 255, 255, 255 } : frame->sprite.tint;
            }
            else {
                animated[i].is_playing = false;
                sprite[i].texture.id = 0;
            }
        }
        else {
            animated[i].is_playing = false;
            sprite[i].texture.id = 0;
        }
    }
}

// deprecated: Ahora el sistema sera por frame....
static void gb_ecs_observe_set_gb_animated_t(ecs_iter_t* it)
{
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 1);
    gb_animated_t* animated = ecs_field(it, gb_animated_t, 2);

    for (int i = 0; i < it->count; i++)
    {
        gb_resource_t* resource = gb_resource(it->world, animated[i].resource);
        g_return_if_fail(resource != NULL);
        binn* json = resource->json;

        char* anim_id = (animated[i].animation == NULL) ? binn_object_str(json, "default") : animated[i].animation;
        animated[i].width = binn_object_int32(json, "width");
        animated[i].height = binn_object_int32(json, "height");

        sprite[i].src.width = animated[i].width;
        sprite[i].src.height = animated[i].height;

        sprite[i].dst.width = animated[i].width;
        sprite[i].dst.height = animated[i].height;

        binn* animation = binn_object_object(binn_object_object(json, "animations"), anim_id);
        {
            animated[i].loop = binn_object_bool(animation, "loop");
            animated[i].speed = binn_object_int32(animation, "speed");

            binn* frames = binn_object_list(animation, "frames");
            animated[i].count = binn_count(frames);
            for (int n = 1; n <= animated[i].count; n++) {
                animated[i].frames[n - 1] = binn_list_int32(frames, n);
            }

            animated[i].start_frame = animated[i].frames[0];
            animated[i].end_frame = animated[i].frames[(animated[i].count - 1)];
            animated[i].current = 0;
        }
    }
}

static void gb_ecs_observe_set_gb_sprite_t(ecs_iter_t* it)
{
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        gb_resource_t* resource = gb_resource(it->world, sprite[i].resource);
        g_return_if_fail(resource != NULL);
        g_return_if_fail(resource->texture.id != 0);

        if (resource->json != NULL)
        {
            sprite[i] = gb_sprite_deserialize(resource->json);
        }

        sprite[i].texture = resource->texture;

        SET_DEFAULT_VALUE(sprite[i].src.width, resource->texture.width);
        SET_DEFAULT_VALUE(sprite[i].src.height, resource->texture.height);
        SET_DEFAULT_VALUE(sprite[i].dst.width, resource->texture.width);
        SET_DEFAULT_VALUE(sprite[i].dst.height, resource->texture.height);

        sprite[i].tint = (sprite[i].tint.a == 0) ? (gb_color_t) { 255, 255, 255, 255 } : sprite[i].tint;
    }
}

static void gb_ecs_observe_set_gb_text_t(ecs_iter_t* it)
{
    gb_text_t* text = ecs_field(it, gb_text_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        text[i].font = gb_resource(it->world, text[i].resource)->font;
        text[i].size = (text[i].size <= 0) ? 20.0f : text[i].size;
        text[i].spacing = (text[i].spacing <= 0.0f) ? (text[i].size / 10) : text[i].spacing;
    }
}

static void gb_ecs_observe_set_gb_resource_t(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    gb_resource_t* resource = ecs_field(it, gb_resource_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        char* path = gb_path_join_relative_content(resource[i].path);

        if (event == EcsOnSet) {
            if (IsFileExtension(path, ".png") || IsFileExtension(path, ".jpg"))
                resource[i].texture = LoadTexture(path);
            if (IsFileExtension(path, ".ttf"))
                resource[i].font = LoadFont(path);
            if (IsFileExtension(path, ".asprites"))
                resource[i].json = binn_serialize_from_file(path);
            if (IsFileExtension(path, ".sprite")) {
                resource[i].json = binn_serialize_from_file(path);
                resource[i].texture = LoadTexture(gb_path_join_relative_content(binn_object_str(resource[i].json, "resource")));
            }
        }
        else if (event == EcsOnRemove) {
            if (IsFileExtension(path, ".png") || IsFileExtension(path, ".jpg"))
                UnloadTexture(resource[i].texture);
            if (IsFileExtension(path, ".ttf"))
                UnloadFont(resource[i].font);
            if (IsFileExtension(path, ".asprites") || IsFileExtension(path, ".sprite"))
                binn_free(resource[i].json);
        }

        free(path);
    }
}

static void gb_ecs_observer_set_gb_app_t(ecs_iter_t* it)
{
    ecs_entity_t event = it->event;
    gb_app_t* win = ecs_field(it, gb_app_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (event == EcsOnSet)
            InitWindow(win->width, win->height, win->title);
        else if (event == EcsOnRemove)
            CloseWindow();
    }
}

static void gb_ecs_observer_set_gb_camera_t(ecs_iter_t* it)
{
    gb_camera_t* camera = ecs_field(it, gb_camera_t, 1);
    for (int i = 0; i < it->count; i++)
    {
        camera[i].zoom = (camera[i].zoom == 0) ? 1.0f : camera[i].zoom;
    }
}

static void gb_ecs_update_gb_camera_t(ecs_iter_t* it)
{
    gb_camera_t* camera = ecs_field(it, gb_camera_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (camera[i].mode == GB_CAMERA_FOLLOWING)
        {
        }
        else if (camera[i].mode == GB_CAMERA_EDITOR)
        {
            // move camera mouse movement
            if (engine.input.mouse_button_down(MOUSE_BUTTON_RIGHT))
            {
                gb_vec2_t delta = engine.input.mouse_delta();
                camera[i].target.x -= delta.x / camera[i].zoom;
                camera[i].target.y -= delta.y / camera[i].zoom;
            }

            // zoom with mouse wheel
            float wheel = engine.input.mouse_wheel();
            if (wheel != 0)
            {
                gb_vec2_t mouseWorld = engine.screen_to_world(camera[i], engine.input.mouse_position());

                camera[i].offset = engine.input.mouse_position();
                camera[i].target = mouseWorld;
                camera[i].zoom -= wheel * 0.05f;
                if (camera[i].zoom < 0.1f) camera[i].zoom = 0.1f;
            }
        }
    }
}

// deprecated: Ahora el sistema sera por frame....
static void gb_ecs_update_gb_animated_t(ecs_iter_t* it)
{
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 1);
    gb_animated_t* animated = ecs_field(it, gb_animated_t, 2);

    for (int i = 0; i < it->count; i++)
    {
        gb_texture_t texture = sprite[i].texture;
        int frame_width = animated[i].width;
        int frame_height = animated[i].height;
        int start_frame = animated[i].start_frame;
        int end_frame = animated[i].end_frame;
        float speed = (float)(animated[i].speed * 0.01f);

        if (animated[i].is_playing)
        {
            animated[i].counter += GetFrameTime();
            if (animated[i].counter >= speed)
            {
                animated[i].counter = 0.0f;
                animated[i].current += 1;
                if (animated[i].current > (animated[i].count - 1))
                {
                    if (animated[i].loop)
                        animated[i].current = 0;
                    else {
                        animated[i].current = (animated[i].count - 1);
                        animated[i].is_playing = false;
                    }
                }
            }
        }

        int frame_now = animated[i].frames[animated[i].current];
        int frame_row = (texture.width / frame_width);

        sprite[i].src.x = (frame_now % frame_row) * frame_width;
        sprite[i].src.y = (frame_now / frame_row) * frame_height;
    }
}

static void gb_ecs_update_gb_bounding_t(ecs_iter_t* it)
{
    gb_bounding_t* bounding = ecs_field(it, gb_bounding_t, 1);
    gb_transform_t* transform = ecs_field(it, gb_transform_t, 2);

    // Drawing
    gb_shape_rect_t* rect = ecs_field(it, gb_shape_rect_t, 3);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 4);
    gb_text_t* text = ecs_field(it, gb_text_t, 5);

    for (int i = 0; i < it->count; i++)
    {
        bounding[i].size.x = 0.0f;
        bounding[i].size.y = 0.0f;

        if (rect) {
            bounding[i].size = (gb_vec2_t){ rect[i].width, rect[i].height };
        }

        if (sprite) {
            bounding[i].size = (gb_vec2_t){ sprite[i].dst.width, sprite[i].dst.height };
        }

        if (text) {
            bounding[i].size = MeasureTextEx(text[i].font, text[i].text, text[i].size, text[i].spacing);
        }

        bounding[i].min.x = transform[i].position.x - (transform[i].origin.x * bounding[i].size.x);
        bounding[i].min.y = transform[i].position.y - (transform[i].origin.y * bounding[i].size.y);
        bounding[i].max.x = bounding[i].size.x;
        bounding[i].max.y = bounding[i].size.y;
        bounding[i].center.x = bounding[i].min.x + (bounding[i].size.x / 2);
        bounding[i].center.y = bounding[i].min.y + (bounding[i].size.y / 2);
    }
}

static void gb_ecs_update_gb_gizmos_t(ecs_iter_t* it)
{
    gb_gizmos_t* gizmos = ecs_field(it, gb_gizmos_t, 1);
    gb_transform_t* transform = ecs_field(it, gb_transform_t, 2);
    gb_bounding_t* bounding = ecs_field(it, gb_bounding_t, 3);

    ecs_entity_t Engine = ecs_lookup(it->world, "Engine");
    gb_camera_t* camera = ecs_get(it->world, Engine, gb_camera_t);

    gb_vec2_t mouse = engine.screen_to_world(*camera, engine.input.mouse_position());
    gb_vec2_t delta = engine.input.mouse_delta();

    bool shift = engine.input.key_down(KEY_LEFT_SHIFT);
    // bool ctrl = engine.input.key_down(KEY_LEFT_CONTROL);
    bool mouse_btn_pres_left = engine.input.mouse_button_pressed(MOUSE_BUTTON_LEFT);
    bool mouse_btn_down_left = engine.input.mouse_button_down(MOUSE_BUTTON_LEFT);

    for (int i = it->count - 1; i >= 0; i--)
    {
        Rectangle bonding = (Rectangle){ bounding[i].min.x, bounding[i].min.y, bounding[i].max.x, bounding[i].max.y };
        ecs_entity_t entity = it->entities[i];

        // seleccionamos una sola entidad por click
        if (mouse_btn_pres_left)
        {
            bool selected = CheckCollisionPointRec(mouse, bonding);

            // no deseleccionamos cuando tenemos shift presionado
            if (!shift)
            {
                for (int n = it->count - 1; n >= 0; n--)
                {
                    gizmos[n].selected = false;
                }
            }

            if (selected)
            {
                gizmos[i].selected = true;
                break;
            }
        }

        if (mouse_btn_down_left)
        {
            if (gizmos[i].selected)
            {
                transform[i].position.x += delta.x;
                transform[i].position.y += delta.y;
            }
        }
    }
}


static void gb_ecs_predraw_begin_drawing_rendering(ecs_iter_t* it)
{
    gb_app_t* win = ecs_field(it, gb_app_t, 1);
    gb_camera_t* camera = ecs_field(it, gb_camera_t, 2);

    for (int i = 0; i < it->count; i++)
    {
        Camera2D cam = {
            .offset = camera[i].offset,
            .target = camera[i].target,
            .rotation = camera[i].rotation,
            .zoom = camera[i].zoom,
        };

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(cam);

        if (win[i].show_grid)
            gb_rendering_draw_grid_2d(win[i].width, 48, (gb_color_t) { 10, 10, 10, 255 });
    }
}

static void gb_ecs_postdraw_drawing_rendering(ecs_iter_t* it)
{
    gb_transform_t* transform = ecs_field(it, gb_transform_t, 1);
    gb_bounding_t* bounding = ecs_field(it, gb_bounding_t, 2);

    // Drawing
    gb_gizmos_t* gismos = ecs_field(it, gb_gizmos_t, 3);
    gb_shape_rect_t* rect = ecs_field(it, gb_shape_rect_t, 4);
    gb_sprite_t* sprite = ecs_field(it, gb_sprite_t, 5);
    gb_text_t* text = ecs_field(it, gb_text_t, 6);

    for (int i = 0; i < it->count; i++)
    {
        gb_transform_t trans = transform[i];

        gb_gfx_push_matrix();
        {
            gb_gfx_translate(trans.position.x, trans.position.y, 0.0f);
            gb_gfx_rotate(trans.rotation, 0.0f, 0.0f, 1.0f);
            gb_gfx_translate(-(trans.origin.x * bounding[i].size.x), -(trans.origin.y * bounding[i].size.y), 0.0f);
            gb_gfx_scale(trans.scale.x, trans.scale.y, 1.0f);

            if (rect) {
                gb_rendering_draw_rect(rect[i]);
            }

            if (sprite) {
                if (sprite[i].texture.id != 0)
                    gb_rendering_draw_sprite(sprite[i]);
            }

            if (text) {
                if (text[i].font.texture.id != 0)
                    gb_rendering_draw_text(text[i]);
            }

            if (gismos) {
                if (gismos[i].selected)
                    gb_rendering_draw_gismos(trans, bounding[i]);
            }
        }
        gb_gfx_pop_matrix();
    }
}

static void gb_ecs_postdraw_enddrawing_rendering(ecs_iter_t* it)
{
    gb_app_t* win = ecs_field(it, gb_app_t, 1);

    for (int i = 0; i < it->count; i++)
    {
        if (win[i].show_fps)
            DrawFPS(10, 20);

        EndMode2D();
        EndDrawing();

        if (WindowShouldClose()) {
            ecs_quit(it->world);
        }
    }
}

static void gb_engine_system_init(gb_world_t* world)
{
    render_phases.PreDraw = ecs_new_w_id(world, EcsPhase);
    render_phases.Background = ecs_new_w_id(world, EcsPhase);
    render_phases.Draw = ecs_new_w_id(world, EcsPhase);
    render_phases.PostDraw = ecs_new_w_id(world, EcsPhase);

    ecs_add_pair(world, render_phases.PreDraw, EcsDependsOn, EcsOnStore);
    ecs_add_pair(world, render_phases.Background, EcsDependsOn, render_phases.PreDraw);
    ecs_add_pair(world, render_phases.Draw, EcsDependsOn, render_phases.Background);
    ecs_add_pair(world, render_phases.PostDraw, EcsDependsOn, render_phases.Draw);

    ecs_observer(world, {
        .filter = {.terms = { {.id = ecs_id(gb_sprite_t)} }},
        .events = { EcsOnSet },
        .callback = gb_ecs_observe_set_gb_sprite_t
    });

    ecs_observer(world, {
        .filter = {.terms = { {.id = ecs_id(gb_text_t)} }},
        .events = { EcsOnSet },
        .callback = gb_ecs_observe_set_gb_text_t
    });

    ecs_observer(world, {
        .filter = {.terms = { {.id = ecs_id(gb_sprite_t)}, {.id = ecs_id(gb_animated_t)} }},
        .events = { EcsOnSet },
        .callback = gb_ecs_observe_set_gb_animated_t
    });

    ecs_observer(world, {
        .filter = {.terms = { {.id = ecs_id(gb_animate_sprite_t)}, {.id = ecs_id(gb_sprite_t)} }},
        .events = { EcsOnSet },
        .callback = gb_ecs_observe_set_gb_animate_sprite_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_update_gb_animate_sprite_t", .add = {ecs_dependson(EcsPostUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_animate_sprite_t)}, {.id = ecs_id(gb_sprite_t)} },
        .callback = gb_ecs_update_gb_animate_sprite_t
    });

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_resource_t)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = gb_ecs_observe_set_gb_resource_t
    });

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_app_t)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = gb_ecs_observer_set_gb_app_t
    });

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_camera_t)}}},
        .events = { EcsOnSet },
        .callback = gb_ecs_observer_set_gb_camera_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_update_gb_bounding_t", .add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_bounding_t)}, {.id = ecs_id(gb_transform_t)}, {.id = ecs_id(gb_shape_rect_t), .oper = EcsOptional}, {.id = ecs_id(gb_sprite_t), .oper = EcsOptional}, {.id = ecs_id(gb_text_t), .oper = EcsOptional} },
        .callback = gb_ecs_update_gb_bounding_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_update_gb_camera_t", .add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_camera_t)} },
        .callback = gb_ecs_update_gb_camera_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_update_gb_animated_t", .add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_sprite_t)}, {.id = ecs_id(gb_animated_t)} },
        .callback = gb_ecs_update_gb_animated_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_update_gb_gizmos_t", .add = {ecs_dependson(EcsOnUpdate)} }),
        .query.filter.terms = { {.id = ecs_id(gb_gizmos_t)}, {.id = ecs_id(gb_transform_t)}, {.id = ecs_id(gb_bounding_t)} },
        .callback = gb_ecs_update_gb_gizmos_t
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_predraw_begin_drawing_rendering", .add = {ecs_dependson(render_phases.PreDraw)} }),
        .query.filter.terms = { {.id = ecs_id(gb_app_t)}, {.id = ecs_id(gb_camera_t)} },
        .callback = gb_ecs_predraw_begin_drawing_rendering
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_postdraw_drawing_rendering", .add = {ecs_dependson(render_phases.Draw)} }),
        .query.filter.terms = { {.id = ecs_id(gb_transform_t)}, {.id = ecs_id(gb_bounding_t)}, {.id = ecs_id(gb_gizmos_t), .oper = EcsOptional}, {.id = ecs_id(gb_shape_rect_t), .oper = EcsOptional}, {.id = ecs_id(gb_sprite_t), .oper = EcsOptional}, {.id = ecs_id(gb_text_t), .oper = EcsOptional} },
        .callback = gb_ecs_postdraw_drawing_rendering
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "gb_ecs_postdraw_enddrawing_rendering", .add = {ecs_dependson(render_phases.PostDraw)} }),
        .query.filter.terms = { {.id = ecs_id(gb_app_t)} },
        .callback = gb_ecs_postdraw_enddrawing_rendering
    });

}


// ########################################
// Resource functions
// ########################################
// Description de resource functions: Son funciones que se utilizarán en el código
// de Gobu para crear recursos, como por ejemplo, crear un recurso de tipo textura,
// un recurso de tipo fuente, etc.
//

/**
 * @brief Establece un recurso en el mundo de entidades.
 *
 * Esta función permite establecer un recurso en el mundo de entidades especificado.
 * El recurso se identifica mediante una clave y se especifica su ruta de acceso.
 *
 * @param world El mundo de entidades en el que se establecerá el recurso.
 * @param key La clave que identifica el recurso.
 * @param path La ruta de acceso al recurso.
 * @return true si se estableció el recurso correctamente, false en caso contrario.
 */
const char* gb_resource_set(gb_world_t* world, const char* path)
{
    const char* path_relative = gb_path_relative_content(path);
    const char* key = gb_path_normalize(gb_strdups("resource://%s", gb_str_replace(path_relative, ".", "!")));

    if (ecs_is_valid(world, ecs_lookup(world, key)) == false) {
        ecs_entity_t resource = ecs_new_entity(world, key);
        ecs_set(world, resource, gb_resource_t, { .path = path_relative });
    }

    return key;
}

/**
 * @brief Obtiene un recurso de Gobu.
 *
 * Esta función devuelve un puntero al recurso identificado por la clave especificada.
 *
 * @param world Puntero al mundo de entidades de Gobu.
 * @param key Clave del recurso a obtener.
 * @return Puntero al recurso correspondiente a la clave especificada, o NULL si no se encuentra.
 */
const gb_resource_t* gb_resource(gb_world_t* world, const char* key)
{
    ecs_entity_t resource = ecs_lookup(world, key);
    g_return_val_if_fail(resource != 0, NULL);
    return (gb_resource_t*)ecs_get(world, resource, gb_resource_t);
}

/**
 * @brief Elimina un recurso del mundo.
 *
 * Esta función elimina un recurso del mundo dado utilizando la clave especificada.
 *
 * @param world El mundo del cual eliminar el recurso.
 * @param key La clave del recurso a eliminar.
 * @return true si el recurso se eliminó correctamente, false en caso contrario.
 */
bool gb_resource_remove(gb_world_t* world, const char* key)
{
    ecs_entity_t resource = ecs_lookup(world, key);
    g_return_val_if_fail(resource != 0, false);
    ecs_delete(world, resource);
    return ecs_lookup(world, key) == 0;
}

// ########################################
// ECS functions
// ########################################
// Description de ecs functions: Son funciones que se utilizarán en el código
// de Gobu para crear entidades, como por ejemplo, crear una entidad de tipo
// sprite, una entidad de tipo texto, etc.
//

ecs_entity_t gb_ecs_entity_new(gb_world_t* world, ecs_entity_t parent, const char* name, const gb_transform_t t)
{
    ecs_entity_t entity = ecs_new_id(world);
    if (parent != 0)
        gb_ecs_entity_set_parent(world, parent, entity);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, parent, name, ".", ".", true);
    char* name_id = (lookup == 0 ? name : gb_strdups("%s%lld", name, entity));
    gb_ecs_entity_set_name(world, entity, name_id);

    gb_ecs_entity_set(world, entity, gb_info_t, { .name = gb_strdup(name) });
    gb_ecs_entity_set(world, entity, gb_transform_t, { .position = t.position, .scale = t.scale, .rotation = t.rotation, .origin = t.origin });
    gb_ecs_entity_set(world, entity, gb_bounding_t, { 0 });
    gb_ecs_entity_set(world, entity, gb_gizmos_t, { .selected = false });

    return entity;
}

void gb_ecs_entity_set_parent(gb_world_t* world, ecs_entity_t parent, ecs_entity_t entity)
{
    ecs_add_pair(world, entity, EcsChildOf, parent);
}

ecs_entity_t gb_ecs_entity_get_parent(gb_world_t* world, ecs_entity_t entity)
{
    ecs_entity_t parent = ecs_get_parent(world, entity);
    return parent;
}

const char* gb_ecs_entity_get_name(gb_world_t* world, ecs_entity_t entity)
{
    return ecs_get_name(world, entity);
}

ecs_entity_t gb_ecs_entity_set_name(gb_world_t* world, ecs_entity_t entity, const char* name)
{
    return ecs_set_name(world, entity, gb_strdup(name));
}

void gb_ecs_vec_remove(ecs_vec_t* v, ecs_size_t size, int32_t index)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index < v->count, ECS_OUT_OF_RANGE, NULL);
    if (index == --v->count) {
        return;
    }

    for (int32_t i = index; i < v->count; i++) {
        ecs_os_memcpy(
            ECS_ELEM(v->array, size, i),
            ECS_ELEM(v->array, size, i + 1),
            size);
    }
}

void gb_ecs_vec_swap(ecs_vec_t* v, ecs_size_t size, int32_t index_a, int32_t index_b)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index_a < v->count, ECS_OUT_OF_RANGE, NULL);
    ecs_assert(index_b < v->count, ECS_OUT_OF_RANGE, NULL);

    if (index_a == index_b) {
        return;
    }

    void* buffer = ecs_os_malloc(size);
    ecs_os_memcpy(buffer, ECS_ELEM(v->array, size, index_a), size);
    ecs_os_memcpy(ECS_ELEM(v->array, size, index_a), ECS_ELEM(v->array, size, index_b), size);
    ecs_os_memcpy(ECS_ELEM(v->array, size, index_b), buffer, size);
    ecs_os_free(buffer);
}

// ########################################
// APP functions
// ########################################
// Description de window app functions: 

static bool input_ready = FALSE;

gb_world_t* gb_app_init(gb_app_t* app)
{
    gb_world_t* world = ecs_init();

    gb_input_init();
    gb_engine_component_init(world);
    gb_engine_system_init(world);

    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    ecs_entity_t Engine = ecs_new_entity(world, "Engine");
    ecs_set(world, Engine, gb_app_t, {
        .title = app->title,         // Establece el título de la ventana
        .width = app->width,         // Establece el ancho de la ventana
        .height = app->height,       // Establece la altura de la ventana
        .fps = app->fps,             // Establece los cuadros por segundo
        .show_fps = app->show_fps,   // Establece si se muestra los FPS
        .show_grid = app->show_grid  // Establece si se muestra la cuadrícula
    });
    ecs_set(world, Engine, gb_camera_t, { .zoom = 1.0f, .rotation = 0.0f });
    SetTargetFPS(app->fps ? app->fps : 60);

    return world;
}

void gb_app_main(gb_world_t* world)
{
    while (ecs_progress(world, GetFrameTime())) {}
}

void gb_app_progress(gb_world_t* world)
{
    ecs_progress(world, GetFrameTime());
}

// ########################################
// Sprites functions
// ########################################

gb_sprite_t gb_sprite_deserialize(binn* fsprite)
{
    gb_sprite_t sprite = { 0 };

    sprite.resource = binn_object_str(fsprite, "resource");

    sprite.src.x = binn_object_float(fsprite, "x");
    sprite.src.y = binn_object_float(fsprite, "y");
    sprite.dst.width = binn_object_float(fsprite, "width");
    sprite.dst.height = binn_object_float(fsprite, "height");
    sprite.src.width = sprite.dst.width;
    sprite.src.height = sprite.dst.height;
    sprite.dst.x = 0.0f;
    sprite.dst.y = 0.0f;

    return sprite;
}

binn* gb_sprite_serialize(gb_sprite_t sprite)
{
    binn* fsprite = binn_object();
    binn_object_set_str(fsprite, "resource", sprite.resource);
    binn_object_set_float(fsprite, "x", sprite.src.x);
    binn_object_set_float(fsprite, "y", sprite.src.y);
    binn_object_set_float(fsprite, "width", sprite.src.width);
    binn_object_set_float(fsprite, "height", sprite.src.height);
    return fsprite;
}

// ########################################
// Engine functions
// ########################################

static gb_vec2_t getscreentoworld2d(gb_camera_t camera, gb_vec2_t position)
{
    Camera2D cam = {
        .offset = (gb_vec2_t){camera.offset.x, camera.offset.y},
        .target = (gb_vec2_t){camera.target.x, camera.target.y},
        .rotation = camera.rotation,
        .zoom = camera.zoom,
    };
    return GetScreenToWorld2D(position, cam);
}

static gb_vec2_t getworldtoscreen2d(gb_camera_t camera, gb_vec2_t position)
{
    Camera2D cam = {
        .offset = (gb_vec2_t){camera.offset.x, camera.offset.y},
        .target = (gb_vec2_t){camera.target.x, camera.target.y},
        .rotation = camera.rotation,
        .zoom = camera.zoom,
    };
    return GetWorldToScreen2D(position, cam);
}

static void gb_input_init(void)
{
    engine.input.mouse_button_down = IsMouseButtonDown;
    engine.input.mouse_button_pressed = IsMouseButtonPressed;
    engine.input.mouse_button_released = IsMouseButtonReleased;
    engine.input.mouse_position = GetMousePosition;
    engine.input.mouse_delta = GetMouseDelta;
    engine.input.mouse_wheel = GetMouseWheelMove;
    engine.input.mouse_position_set = SetMousePosition;
    engine.input.mouse_cursor_set = SetMouseCursor;

    engine.input.key_down = IsKeyDown;
    engine.input.key_pressed = IsKeyPressed;
    engine.input.key_released = IsKeyReleased;

    engine.world_to_screen = getworldtoscreen2d;
    engine.screen_to_world = getscreentoworld2d;

    input_ready = TRUE;
}

