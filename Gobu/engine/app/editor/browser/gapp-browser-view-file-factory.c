#include "gapp-browser-view-file-factory.h"
#include "gapp-browser-view-file-selected.h"

/**
 * Obtiene el icono de un archivo en el explorador de mundos del editor en Gobu.
 *
 * @param image      El widget de imagen en el que se muestra el icono del archivo.
 * @param info_file  La información del archivo para la obtención del icono.
 */
static void gapp_browser_fn_get_icon_file(GtkWidget *image, GFileInfo *info_file)
{
    GIcon *icon;
    const char *ext_file = g_file_info_get_name(info_file);
    GFile *file = G_FILE(g_file_info_get_attribute_object(info_file, "standard::file"));

    if (gb_fs_is_extension(ext_file, ".png") || gb_fs_is_extension(ext_file, ".jpg"))
    {
        gtk_image_set_from_file(image, g_file_get_path(file));
        return;
    }
    else if (gb_fs_is_extension(ext_file, ".sprite"))
    {
        // binn* json = binn_serialize_from_file(g_file_get_path(file));
        // gb_sprite_t sprite = gb_sprite_deserialize(json);
        // binn_free(json);

        // GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(gb_path_join_relative_content(sprite.resource), NULL);
        // pixbuf = gdk_pixbuf_new_subpixbuf(pixbuf, sprite.src.x, sprite.src.y, sprite.dst.width, sprite.dst.height);
        // gtk_image_set_from_pixbuf(image, pixbuf);
        // g_object_unref(pixbuf);
        // return;
    }
    else if (gb_fs_is_extension(ext_file, ".gbscript"))
    {
        icon = project_editor_get_icon(GOBU_RESOURCE_ICON_COMPONENT);
    }
    else if (gb_fs_is_extension(ext_file, ".level"))
    {
        icon = project_editor_get_icon(GOBU_RESOURCE_ICON_LEVEL);
    }
    else if (gb_fs_is_extension(ext_file, ".prefab"))
    {
        icon = project_editor_get_icon(GOBU_RESOURCE_ICON_ENTITY);
    }
    else if (gb_fs_is_extension(ext_file, ".asprites"))
    {
        icon = project_editor_get_icon(GOBU_RESOURCE_ICON_ASPRITE);
    }
    else
    {
        icon = g_file_info_get_icon(info_file);
        gtk_image_set_icon_size(GTK_IMAGE(image), GTK_ICON_SIZE_LARGE);
    }

    gtk_image_set_from_gicon(GTK_IMAGE(image), icon);
}

/**
 * Prepara la fuente de arrastre en el explorador de mundos del editor en Gobu.
 *
 * @param source      La fuente de arrastre que se prepara.
 * @param x           La posición X del puntero en el momento del arrastre.
 * @param y           La posición Y del puntero en el momento del arrastre.
 * @param list_item   El elemento de lista asociado al arrastre.
 *
 * @return Un proveedor de contenido Gdk que representa los datos de arrastre preparados.
 */
static GdkContentProvider *factory_signal_factory_drag_prepare(GtkDragSource *source, gdouble x, gdouble y, GtkListItem *list_item)
{
    GtkWidget *box = gtk_list_item_get_child(list_item);

    GappBrowser *browser = g_object_get_data(box, "BrowserContent");
    GappBrowserPrivate *private = g_type_instance_get_private(browser, GAPP_TYPE_BROWSER);

    guint pos_selected = GPOINTER_TO_UINT(g_object_get_data(box, "position_id"));

    GListStore *list_file = gapp_browser_view_file_get_file_seleted(private);
    guint items_n = g_list_model_get_n_items(G_LIST_MODEL(list_file));

    // 1 - Si no tenemos ningun item seleccionado, seleccionamos el item que se esta arrastrando.
    // 1.1 - Cargamos la lista de nuevo para que se actualice la seleccion.
    // 2 - Si el item que se esta arrastrando esta seleccionado, no hacemos nada.
    // Reglas:
    // items_n == 0 -> no hay ningun item seleccionado.
    // items_n == 1 -> hay un item seleccionado pero no es el que se esta arrastrando.
    if (items_n <= 1 && !gtk_selection_model_is_selected(GTK_SELECTION_MODEL(private->selection), pos_selected))
    {
        gtk_selection_model_select_item(GTK_SELECTION_MODEL(private->selection), pos_selected, TRUE);
        list_file = gapp_browser_view_file_get_file_seleted(private);
    }

    return gdk_content_provider_new_union((GdkContentProvider *[1]){
                                              gdk_content_provider_new_typed(G_TYPE_LIST_STORE, list_file),
                                          },
                                          1);
}

