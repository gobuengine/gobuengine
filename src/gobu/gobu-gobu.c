#include "gobu-gobu.h"
#include "thirdparty/binn/binn_json.h"

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
        Camera2D camera;
    } CameraManager;

    GobuEcsWorld *world;
} GobuCoreData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

static GobuCoreData CORE = {0};

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

/*

*/

/**
 * Convierte un objeto binario binn en una cadena JSON.
 *
 * Esta función toma un objeto binario (binn) y lo convierte en una cadena JSON,
 * devolviendo un puntero a la cadena resultante.
 *
 * @param b  El objeto binario binn que se va a convertir en JSON.
 *
 * @return   Un puntero a la cadena JSON resultante. Esta cadena debe ser liberada
 *           por el llamador cuando ya no sea necesaria.
 */
const char *gobu_json_stringify(binn *b)
{
    return binn_serialize(b);
}

/**
 * Analiza una cadena JSON y crea un objeto binario binn.
 *
 * Esta función toma una cadena JSON como entrada y la analiza para crear
 * un objeto binario (binn) equivalente, devolviendo un puntero al objeto binario resultante.
 *
 * @param json_string  La cadena JSON que se va a analizar.
 *
 * @return   Un puntero al objeto binario binn resultante, o NULL en caso de error.
 *           El objeto binario debe ser liberado por el llamador cuando ya no sea necesario.
 */
binn *gobu_json_parse(char *json_string)
{
    return binn_serialize_load(json_string);
}

/**
 * Carga un objeto binario binn desde un archivo JSON.
 *
 * Esta función carga un archivo JSON desde la ubicación especificada por "filename"
 * y crea un objeto binario (binn) equivalente, devolviendo un puntero al objeto binario resultante.
 *
 * @param filename  La ruta al archivo JSON que se va a cargar.
 *
 * @return   Un puntero al objeto binario binn resultante, o NULL en caso de error.
 *           El objeto binario debe ser liberado por el llamador cuando ya no sea necesario.
 */
binn *gobu_json_load_from_file(const char *filename)
{
    return binn_serialize_from_file(filename);
}

/**
 * Guarda un objeto binario binn como un archivo JSON.
 *
 * Esta función toma un objeto binario (binn) y lo guarda como un archivo JSON
 * en la ubicación especificada por "filename".
 *
 * @param b         El objeto binario binn que se va a guardar como JSON.
 * @param filename  La ruta donde se guardará el archivo JSON.
 *
 * @return   true si la operación de guardado fue exitosa, false en caso de error.
 */
bool gobu_json_save_to_file(binn *b, const char *filename)
{
    return binn_deserialize_from_file(b, filename);
}

/*

*/

/**
 * Carga un proyecto Gobu desde una ubicación especificada.
 *
 * Esta función se encarga de cargar un proyecto Gobu desde la ubicación especificada
 * por la ruta "path".
 *
 * @param path  La ruta al proyecto Gobu que se va a cargar.
 *
 * @return   TRUE si la operación de carga fue exitosa, FALSE en caso de error.
 */
gboolean gobu_project_load(const gchar *path)
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
 * Obtiene la ruta del proyecto actual de Gobu.
 *
 * Esta función se encarga de obtener la ruta del proyecto actual de Gobu,
 * devolviendo un puntero a la cadena que representa la ruta.
 *
 * @return   Un puntero a la cadena que contiene la ruta del proyecto actual de Gobu.
 */
const char *gobu_project_get_path(void)
{
    return g_strdup(CORE.project.path);
}

/**
 * Obtiene el nombre del proyecto actual de Gobu.
 *
 * Esta función se encarga de obtener el nombre del proyecto actual de Gobu,
 * devolviendo un puntero a la cadena que representa el nombre.
 *
 * @return   Un puntero a la cadena que contiene el nombre del proyecto actual de Gobu.
 */
const char *gobu_project_get_name(void)
{
    return g_strdup(CORE.project.name);
}

void gobu_camera_begin(void)
{
    BeginMode2D(CORE.CameraManager.camera);
}

void gobu_camera_end(void)
{
    EndMode2D();
}

/*

*/

