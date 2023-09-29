#include "gobu-gobu.h"

#include "thirdparty/binn/binn_json.h"

#define SOKOL_GLCORE33
#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#include "thirdparty/sokol/sokol_gfx.h"
#include "thirdparty/sokol/sokol_gp.h"
#include "thirdparty/sokol/sokol_glue.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct GobuCoreData
{
    struct
    {
        gchar *path;
        gchar *name;
    } project;

    struct
    {
        GobuCamera main;
    } CameraManager;

    struct
    {
        GobuEcsWorld *world;
        GobuEcsEntity Root;
    } Ecs;
} GobuCoreData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

static GobuCoreData CORE = {0};

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

static void _GobuEcsComponentDefaultInit(void);

/*

*/

/**
 * Convierte un objeto binn en una cadena JSON.
 *
 * @param b Puntero al objeto binn que se va a convertir.
 * @return Una cadena JSON que representa el objeto binn, o NULL si falla.
 */
const char *GobuJsonStringify(binn *b)
{
    return binn_serialize(b);
}

/**
 * Parsea una cadena JSON y la convierte en un objeto binn.
 *
 * @param json_string La cadena JSON que se va a parsear.
 * @return Un puntero al objeto binn creado a partir de la cadena JSON, o NULL si falla.
 */
binn *GobuJsonParse(char *json_string)
{
    return binn_serialize_load(json_string);
}

/**
 * Carga un objeto binn desde un archivo en formato JSON.
 *
 * @param filename El nombre del archivo desde el que se va a cargar el objeto JSON.
 * @return Un puntero al objeto binn cargado desde el archivo, o NULL si falla.
 */
binn *GobuJsonLoadFromFile(const char *filename)
{
    return binn_serialize_from_file(filename);
}

/**
 * Guarda un objeto binn en un archivo en formato JSON.
 *
 * @param b Puntero al objeto binn que se va a guardar.
 * @param filename El nombre del archivo en el que se va a guardar el objeto JSON.
 * @return TRUE si la operación de guardado se realizó con éxito, FALSE si falla.
 */
gboolean GobuJsonSaveToFile(binn *b, const char *filename)
{
    return binn_deserialize_from_file(b, filename);
}

/*

*/

/**
 * Carga un proyecto Gobu desde la ruta especificada.
 *
 * @param path La ruta al directorio del proyecto Gobu que se va a cargar.
 * @return TRUE si la carga del proyecto se realizó con éxito, FALSE si falla.
 */
gboolean GobuProjectLoad(const gchar *path)
{
    gboolean test = g_file_test(path, G_FILE_TEST_EXISTS);
    if (test == TRUE)
    {
        CORE.project.path = g_path_get_dirname(path);
        CORE.project.name = g_basename(CORE.project.path);
        // CORE.project.component = g_build_filename(CORE.project.path, "Scripts", "game.gcomponent", NULL);
    }
    return test;
}

/**
 * Obtiene la ruta actual del proyecto Gobu cargado.
 *
 * @return Una cadena que representa la ruta del proyecto Gobu cargado, o NULL si no hay un proyecto cargado.
 */
const char *GobuProjectGetPath(void)
{
    return g_strdup(CORE.project.path);
}

/**
 * Obtiene el nombre del proyecto Gobu cargado.
 *
 * @return Una cadena que representa el nombre del proyecto Gobu cargado, o NULL si no hay un proyecto cargado.
 */
const char *GobuProjectGetName(void)
{
    return g_strdup(CORE.project.name);
}

/*

*/

/**
 * Inicializa el sistema de renderizado de Gobu.
 */
void GobuRenderInit(void)
{
    sg_setup(&(sg_desc){0});
    sgp_setup(&(sgp_desc){0});

    CORE.CameraManager.main.pos = (GobuVec2){0.0f, 0.0f};
    CORE.CameraManager.main.scale = (GobuVec2){1.0f, 1.0f};
    CORE.CameraManager.main.rotation = 0.0f;

    CORE.Ecs.world = GobuEcsWorldInit();
    _GobuEcsComponentDefaultInit();

    CORE.Ecs.Root = GobuEcsEntityNew("Root");
}

/**
 * Apaga y libera los recursos del sistema de renderizado de Gobu.
 */
