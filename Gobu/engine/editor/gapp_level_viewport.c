#include "gapp_level_viewport.h"
#include "gapp_main.h"

struct _GappLevelViewport
{
    GtkWidget parent;
    GappLevelEditor* editor;
};

G_DEFINE_TYPE_WITH_PRIVATE(GappLevelViewport, gapp_level_viewport, GAPP_TYPE_GOBU_EMBED);

static void signal_viewport_start(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_render(GappLevelViewport* viewpor, gpointer data);
static void signal_viewport_resize(GappLevelViewport* viewpor, int width, int height, gpointer data);
static void signal_viewport_drop(GappLevelViewport* viewpor, GFile* file, double x, double y, gpointer data);
static gboolean signal_viewport_zoom(GtkEventControllerScroll* controller, gdouble dx, gdouble dy, GtkWidget* widget);
static void signal_viewport_mouse_move(GtkEventControllerMotion* controller, double x, double y, GtkWidget* widget);
static void signal_viewport_mouse_button_pressed(GtkGestureClick* gesture, int n_press, double x, double y, GtkWidget* widget);
static void signal_viewport_mouse_button_released(GtkGestureClick* gesture, int n_press, double x, double y, GtkWidget* widget);

static void gapp_level_viewport_class_init(GappLevelViewportClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gapp_level_viewport_init(GappLevelViewport* self)
{
    g_signal_connect(self, "gobu-embed-start", G_CALLBACK(signal_viewport_start), NULL);
    g_signal_connect(self, "gobu-embed-render", G_CALLBACK(signal_viewport_render), NULL);
    g_signal_connect(self, "gobu-embed-resize", G_CALLBACK(signal_viewport_resize), NULL);
    g_signal_connect(self, "gobu-embed-drop", G_CALLBACK(signal_viewport_drop), NULL);

    GtkEventController* zoom = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(zoom));
    g_signal_connect(zoom, "scroll", G_CALLBACK(signal_viewport_zoom), self);

    GtkEventController* motion = gtk_event_controller_motion_new();
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(motion));
    g_signal_connect(motion, "motion", G_CALLBACK(signal_viewport_mouse_move), self);

    GtkGesture* gesture = gtk_gesture_click_new();
    gtk_widget_add_controller(self, GTK_EVENT_CONTROLLER(gesture));
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    g_signal_connect(gesture, "pressed", G_CALLBACK(signal_viewport_mouse_button_pressed), self);
    g_signal_connect(gesture, "released", G_CALLBACK(signal_viewport_mouse_button_released), self);
}

/**
 * @brief Crea una nueva instancia de GappLevelViewport.
 *
 * @param editor El editor de nivel asociado al viewport.
 * @return Un puntero a la nueva instancia de GappLevelViewport.
 */
GappLevelViewport* gapp_level_viewport_new(GappLevelEditor* editor)
{
    GappLevelViewport* self = g_object_new(GAPP_LEVEL_TYPE_VIEWPORT, NULL);
    {
        self->editor = editor;
    }
    return self;
}

/**
 * @brief Función que se ejecuta cuando se inicia la vista del viewport.
 *
 * @param viewport El objeto GappLevelViewport.
 * @param data Datos adicionales pasados como argumento.
 */
static void signal_viewport_start(GappLevelViewport* viewport, gpointer data)
{
    int width = gapp_gobu_embed_get_width(viewport);
    int height = gapp_gobu_embed_get_height(viewport);
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);

    gobu_rendering_init(world, &(GWindow){.title = "LevelEditor", .width = width, .height = height, .viewport = true, .show_grid = true});

    GCamera* camera = ecs_singleton_get(world, GCamera);
    camera->mode = CAMERA_EDITOR;
}

/**
 * @brief Renderiza el viewport de nivel.
 *
 * Esta función se encarga de renderizar el viewport de nivel.
 *
 * @param viewport El viewport de nivel a renderizar.
 * @param data Datos adicionales pasados a la función de señal.
 */
static void signal_viewport_render(GappLevelViewport* viewport, gpointer data)
{
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);
    gobu_rendering_progress(world);
}

/**
 * @brief Función que se ejecuta cuando se redimensiona el viewport.
 *
 * @param viewport El viewport que se está redimensionando.
 * @param width El nuevo ancho del viewport.
 * @param height La nueva altura del viewport.
 * @param data Datos adicionales pasados a la función.
 */
static void signal_viewport_resize(GappLevelViewport* viewport, int width, int height, gpointer data)
{
    ViewportSizeCallback(width, height);
}


/**
 * @brief Maneja la señal de soltar en el viewport del nivel.
 *
 * Esta función se llama cuando se suelta un archivo en el viewport del nivel.
 *
 * @param viewport El viewport del nivel.
 * @param file El archivo soltado.
 * @param x La posición x donde se soltó el archivo.
 * @param y La posición y donde se soltó el archivo.
 * @param data Datos adicionales pasados al manejador de señales.
 */
