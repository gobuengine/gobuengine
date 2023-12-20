#include "gapp_extrasprites.h"

struct _GbAppExtraSprites
{
    GtkWidget parent;
};

extern GAPP* EditorCore;

G_DEFINE_TYPE_WITH_PRIVATE(GbAppExtraSprites, gbapp_extrasprites, GTK_TYPE_BOX);

static void gbapp_extrasprites_class_init(GbAppExtraSpritesClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
}

static void gbapp_extrasprites_init(GbAppExtraSprites* self)
{
    GbAppExtraSpritesPrivate* priv = gbapp_extrasprites_get_instance_private(self);
}

/**
 * Extra de un AtlasSprite varios Sprite.
 *
 * @return Un nuevo widget que ExtraSprites.
 */
GtkWidget* gbapp_extrasprites_new(void)
{
    return g_object_new(GBAPP_TYPE_EXTRASPRITES, "orientation", GTK_ORIENTATION_VERTICAL, NULL);
}

