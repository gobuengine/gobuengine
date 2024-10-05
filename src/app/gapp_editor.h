// gobu_editor.h
#ifndef GOBU_EDITOR_MAIN_H
#define GOBU_EDITOR_MAIN_H

#include "config.h"

G_BEGIN_DECLS

#define GOBU_TYPE_EDITOR_MAIN (gobu_editor_main_get_type())
G_DECLARE_FINAL_TYPE(GobuEditorMain, gobu_editor_main, GOBU, EDITOR_MAIN, GtkApplicationWindow)

/**
 * gobu_editor_main_new:
 * @app: La instancia de GtkApplication a la que se asociará el GobuEditorMain.
 *
 * Crea una nueva instancia de GobuEditorMain.
 *
 * Returns: (transfer full): Una nueva instancia de #GobuEditorMain.
 *   Usa g_object_unref() cuando ya no la necesites.
 */
GobuEditorMain *gobu_editor_main_new(GtkApplication *app);

/**
 * gobu_editor_main_show:
 * @self: La instancia de GobuEditorMain a mostrar.
 *
 * Muestra la ventana del GobuEditorMain, trayéndola al frente
 * si ya estaba visible.
 *
 * Esta función es parte de la API pública de GobuEditorMain.
 */
void gobu_editor_main_show(GobuEditorMain *self);

/**
 * Inicializa un proyecto en el editor.
 *
 * @param self Puntero al objeto GobuEditorMain.
 * @param path Ruta del proyecto a inicializar.
 */
void gobu_editor_main_init_project(GobuEditorMain *self, const gchar *path);

/**
 * Añade un nuevo panel al lado derecho del editor.
 *
 * @param self Puntero al objeto GobuEditorMain.
 * @param title Título del nuevo panel.
 * @param module Widget a añadir como contenido del panel.
 * @param is_button_close Indica si se debe mostrar un botón de cierre en la pestaña.
 */
void gapp_editor_append_right_panel(GobuEditorMain *self, const gchar *title, GtkWidget *module, gboolean is_button_close);

/**
 * Obtiene el widget de configuración del editor.
 *
 * Esta función devuelve el widget de configuración asociado con el editor principal.
 *
 * @param self El objeto GobuEditorMain del cual se obtendrá la configuración.
 * @return El widget de configuración (GtkWidget*), o NULL si ocurre un error.
 */
GtkWidget *gapp_editor_get_config(GobuEditorMain *self);

G_END_DECLS

#endif // GOBU_EDITOR_MAIN_H
