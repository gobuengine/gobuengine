// gapp_project_manager.h
#ifndef GOBU_PROJECT_MANAGER_H
#define GOBU_PROJECT_MANAGER_H

#include "config.h"

G_BEGIN_DECLS

#define GOBU_TYPE_PROJECT_MANAGER (gobu_project_manager_get_type())
G_DECLARE_FINAL_TYPE(GobuProjectManager, gobu_project_manager, GOBU, PROJECT_MANAGER, GtkBox)

/**
 * gobu_project_manager_new:
 * @app: La instancia de GtkApplication a la que se asociará el GobuProjectManager.
 *
 * Crea una nueva instancia de GobuProjectManager.
 *
 * Returns: (transfer full): Una nueva instancia de #GobuProjectManager.
 *   Usa g_object_unref() cuando ya no la necesites.
 */
GobuProjectManager *gobu_project_manager_new(void);

/**
 * gobu_project_manager_show:
 * @self: La instancia de GobuProjectManager a mostrar.
 *
 * Muestra la ventana del GobuProjectManager, trayéndola al frente
 * si ya estaba visible.
 *
 * Esta función es parte de la API pública de GobuProjectManager.
 */
void gobu_project_manager_show(GobuProjectManager *self);

G_END_DECLS

#endif // GOBU_PROJECT_MANAGER_H
