#include "gobu_type_factory_item_view_column.h"

struct _GobuFactoryItemViewColumnClass
{
    GObjectClass parent_class;
};

G_DEFINE_TYPE(GobuFactoryItemViewColumn, gobu_factory_item_view_column, G_TYPE_OBJECT)

static void gobu_factory_item_view_column_init(GobuFactoryItemViewColumn *item) {}
static void gobu_factory_item_view_column_class_init(GobuFactoryItemViewColumnClass *item) {}

/**
 * Crea una nueva columna de vista de ítem Gobu.
 *
 * Esta función crea y devuelve una nueva columna de vista de ítem Gobu
 * con el nombre especificado y su visibilidad inicial.
 *
 * @param name El nombre de la nueva columna.
 * @param visible Indica si la columna es visible inicialmente (TRUE o FALSE).
 *
 * @return Un puntero a la nueva columna de vista de ítem Gobu creada.
 */
GobuFactoryItemViewColumn *gobu_factory_item_view_column_new(const char *name, gboolean visible)
{
    GobuFactoryItemViewColumn *item;
    item = g_object_new(GOBU_FACTORY_ITEM_TYPE_VIEW_COLUMN, NULL);
    item->name = name;
    item->visible = visible;
    return item;
}

/**
 * Obtiene el nombre de la columna de vista de ítem Gobu.
 *
 * Esta función devuelve el nombre de la columna de vista de ítem Gobu
 * asociada al objeto de columna proporcionado.
 *
 * @param item El objeto de columna de vista de ítem Gobu.
 *
 * @return Un puntero al nombre de la columna.
 */
const gchar *gobu_factory_item_view_column_get_name(GobuFactoryItemViewColumn *item)
{
    return item->name;
}

/**
 * Obtiene la visibilidad de la columna de vista de ítem Gobu.
 *
 * Esta función devuelve un valor booleano que indica si la columna de vista de ítem Gobu
 * asociada al objeto de columna proporcionado está visible.
 *
 * @param item El objeto de columna de vista de ítem Gobu.
 *
 * @return TRUE si la columna es visible, FALSE si no lo es.
 */
gboolean *gobu_factory_item_view_column_get_visible(GobuFactoryItemViewColumn *item)
{
    return item->visible;
}