/**
 * Inicializa el subsistema de renderización en Gobu.
 *
 * Esta función se encarga de realizar la inicialización necesaria para el subsistema
 * de renderización en Gobu, preparando el entorno para la renderización.
 */
void gobu_render_init(int width, int height)
{
    srand((unsigned int)time(NULL));

    // sg_setup(&(sg_desc){0});
    // sgp_setup(&(sgp_desc){0});

    InitWindow(width, height, NULL);

    CORE.CameraManager.camera.target = (Vector2){0.0f, 0.0f};
    CORE.CameraManager.camera.offset = (Vector2){width/2.0f, height/2.0f};
    CORE.CameraManager.camera.zoom = 1.0f;
    CORE.CameraManager.camera.rotation = 0.0f;
}

/**
 * Apaga el subsistema de renderización en Gobu de manera segura.
 *
 * Esta función se encarga de realizar el apagado adecuado del subsistema de
 * renderización en Gobu, liberando recursos y cerrando conexiones de manera segura.
 */
void gobu_render_shutdown(void)
{
    gobu_ecs_world_free();
    // sgp_shutdown();
    // sg_shutdown();
    CloseWindow();
}

/**
 * Inicia un nuevo frame de renderización en Gobu.
 *
 * Esta función se encarga de iniciar un nuevo frame de renderización en Gobu
 * con las dimensiones especificadas y el color de fondo dado.
 *
 * @param width   El ancho del frame de renderización en píxeles.
 * @param height  El alto del frame de renderización en píxeles.
 * @param color   El color de fondo del frame de renderización.
 */
void gobu_render_frame_begin(Color color)
{
    BeginDrawing();
    gobu_render_clear_color((Color){0.0f, 0.0f, 0.0f, 1.0f});
}

/**
 * Finaliza el frame de renderización actual en Gobu.
 *
 * Esta función se encarga de finalizar el frame de renderización actual en Gobu
 * con las dimensiones especificadas.
 *
 * @param width   El ancho del frame de renderización en píxeles.
 * @param height  El alto del frame de renderización en píxeles.
 */
void gobu_render_frame_end(int width, int height)
{
    EndDrawing();
}

/**
 * Renderiza un viewport en Gobu con las dimensiones y posición especificadas.
 *
 * Esta función se encarga de renderizar un viewport en Gobu en la posición y
 * dimensiones especificadas.
 *
 * @param x       La coordenada X de la esquina superior izquierda del viewport.
 * @param y       La coordenada Y de la esquina superior izquierda del viewport.
 * @param width   El ancho del viewport en píxeles.
 * @param height  El alto del viewport en píxeles.
 */
void gobu_render_viewport(int x, int y, int width, int height)
{
}

/**
 * Renderiza un proyecto en Gobu con límites de visualización especificados.
 *
 * Esta función se encarga de renderizar un proyecto en Gobu con los límites de
 * visualización especificados, definiendo el área visible en el mundo del proyecto.
 *
 * @param left    El límite izquierdo del área de visualización.
 * @param right   El límite derecho del área de visualización.
 * @param top     El límite superior del área de visualización.
 * @param bottom  El límite inferior del área de visualización.
 */
void gobu_render_project(float left, float right, float top, float bottom)
{
}

/**
 * Establece el color de fondo para la renderización en Gobu.
 *
 * Esta función se encarga de establecer el color de fondo que se utilizará
 * para limpiar el buffer de renderización en Gobu antes de renderizar.
 *
 * @param color  El color de fondo deseado.
 */
void gobu_render_clear_color(Color color)
{
    ClearBackground(RAYWHITE);
}

/**
 * Limpia el buffer de renderización en Gobu.
 *
 * Esta función se encarga de limpiar el buffer de renderización en Gobu, utilizando
 * el color de fondo previamente configurado.
 */
void gobu_render_clear(void)
{
}

/**
 * Restablece el color de renderización en Gobu al valor predeterminado.
 *
 * Esta función se encarga de restablecer el color de renderización en Gobu
 * al valor predeterminado, eliminando cualquier modificación previa.
 */
void gobu_render_reset_color(void)
{
}

/**
 * Establece el color de renderización en Gobu.
 *
 * Esta función se encarga de establecer el color que se utilizará para renderizar
 * elementos en Gobu, como líneas, formas o rellenos.
 *
 * @param color  El color deseado para la renderización.
 */
