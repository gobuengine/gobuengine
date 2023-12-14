#include "gbapp_asheets.h"

struct _GbAppAsheets
{
    GtkWidget parent;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GbAppAsheets, gbapp_asheets, GTK_TYPE_BOX);

static void gbapp_asheets_class_init(GbAppAsheetsClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gbapp_asheets_init(GbAppAsheets* self)
{
    GbAppAsheetsPrivate* priv = gbapp_asheets_get_instance_private(self);
}

/**
 * Crea un nuevo Animation Sprite Sheets.
 *
 * @return Un nuevo widget que Animation Sprite Sheets.
 */
GtkWidget* gbapp_asheets_new(void)
{
    return g_object_new(GBAPP_TYPE_ASHEETS, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}

