#include "gobu.h"
#include "thirdparty/goburender/rlgl.h"
#include "config.h"

// --------------------------------------------------------------
// Cree el archivo gobu-engine para implementar la idea
// de tener todo o casi todo en un solo lugar se que esto
// seria mas dificil de mantener y es una idea que loca.
// En una version futura se podria dividir en varios archivos.
// --------------------------------------------------------------

ECS_COMPONENT_DECLARE(gb_render_phases_t);
ECS_COMPONENT_DECLARE(gb_app_t);
ECS_COMPONENT_DECLARE(gb_info_t);

gb_render_phases_t render_phases;
gb_engine_t engine;

static bool gb_setting_load(const char* filename);
static void gb_input_init(void);


// ########################################
// Utility functions
// ########################################
// Description de utility functions: Son funciones que se utilizarán en el código
// de Gobu para realizar tareas específicas, como por ejemplo, obtener el nombre
// de un archivo, verificar si existe, etc.
//

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
    ECS_COMPONENT_DEFINE(world, gb_render_phases_t);
    ECS_COMPONENT_DEFINE(world, gb_app_t);
    ECS_COMPONENT_DEFINE(world, gb_info_t);

    ecs_struct(world, {
        .entity = ecs_id(gb_info_t),
        .members = {
            {.name = "name", .type = ecs_id(ecs_string_t) },
            {.name = "type", .type = ecs_id(ecs_u8_t) },
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(gb_resource_t),
        .members = {
            {.name = "path", .type = ecs_id(ecs_string_t) },
        }
    });
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

    gb_type_rect_module_import(world);
    gb_type_vec2_module_import(world);
    gb_type_color_module_import(world);
    gb_type_texture_module_import(world);
    gb_sprite_module_import(world);

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_id(gb_app_t)}}},
        .events = { EcsOnSet, EcsOnRemove },
        .callback = gb_ecs_observer_set_gb_app_t
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

const char* gb_resource_key_normalize(const char* path)
{
    if (strstr(path, "resource://") != NULL)
        return path;
    else
        return gb_path_normalize(gb_strdups("resource://%s", gb_str_replace(path, ".", "!")));
}

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
        ecs_set(world, resource, gb_resource_t, { .path = gb_strdup(path_relative) });
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