void gobu_render_set_color(Color color)
{
}

/**
 * Configura la rotación en Gobu alrededor de un punto específico.
 *
 * Esta función se encarga de configurar la rotación en Gobu alrededor de un punto
 * específico definido por las coordenadas (x, y), utilizando un ángulo de rotación
 * de theta en radianes.
 *
 * @param theta  El ángulo de rotación en radianes.
 * @param x      La coordenada X del punto de rotación.
 * @param y      La coordenada Y del punto de rotación.
 */
void gobu_render_set_rotate(float theta, float x, float y)
{
}

/**
 * Configura la escala en Gobu en un punto específico.
 *
 * Esta función se encarga de configurar la escala en Gobu utilizando los factores de escala
 * sx y sy en el punto de origen definido por las coordenadas (x, y).
 *
 * @param sx  El factor de escala en el eje X.
 * @param sy  El factor de escala en el eje Y.
 * @param x   La coordenada X del punto de origen de la escala.
 * @param y   La coordenada Y del punto de origen de la escala.
 */
void gobu_render_set_scale(float sx, float sy, float x, float y)
{
}

/**
 * Configura una traslación en Gobu en las coordenadas (x, y).
 *
 * Esta función se encarga de configurar una traslación en Gobu para mover los elementos
 * gráficos en las coordenadas (x, y).
 *
 * @param x  La coordenada X de la traslación.
 * @param y  La coordenada Y de la traslación.
 */
void gobu_render_set_translate(float x, float y)
{
}

/*

*/

/**
 * Dibuja un rectángulo relleno con transformaciones en Gobu.
 *
 * Esta función se encarga de dibujar un rectángulo relleno en Gobu, con la posibilidad de
 * aplicar transformaciones como escala, origen, rotación y color personalizado.
 *
 * @param rect      El rectángulo a dibujar (coordenadas y dimensiones).
 * @param scale     Los factores de escala en los ejes X e Y.
 * @param origin    El punto de origen para las transformaciones.
 * @param rotation  El ángulo de rotación en radianes.
 * @param color     El color de relleno del rectángulo.
 */
void gobu_shape_draw_filled_rect(Rectangle rect, Vector2 scale, Vector2 origin, float rotation, Color color)
{
    float ox = origin.x <= 0 ? 1.0f : origin.x;
    float oy = origin.y <= 0 ? 1.0f : origin.y;
    Vector2 pivot = (Vector2){rect.width * ox, rect.height * oy};
    DrawRectanglePro(rect, pivot, rotation, color);
}

/**
 * Dibuja un tablero de ajedrez en Gobu con dimensiones personalizadas.
 *
 * Esta función se encarga de dibujar un tablero de ajedrez en Gobu con las dimensiones
 * especificadas, considerando el ancho y alto del tablero, así como el ancho y alto de la pantalla.
 *
 * @param width          El ancho del tablero de ajedrez.
 * @param height         El alto del tablero de ajedrez.
 * @param screen_width   El ancho de la pantalla en la que se dibujará el tablero.
 * @param screen_height  El alto de la pantalla en la que se dibujará el tablero.
 */
void gobu_shape_draw_checkboard(int width, int height, int screen_width, int screen_height)
{
    for (int y = 0; y < screen_height / height + 1; y++)
        for (int x = 0; x < screen_width / width + 1; x++)
            if ((x + y) % 2 == 0)
                gobu_shape_draw_filled_rect((Rectangle){x * width, y * height, width, height},
                (Vector2){1.0f, 1.0f}, (Vector2){0.0f, 0.0f}, 0.0f, gobu_color_rgb_to_color(60, 60, 60));
}

/*

*/

/**
 * Dibuja una textura en Gobu con propiedades personalizadas.
 *
 * Esta función se encarga de dibujar una textura en Gobu, utilizando las propiedades
 * especificadas, como la región de origen (source), la región de destino (dest), y un
 * color de tintado opcional.
 *
 * @param texture  La textura que se va a dibujar.
 * @param source   La región de origen en la textura (coordenadas y dimensiones).
 * @param dest     La región de destino en el renderizado (coordenadas y dimensiones).
 * @param tint     El color de tintado opcional para la textura.
 */