/**
 * Inicia la fuente de arrastre en el explorador de mundos del editor en Gobu.
 *
 * @param source      La fuente de arrastre que se inicia.
 * @param drag        El objeto de arrastre asociado.
 * @param list_item   El elemento de lista que se arrastra.
 *
 * @return Un proveedor de contenido Gdk que representa los datos de arrastre iniciales.
 */
static void factory_signal_factory_drag_begin(GtkDragSource *source, GdkDrag *drag, GtkListItem *list_item)
{
    GtkWidget *box = gtk_list_item_get_child(list_item);

    GtkWidget *paintable = gtk_widget_paintable_new(gtk_widget_get_first_child(box));
    gtk_drag_source_set_icon(source, paintable, 0, 0);

    g_object_unref(paintable);
}

/**
 * Configura la vista de archivo en el explorador de mundos del editor en Gobu.
 *
 * @param factory     La fábrica de elementos de lista en la que se realiza la configuración.
 * @param list_item   El elemento de lista al que se aplica la configuración de la vista de archivo.
 * @param user_data   Datos de usuario opcionales para la configuración de la vista de archivo.
 */
void gapp_browser_view_file_signal_setup_factory(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    GtkWidget *box, *imagen, *label, *entry;
    GtkExpression *expression;
    GtkDragSource *source;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_list_item_set_child(list_item, box);
    gtk_widget_set_size_request(box, 84, 84);

    imagen = gtk_image_new();
    gtk_widget_set_vexpand(imagen, TRUE);
    gtk_widget_set_hexpand(imagen, TRUE);
    gtk_image_set_icon_size(imagen, GTK_ICON_SIZE_LARGE);
    gtk_box_append(GTK_BOX(box), imagen);

    label = gtk_label_new(NULL);
    gtk_label_set_wrap_mode(label, PANGO_WRAP_CHAR);
    gtk_label_set_justify(label, GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(box), label);

    g_object_set_data(box, "BrowserContent", user_data);

    source = gtk_drag_source_new();
    gtk_drag_source_set_actions(source, GDK_ACTION_COPY);
    g_signal_connect(source, "prepare", G_CALLBACK(factory_signal_factory_drag_prepare), list_item);
    g_signal_connect(source, "drag-begin", G_CALLBACK(factory_signal_factory_drag_begin), list_item);
    gtk_widget_add_controller(box, GTK_EVENT_CONTROLLER(source));
}

/**
 * Vincula la vista de archivo en el explorador de mundos del editor en Gobu.
 *
 * @param factory     La fábrica de elementos de lista en la que se realiza la vinculación.
 * @param list_item   El elemento de lista al que se vincula la vista de archivo.
 * @param user_data   Datos de usuario opcionales para la vinculación de la vista de archivo.
 */
void gapp_browser_view_file_signal_bind_factory(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data)
{
    GtkWidget *box, *label_name, *image;

    box = gtk_list_item_get_child(list_item);
    gb_return_if_fail(box);

    GFileInfo *info_file = gtk_list_item_get_item(list_item);
    gint id = gtk_list_item_get_position(list_item);

    image = gtk_widget_get_first_child(box);
    label_name = gtk_widget_get_next_sibling(image);

    const char *name = g_file_info_get_name(info_file);
    gtk_label_set_label(GTK_LABEL(label_name), gb_fs_get_name(name, true));
    gtk_widget_set_tooltip_text(box, name);

    gapp_browser_fn_get_icon_file(image, info_file);
    // TODO: Buscar una mejor solucion
    g_object_set_data(G_OBJECT(image), "position_id", GINT_TO_POINTER(id));
    g_object_set_data(G_OBJECT(label_name), "position_id", GINT_TO_POINTER(id));
    g_object_set_data(G_OBJECT(box), "position_id", GINT_TO_POINTER(id));
}