static void signal_viewport_drop(GappLevelViewport* viewport, GFile* file, double x, double y, gpointer data)
{
    gchar* filename = g_file_get_path(file);
    gchar* name = gb_fs_get_name(filename, true);
    ecs_world_t* world = gapp_level_editor_get_world(viewport->editor);

    /**
     * Establece el recurso para el mundo dado con el nombre y el nombre de archivo especificados.
     * 
     */
    if (gobu_resource_set(world, name, filename))
        debug_print(CONSOLE_INFO, gb_strdups("Resource load: %s", name));

    /**
     * Obtiene la cámara principal y el sistema de entrada del sistema de componentes de entidad del mundo.
     * Convierte las coordenadas de la pantalla a coordenadas del mundo utilizando la matriz de transformación de la cámara principal.
     * 
     */
    GCamera* cameraMain = ecs_singleton_get(world, GCamera);
    GInputSystem* input = ecs_singleton_get(world, GInputSystem);
    Vector2 mouseWorld = input->get_screen_to_world((Vector2) { x, y }, * cameraMain);

    // Crea una nueva entidad y le asigna un nombre.
    ecs_entity_t entity = ecs_new_id(world);
    ecs_set_name(world, entity, gb_strdups("%s%d", name, entity));
    ecs_add_pair(world, entity, EcsChildOf, ecs_lookup(world, "World"));
    ecs_set(world, entity, GPosition, { mouseWorld.x, mouseWorld.y });

    if (gb_fs_is_extension(filename, ".png")) {
        ecs_set(world, entity, GSprite, { .resource = name });
        debug_print(CONSOLE_INFO, gb_strdups("Spawn Entity [%lld], Component Sprite [%s]", entity, name));
    }
}

/**
 * @brief Función que maneja la señal de zoom en el viewport.
 *
 * Esta función se encarga de manejar la señal de zoom en el viewport de la aplicación.
 *
 * @param controller El controlador de eventos de desplazamiento.
 * @param dx La cantidad de desplazamiento horizontal.
 * @param dy La cantidad de desplazamiento vertical.
 * @param widget El widget del viewport.
 * @return gboolean Devuelve TRUE si el evento fue manejado correctamente, FALSE en caso contrario.
 */
static gboolean signal_viewport_zoom(GtkEventControllerScroll* controller, gdouble dx, gdouble dy, GtkWidget* widget)
{
    SetMouseWheelMove(0.0f, dy);
    return FALSE;
}

/**
 * @brief Maneja el evento de movimiento del mouse en el viewport.
 *
 * @param controller El controlador del evento de movimiento del mouse.
 * @param x La coordenada x del mouse.
 * @param y La coordenada y del mouse.
 * @param widget El widget en el que se produce el evento.
 */
static void signal_viewport_mouse_move(GtkEventControllerMotion* controller, double x, double y, GtkWidget* widget)
{
    gtk_widget_set_can_focus(GTK_GL_AREA(widget), true);
    gtk_widget_grab_focus(widget);
    SetMouse(x, y);
}

/**
 * @brief Función que se ejecuta cuando se presiona un botón del mouse en el viewport.
 *
 * @param gesture El gesto de clic asociado al evento.
 * @param n_press El número de veces que se ha presionado el botón del mouse.
 * @param x La coordenada X del punto donde se ha presionado el botón del mouse.
 * @param y La coordenada Y del punto donde se ha presionado el botón del mouse.
 * @param widget El widget en el que se ha producido el evento.
 */
static void signal_viewport_mouse_button_pressed(GtkGestureClick* gesture, int n_press, double x, double y, GtkWidget* widget)
{
    gint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    switch (button)
    {
    case 1:
        SetMouseButton(MOUSE_BUTTON_LEFT, 1);
        break;
    case 2:
        SetMouseButton(MOUSE_BUTTON_MIDDLE, 1);
        break;
    case 3:
        SetMouseButton(MOUSE_BUTTON_RIGHT, 1);
        break;
    default:
    }

    gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

/**
 * @brief Función que se ejecuta cuando se suelta un botón del mouse en el viewport.
 *
 * @param gesture El gesto de clic que se activó.
 * @param n_press El número de veces que se presionó el botón del mouse.
 * @param x La coordenada x del punto donde se soltó el botón del mouse.
 * @param y La coordenada y del punto donde se soltó el botón del mouse.
 * @param widget El widget en el que se soltó el botón del mouse.
 */
static void signal_viewport_mouse_button_released(GtkGestureClick* gesture, int n_press, double x, double y, GtkWidget* widget)
{
    gint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));

    switch (button)
    {
    case 1:
        SetMouseButton(MOUSE_BUTTON_LEFT, 0);
        break;
    case 2:
        SetMouseButton(MOUSE_BUTTON_MIDDLE, 0);
        break;
    case 3:
        SetMouseButton(MOUSE_BUTTON_RIGHT, 0);
        break;
    default:
    }

    gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