void gobu_texture_draw_texture(Texture texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint)
{
    if (texture.id > 0)
    {
        DrawTexturePro(texture, source, dest, origin, rotation, tint);
    }
}

/**
 * Carga una textura desde un archivo en Gobu.
 *
 * Esta función se encarga de cargar una textura desde un archivo en Gobu, utilizando
 * la ruta especificada por "filename".
 *
 * @param filename  La ruta al archivo de la textura que se va a cargar.
 *
 * @return   Una instancia de Texture que representa la textura cargada.
 */
Texture gobu_texture_load_file(const char *filename)
{
    return LoadTexture(filename);
}

/**
 * Libera la memoria de una textura en Gobu.
 *
 * Esta función se encarga de liberar la memoria y recursos asociados a una textura
 * en Gobu, permitiendo su eliminación segura.
 *
 * @param texture  La textura que se va a liberar de memoria.
 */
void gobu_texture_free(Texture texture)
{
    UnloadTexture(texture);
}

/*

*/

/**
 * Convierte valores RGB de 8 bits en un objeto Color.
 *
 * Esta función toma los componentes de color en formato de 8 bits (r, g, b) y los
 * convierte en un objeto Color que representa el color especificado.
 *
 * @param r  El componente rojo (red) de 8 bits.
 * @param g  El componente verde (green) de 8 bits.
 * @param b  El componente azul (blue) de 8 bits.
 *
 * @return   Un objeto Color que representa el color especificado.
 */
Color gobu_color_rgb_to_color(uint8_t r, uint8_t g, uint8_t b)
{
    return gobu_color_rgba_to_color(r, g, b, 255);
}

/**
 * Convierte valores RGBA de 8 bits en un objeto Color.
 *
 * Esta función toma los componentes de color en formato de 8 bits (r, g, b, a) y los
 * convierte en un objeto Color que representa el color especificado.
 *
 * @param r  El componente rojo (red) de 8 bits.
 * @param g  El componente verde (green) de 8 bits.
 * @param b  El componente azul (blue) de 8 bits.
 * @param a  El componente alfa (alpha) de 8 bits que representa la transparencia.
 *
 * @return   Un objeto Color que representa el color especificado.
 */
Color gobu_color_rgba_to_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (Color){r, g, b, a};
}

/**
 * Genera un color aleatorio en formato RGB.
 *
 * Esta función genera un color aleatorio en formato RGB (Red, Green, Blue) con componentes
 * en el rango de 0 a 255 (8 bits por componente).
 *
 * @param r  Puntero a una variable que almacenará el componente rojo generado.
 * @param g  Puntero a una variable que almacenará el componente verde generado.
 * @param b  Puntero a una variable que almacenará el componente azul generado.
 *
 * @return   Una instancia de Color que representa el color aleatorio generado.
 */
Color gobu_color_random_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t fr = (uint8_t)gobu_math_random((float)r);
    uint8_t fg = (uint8_t)gobu_math_random((float)g);
    uint8_t fb = (uint8_t)gobu_math_random((float)b);
    return gobu_color_rgb_to_color(fr, fg, fb);
}

/*

*/

/**
 * Genera un número aleatorio en el rango [0, number) en Gobu.
 *
 * Esta función genera un número aleatorio en el rango [0, number) utilizando el generador
 * de números aleatorios en Gobu. El valor "number" especifica el límite superior del rango,
 * y el número generado estará en el intervalo [0, number).
 *
 * @param number  El límite superior del rango para la generación aleatoria.
 *
 * @return   Un número aleatorio en el rango [0, number).
 */
float gobu_math_random(float number)
{
    return (((float)rand() / (float)(RAND_MAX)) * number);
}

/*

*/

/**
 * Inicializa un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de crear y configurar un mundo de entidades y componentes
 * en Gobu, proporcionando una instancia de GobuEcsWorld lista para su uso.
 *
 * @return   Una instancia de GobuEcsWorld inicializada y lista para su uso.
 */
void gobu_ecs_world_init(void)
{
    CORE.world = ecs_init();
}

/**
 * Obtiene el mundo de entidades y componentes actual en Gobu.
 *
 * Esta función se encarga de obtener el mundo de entidades y componentes actual en el contexto
 * de Gobu, permitiendo acceder y manipular las entidades y sistemas en el mundo.
 *
 * @return   Una instancia de GobuEcsWorld que representa el mundo actual de entidades y componentes en Gobu.
 */
