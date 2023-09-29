#include "WorlrdViewport.h"
#include "Widget/GobuEmbedStage.h"
#include "GobuWidgets.h"

static void on_update(GtkWidget *viewport)
{
    // ermine_scene__update(scene_main);
}

float angle = 0;
static void on_draw(GtkWidget *viewport)
{
    // ermine_scene__draw(scene_main);
    GobuShapeDrawFilledRect((GobuRectangle){100, 100, 50, 50}, (GobuVec2){1.0f, 1.0f}, (GobuVec2){0.5f, 0.5f}, angle, GobuColorRGBToFloat(255, 0, 0));
    angle += 0.05f;
}

static void on_start(GtkWidget *viewport)
{
    // scene_main = ermine_scene_new();
}

static gboolean drag_drop(GtkDropTarget *target, const GValue *value, double x, double y)
{
    printf("Drop target: %f, %f\n", x, y);
    return TRUE;
}

GtkWidget *GobuWorldViewportNew(void)
{
    GtkWidget *box, *toolbar, *item;
    GtkDropTarget *target;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    {
        // toolbar = GobuWidgetToolbarNew();
        // gtk_box_append(box, toolbar);
        // {
        //     item = GobuWidgetButtonIconLabelNew("media-removable-symbolic", "Save");
        //     gtk_box_append(GTK_BOX(toolbar), item);
        // }

        GtkWidget *viewport = gtk_gobu_embed_stage_new();
        // ermine_embed_set_color(viewport, RAYWHITE);
        g_signal_connect(viewport, "stage-start", on_start, NULL);
        g_signal_connect(viewport, "stage-draw", on_draw, NULL);
        g_signal_connect(viewport, "stage-update", on_update, NULL);
        gtk_box_append(box, viewport);

        target = gtk_drop_target_new(G_TYPE_STRING, GDK_ACTION_MOVE);
        g_signal_connect(target, "drop", G_CALLBACK(drag_drop), viewport);
        gtk_widget_add_controller(viewport, GTK_EVENT_CONTROLLER(target));
    }

    return box;
}