void GobuRenderShutdown(void)
{
    GobuEcsWorldFree(CORE.Ecs.world);
    sgp_shutdown();
    sg_shutdown();
}

/**
 * Inicia un nuevo frame de renderizado con las dimensiones y color de fondo especificados.
 *
 * @param width Ancho del frame en píxeles.
 * @param height Alto del frame en píxeles.
 * @param color Color de fondo del frame.
 */
void GobuRenderFrameBegin(int width, int height, GobuColor color)
{
    float ratio = width / (float)height;

    sgp_begin(width, height);
    GobuRenderViewport(0, 0, width, height);
    // GobuRenderProject(width * -0.5f, width * 0.5f, height * 0.5f, height * -0.5f);
    GobuRenderSetScale(CORE.CameraManager.main.scale.x, CORE.CameraManager.main.scale.y, 0, 0);
    GobuRenderSetTranslate(CORE.CameraManager.main.pos.x, CORE.CameraManager.main.pos.y);
    GobuRenderSetRotate(CORE.CameraManager.main.rotation, 0, 0);
    GobuRenderClearColor(color);
}

/**
 * Finaliza el frame de renderizado actual con las dimensiones especificadas.
 *
 * @param width Ancho del frame en píxeles.
 * @param height Alto del frame en píxeles.
 */
void GobuRenderFrameEnd(int width, int height)
{
    sg_pass_action pass_action = {0};
    sg_begin_default_pass(&pass_action, width, height);
    sgp_flush();
    sgp_end();
    sg_end_pass();
    sg_commit();
}

/**
 * Establece la vista del renderizado en una región específica de la pantalla.
 *
 * @param x Coordenada X de la esquina superior izquierda de la región de la vista.
 * @param y Coordenada Y de la esquina superior izquierda de la región de la vista.
 * @param width Ancho de la región de la vista en píxeles.
 * @param height Alto de la región de la vista en píxeles.
 */
void GobuRenderViewport(int x, int y, int width, int height)
{
    sgp_viewport(x, y, width, height);
}

/**
 * Establece la proyección del renderizado con coordenadas de vista especificadas.
 *
 * @param left Coordenada X izquierda de la vista.
 * @param right Coordenada X derecha de la vista.
 * @param top Coordenada Y superior de la vista.
 * @param bottom Coordenada Y inferior de la vista.
 */
void GobuRenderProject(float left, float right, float top, float bottom)
{
    sgp_project(left, right, top, bottom);
}

/**
 * Establece el color de fondo para limpiar el frame.
 *
 * @param color Color de fondo.
 */
void GobuRenderClearColor(GobuColor color)
{
    sgp_set_blend_mode(SGP_BLENDMODE_BLEND);
    GobuRenderSetColor(color);
    GobuRenderClear();
}

/**
 * Limpia el frame con el color de fondo actual.
 */
void GobuRenderClear(void)
{
    sgp_clear();
}

/**
 * Restablece el color de dibujo a su valor predeterminado.
 */
void GobuRenderResetColor(void)
{
    sgp_reset_color();
}

/**
 * Establece el color de dibujo actual para las operaciones de renderizado.
 *
 * @param color El color a utilizar en las operaciones de dibujo.
 */
void GobuRenderSetColor(GobuColor color)
{
    sgp_set_color(color.r, color.g, color.b, color.a);
}

/**
 * Establece una rotación para las transformaciones de renderizado.
 *
 * @param theta Ángulo de rotación en radianes.
 * @param x Coordenada X del punto de origen de la rotación.
 * @param y Coordenada Y del punto de origen de la rotación.
 */
void GobuRenderSetRotate(float theta, float x, float y)
{
    sgp_rotate_at(theta, x, y);
}

/**
 * Establece una escala para las transformaciones de renderizado.
 *
 * @param sx Factor de escala en el eje X.
 * @param sy Factor de escala en el eje Y.
 * @param x Coordenada X del punto de origen de la escala.
 * @param y Coordenada Y del punto de origen de la escala.
 */
void GobuRenderSetScale(float sx, float sy, float x, float y)
{
    sgp_scale_at(sx, sy, x, y);
}

/**
 * Establece una traslación para las transformaciones de renderizado.
 *
 * @param x Cantidad de traslación en el eje X.
 * @param y Cantidad de traslación en el eje Y.
 */