GobuEcsWorld *gobu_ecs_world_get(void)
{
    return CORE.world;
}

/**
 * Libera la memoria y recursos de un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de liberar la memoria y recursos asociados a un mundo de
 * entidades y componentes en Gobu, permitiendo su eliminación segura.
 *
 */
void gobu_ecs_world_free(void)
{
    ecs_fini(CORE.world);
}

/**
 * Avanza el progreso en un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de avanzar el progreso en un mundo de entidades y componentes
 * en Gobu, lo que puede implicar la actualización de la lógica del juego y sistemas asociados.
 *
 */
void gobu_ecs_world_progress(void)
{
    ecs_progress(CORE.world, 0);
}

/**
 * Ejecuta un sistema en un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de ejecutar un sistema especificado en el mundo de entidades y componentes
 * proporcionado. El sistema puede realizar operaciones y actualizaciones basadas en el delta de tiempo
 * proporcionado.
 *
 * @param system  El sistema que se va a ejecutar en el mundo.
 * @param delta   El delta de tiempo que indica el intervalo entre fotogramas para actualizaciones basadas en tiempo.
 */
void gobu_ecs_world_run(GobuEcsEntity system, float delta)
{
    ecs_run(CORE.world, system, delta, NULL);
}

/**
 * Crea una nueva entidad en un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de crear una nueva entidad en el mundo de entidades y componentes
 * proporcionado, y opcionalmente le asigna un nombre descriptivo.
 *
 * @param name   El nombre opcional de la entidad para referencia.
 *
 * @return   Una instancia de GobuEcsEntity que representa la entidad creada.
 */
GobuEcsEntity gobu_ecs_entity_new(const char *name)
{
    GobuEcsEntity e = ecs_new_id(CORE.world);
    ecs_set_name(CORE.world, e, name);
    return e;
}

/**
 * Obtiene un iterador para los hijos de una entidad en un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de obtener un iterador que permite acceder a los hijos de la entidad
 * especificada en el mundo de entidades y componentes proporcionado.
 *
 * @param entity  La entidad de la cual se obtienen los hijos.
 *
 * @return   Un iterador (GobuEcsIter) que permite acceder a los hijos de la entidad.
 */
GobuEcsIter gobu_ecs_entity_get_children(GobuEcsEntity entity)
{
    return ecs_children(CORE.world, entity);
}

/**
 * Avanza al siguiente elemento utilizando un iterador en un contexto de entidades y componentes en Gobu.
 *
 * Esta función se encarga de avanzar al siguiente elemento utilizando el iterador proporcionado,
 * permitiendo iterar a través de los elementos en un contexto de entidades y componentes en Gobu.
 *
 * @param iter  El iterador (GobuEcsIter) que se va a avanzar.
 *
 * @return   Devuelve `true` si se pudo avanzar al siguiente elemento, o `false` si no hay más elementos disponibles.
 */
bool gobu_ecs_iter_next(GobuEcsIter *iter)
{
    return ecs_children_next(iter);
}

/**
 * Obtiene el nombre de una entidad en un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de obtener el nombre de la entidad especificada en el mundo de
 * entidades y componentes proporcionado.
 *
 * @param entity  La entidad de la cual se obtiene el nombre.
 *
 * @return   Un puntero a una cadena de caracteres (const char *) que representa el nombre de la entidad.
 *           Si la entidad no tiene un nombre asignado, se devuelve un puntero nulo (NULL).
 */
const char *gobu_ecs_entity_get_name(GobuEcsEntity entity)
{
    return ecs_get_name(CORE.world, entity);
}

/**
 * Elimina una entidad de un mundo de entidades y componentes en Gobu.
 *
 * Esta función se encarga de eliminar la entidad especificada del mundo de entidades y componentes,
 * lo que implica la liberación de recursos y la eliminación de la entidad de manera segura.
 *
 * @param entity  La entidad que se va a eliminar del mundo.
 */
void gobu_ecs_entity_delete(GobuEcsEntity entity)
{
    ecs_delete(CORE.world, entity);
}
