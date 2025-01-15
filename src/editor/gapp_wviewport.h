// gapp_widget_viewport.h
#ifndef GAPP_WIDGET_VIEWPORT_H
#define GAPP_WIDGET_VIEWPORT_H

#include "config.h"
#include "gobu/gobu.h"

G_BEGIN_DECLS

#define GAPP_WIDGET_TYPE_VIEWPORT (gapp_widget_viewport_get_type())
G_DECLARE_FINAL_TYPE(GappViewport, gapp_widget_viewport, GAPP_WIDGET, VIEWPORT, GtkGLArea)

GappViewport *gapp_widget_viewport_new(void);

G_END_DECLS

#endif // GAPP_WIDGET_VIEWPORT_H