void GobuRenderSetTranslate(float x, float y)
{
    sgp_translate(x, y);
}

/*

*/

/**
 * Dibuja un rectángulo relleno en el sistema de renderizado de Gobu.
 *
 * @param rect Las dimensiones y posición del rectángulo.
 * @param scale Escala a aplicar al rectángulo (ancho y alto).
 * @param origin Punto de origen para la transformación.
 * @param rotation Ángulo de rotación en radianes.
 * @param color El color con el que se rellena el rectángulo.
 */
void GobuShapeDrawFilledRect(GobuRectangle rect, GobuVec2 scale, GobuVec2 origin, float rotation, GobuColor color)
{
    float ox = origin.x <= 0 ? 1.0f : origin.x;
    float oy = origin.y <= 0 ? 1.0f : origin.y;

    GobuVec2 pivot = (GobuVec2){rect.w * ox, rect.h * oy};

    GobuRenderSetColor(color);
    sgp_push_transform();
    GobuRenderSetTranslate(rect.x, rect.y);
    GobuRenderSetScale(scale.x, scale.y, pivot.x, pivot.y);
    GobuRenderSetRotate(rotation, pivot.x, pivot.y);
    sgp_draw_filled_rect(0, 0, rect.w, rect.h);
    sgp_pop_transform();
    GobuRenderResetColor();
}

/**
 * Dibuja un tablero de ajedrez en el sistema de renderizado de Gobu.
 *
 * @param width Ancho de una casilla del tablero.
 * @param height Alto de una casilla del tablero.
 * @param screen_width Ancho total de la pantalla.
 * @param screen_height Alto total de la pantalla.
 */
void GobuShapeDrawCheckboard(int width, int height, int screen_width, int screen_height)
{
    GobuRenderSetColor(GobuColorRGBToFloat(13, 10, 14));
    GobuRenderClear();

    for (int y = 0; y < screen_height / height + 1; y++)
        for (int x = 0; x < screen_width / width + 1; x++)
            if ((x + y) % 2 == 0)
                GobuShapeDrawFilledRect((GobuRectangle){x * width, y * height, width, height}, (GobuVec2){1.0f, 1.0f}, (GobuVec2){0.0f, 0.0f}, 0.0f, GobuColorRGBToFloat(17, 14, 18));
    GobuRenderResetColor();
}

/*

*/

/**
 * Dibuja una textura en el sistema de renderizado de Gobu.
 *
 * @param texture La textura que se va a dibujar.
 * @param source El área de la textura a utilizar como fuente.
 * @param dest El área de destino en la que se dibujará la textura.
 * @param tint El color de tintado que se aplicará a la textura (color multiplicativo).
 */
void GobuTextureDrawTexture(GobuTexture texture, GobuRectangle source, GobuRectangle dest, GobuColor tint)
{
    if (texture.id > 0)
    {
        sgp_set_image(0, (sg_image){.id = texture.id});
        sgp_set_blend_mode(SGP_BLENDMODE_BLEND);
        GobuRenderSetColor(tint);
        sgp_draw_textured_rect_ex(0, (sgp_rect){dest.x, dest.y, dest.w, dest.h}, (sgp_rect){source.x, source.y, source.w, source.h});
        sgp_reset_blend_mode();
        sgp_unset_image(0);
    }
}

/**
 * Carga una textura desde un archivo y la devuelve como un objeto GobuTexture.
 *
 * @param filename El nombre del archivo de imagen desde el que se va a cargar la textura.
 * @return Un objeto GobuTexture que representa la textura cargada, o NULL si falla la carga.
 */
GobuTexture GobuTextureLoadFile(const char *filename)
{
    GobuTexture texture = {0};

    int comp;
    stbi_uc *data = stbi_load(filename, &texture.width, &texture.height, &comp, STBI_rgb_alpha);

    if (data != NULL)
    {
        sg_image d = sg_make_image(&(sg_image_desc){
            .width = texture.width,
            .height = texture.height,
            .data.subimage[0][0] = {
                .ptr = data,
                .size = (size_t){texture.width * texture.height * 4},
            },
        });
        texture.id = d.id;

        stbi_image_free(data);
    }

    return texture;
}

/**
 * Libera la memoria asociada a una textura GobuTexture previamente cargada.
 *
 * @param texture El objeto GobuTexture que se va a liberar.
 */
