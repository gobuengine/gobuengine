// gapp_level_editor.h
#ifndef GOBU_LEVEL_EDITOR_H
#define GOBU_LEVEL_EDITOR_H

#include "config.h"

G_BEGIN_DECLS

#define GOBU_TYPE_LEVEL_EDITOR (gobu_level_editor_get_type())
G_DECLARE_FINAL_TYPE(GobuLevelEditor, gobu_level_editor, GOBU, LEVEL_EDITOR, GtkBox)

/**
 * gobu_level_editor_new:
 * @app: La instancia de GtkApplication a la que se asociará el GobuLevelEditor.
 *
 * Crea una nueva instancia de GobuLevelEditor.
 *
 * Returns: (transfer full): Una nueva instancia de #GobuLevelEditor.
 *   Usa g_object_unref() cuando ya no la necesites.
 */
GobuLevelEditor *gobu_level_editor_new(void);

GtkWidget *gobu_level_editor_get_outliner(GobuLevelEditor *self);

GtkWidget *gobu_level_editor_get_viewport(GobuLevelEditor *self);

GtkWidget *gobu_level_editor_get_inspector(GobuLevelEditor *self);

G_END_DECLS

#endif // GOBU_LEVEL_EDITOR_H