void GobuTextureFree(GobuTexture texture)
{
    sg_image d = (sg_image){.id = texture.id};
    sg_destroy_image(d);
}

/*

*/

/**
 * Convierte valores RGB de 8 bits en un objeto GobuColor con componentes de punto flotante.
 *
 * @param r Valor de componente rojo (0-255).
 * @param g Valor de componente verde (0-255).
 * @param b Valor de componente azul (0-255).
 * @return Un objeto GobuColor con componentes de punto flotante normalizados en el rango [0.0, 1.0].
 */
GobuColor GobuColorRGBToFloat(uint8_t r, uint8_t g, uint8_t b)
{
    float fr = (float)((float)r / 255);
    float fg = (float)((float)g / 255);
    float fb = (float)((float)b / 255);
    return (GobuColor){fr, fg, fb, 1.0f};
}

/**
 * Convierte valores RGBA de 8 bits en un objeto GobuColor con componentes de punto flotante.
 *
 * @param r Valor de componente rojo (0-255).
 * @param g Valor de componente verde (0-255).
 * @param b Valor de componente azul (0-255).
 * @param a Valor de componente alfa (0-255).
 * @return Un objeto GobuColor con componentes de punto flotante normalizados en el rango [0.0, 1.0].
 */
GobuColor GobuColorRGBAToFloat(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    float fr = (float)((float)r / 255);
    float fg = (float)((float)g / 255);
    float fb = (float)((float)b / 255);
    float fa = (float)((float)a / 255);
    return (GobuColor){fr, fg, fb, fa};
}

/*

*/

GobuEcsWorld *GobuEcsWorldInit(void)
{
    GobuEcsWorld *world = ecs_init();
    return world;
}

void GobuEcsWorldFree(GobuEcsWorld *world)
{
    ecs_fini(world);
}

GobuEcsEntity GobuEcsEntityNew(const char *name)
{
    GobuEcsEntity e = ecs_new_id(CORE.Ecs.world);
    ecs_set_name(CORE.Ecs.world, e, name);
    return e;
}

static void _GobuEcsComponentDefaultInit(void)
{
    GobuEcsEntity e_GobuVec2 = ecs_component_init(CORE.Ecs.world, &(ecs_component_desc_t){
        .entity = ecs_entity(CORE.Ecs.world, {.name = "GobuVec2"}),
        .type.size = ECS_SIZEOF(GobuVec2),
        .type.alignment = ECS_ALIGNOF(GobuVec2)
    });

    ecs_struct(CORE.Ecs.world, {
        .entity = e_GobuVec2, 
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t)},
            {.name = "y", .type = ecs_id(ecs_f32_t)},
        }
    });

    GobuEcsEntity e_GobuColor= ecs_component_init(CORE.Ecs.world, &(ecs_component_desc_t){
        .entity = ecs_entity(CORE.Ecs.world, {.name = "GobuColor"}),
        .type.size = ECS_SIZEOF(GobuColor),
        .type.alignment = ECS_ALIGNOF(GobuColor)
    });

    ecs_struct(CORE.Ecs.world, {
        .entity = e_GobuColor, 
        .members = {
            {.name = "r", .type = ecs_id(ecs_f32_t)},
            {.name = "g", .type = ecs_id(ecs_f32_t)},
            {.name = "b", .type = ecs_id(ecs_f32_t)},
            {.name = "a", .type = ecs_id(ecs_f32_t)},
        }
    });

    GobuEcsEntity e_GobuRectangle= ecs_component_init(CORE.Ecs.world, &(ecs_component_desc_t){
        .entity = ecs_entity(CORE.Ecs.world, {.name = "GobuRectangle"}),
        .type.size = ECS_SIZEOF(GobuRectangle),
        .type.alignment = ECS_ALIGNOF(GobuRectangle)
    });

    ecs_struct(CORE.Ecs.world, {
        .entity = e_GobuRectangle, 
        .members = {
            {.name = "x", .type = ecs_id(ecs_f32_t)},
            {.name = "y", .type = ecs_id(ecs_f32_t)},
            {.name = "w", .type = ecs_id(ecs_f32_t)},
            {.name = "h", .type = ecs_id(ecs_f32_t)},
        }
    });

}

