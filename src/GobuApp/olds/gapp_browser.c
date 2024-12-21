#include "gapp_browser.h"
#include "gapp_browser_filete.h"
#include "gapp_common.h"
#include "gapp_widget.h"
#include "gapp_scene.h"
#include "gapp_script.h"
#include "gapp_components.h"
#include "gapp.h"

struct _GappBrowser
{
    GtkWidget parent_instance;
    GtkMultiSelection *selection;
    GtkDirectoryList *directory;
};

typedef struct
{
    const gchar *iconName;
    const gchar *name;
    GCallback callback;
} menuItemAddFileNew;

// MARK:PRIVATE
static void gappBrowserConfigureInterface(GappBrowser *self);

static GtkWidget *browserToolbarSetupInterfacePopoverMenu(GtkWidget *parent, GappBrowser *self);
static void onBrowserViewFileBindFactory(GtkListItemFactory *factory, GtkListItem *list_item, GappBrowser *browser);
static void onBrowserViewFileSetupFactory(GtkListItemFactory *factory, GtkListItem *list_item, gpointer data);
static void onBrowserFileActivated(GtkListView *self, guint position, GappBrowser *browser);
static void onBrowserToolbarRemoveFileClicked(GtkWidget *widget, GappBrowser *self);
static void onBrowserRemoveFileInDialog(GtkAlertDialog *dialog, GAsyncResult *res, GappBrowser *self);
static void onBrowserCreateScriptInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog);
static void onBrowserCreateSceneInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog);
static void onBrowserCreatePrefabInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog);
static void onBrowserCreateFolderInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog);
static gboolean onBrowserFileDrop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer data);
static GdkContentProvider *onBrowserPrepareFileDrag(GtkDragSource *source, double x, double y, GtkListItem *list_item);
static int onBrowserCustomFileTypeComparator(GFileInfo *a, GFileInfo *b, gpointer data);
static GListModel *onBrowserCreateChildModelForTreeRow(gpointer file_info, gpointer data);

static void browserShowPopover(GtkWidget *widget, GtkWidget *popover);
static void browserHidePopover(GtkWidget *popover);
static gboolean browserIsContentFolder(GappBrowser *browser, const gchar *path);
static gchar *browserGetSelectedPathFromDialog(GappWidgetDialogEntry *dialog, const gchar *ext);
static GFile *gapp_browser_create_file_from_file_info(GFileInfo *info);
static GFileInfo *gapp_browser_get_selected_file(GtkSelectionModel *selection);
static GListModel *browserCreateGListFromGFile(GFile *file);
static GListModel *browserGetGListModelFromDirectoryList(GtkDirectoryList *list);
static void browserSetMonitoredDirectory(GtkDirectoryList *directory);

// MARK:BASE CLASS
G_DEFINE_TYPE(GappBrowser, gapp_browser, GTK_TYPE_BOX)

static void object_class_dispose(GObject *object)
{
    GappBrowser *self = GAPP_BROWSER(object);
    g_clear_pointer(&self, gtk_widget_unparent);
    G_OBJECT_CLASS(gapp_browser_parent_class)->dispose(object);
}

static void gapp_browser_class_init(GappBrowserClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = object_class_dispose;
}

static void gapp_browser_init(GappBrowser *self)
{
    g_return_if_fail(GAPP_IS_BROWSER(self));
    gappBrowserConfigureInterface(self);
}

GappBrowser *gapp_browser_new(void)
{
    return g_object_new(GAPP_TYPE_BROWSER,
                        "orientation", GTK_ORIENTATION_VERTICAL,
                        NULL);
}

// MARK: API PRIVATE

static void browserSetMonitoredDirectory(GtkDirectoryList *directory)
{
    g_return_if_fail(GTK_IS_DIRECTORY_LIST(directory));

    gtk_directory_list_set_monitored(directory, TRUE);
    gtk_directory_list_set_io_priority(directory, G_PRIORITY_DEFAULT_IDLE);
}

static GListModel *browserGetGListModelFromDirectoryList(GtkDirectoryList *list)
{
    g_return_val_if_fail(list != NULL, NULL);
    GtkSorter *sorter = gtk_custom_sorter_new(onBrowserCustomFileTypeComparator, NULL, NULL);
    GListModel *sorted_model = G_LIST_MODEL(gtk_sort_list_model_new(G_LIST_MODEL(list), sorter));
    return sorted_model;
}

static GListModel *browserCreateGListFromGFile(GFile *file)
{
    GtkDirectoryList *list = gtk_directory_list_new("standard::*", file);
    browserSetMonitoredDirectory(list);
    return browserGetGListModelFromDirectoryList(list);
}

static GFileInfo *gapp_browser_get_selected_file(GtkSelectionModel *selection)
{
    guint position = 0;
    GListModel *model = gtk_multi_selection_get_model(GTK_MULTI_SELECTION(selection));
    guint n = g_list_model_get_n_items(G_LIST_MODEL(model));

    for (guint i = 0; i < n; i++)
    {
        if (gtk_selection_model_is_selected(GTK_SELECTION_MODEL(selection), i))
        {
            position = i;
            break;
        }
    }

    GtkTreeListRow *row = g_list_model_get_item(G_LIST_MODEL(model), position);
    GFileInfo *info = G_FILE_INFO(gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row)));

    return info;
}

static GFile *gapp_browser_create_file_from_file_info(GFileInfo *info)
{
    g_return_val_if_fail(G_IS_FILE_INFO(info), NULL);

    GFile *file = G_FILE(g_file_info_get_attribute_object(info, "standard::file"));
    return file;
}

static gchar *browserGetSelectedPathFromDialog(GappWidgetDialogEntry *dialog, const gchar *ext)
{
    g_return_val_if_fail(dialog != NULL && dialog->entry != NULL && dialog->data != NULL, NULL);

    GtkSelectionModel *selection;
    GFileInfo *info;
    GFile *file;

    const char *filename = gapp_widget_entry_get_text(GTK_ENTRY(dialog->entry));

    if (filename == NULL || strlen(filename) == 0)
    {
        g_warning("browserGetSelectedPathFromDialog: Folder name is empty");
        return NULL;
    }

    selection = GTK_SELECTION_MODEL(dialog->data);
    if (!GTK_IS_SELECTION_MODEL(selection))
    {
        g_warning("browserGetSelectedPathFromDialog: Invalid selection model");
        return NULL;
    }

    info = gapp_browser_get_selected_file(selection);
    if (!G_IS_FILE_INFO(info))
    {
        g_warning("browserGetSelectedPathFromDialog: Failed to get file info");
        return NULL;
    }

    file = gapp_browser_create_file_from_file_info(info);
    if (!G_IS_FILE(file))
    {
        g_warning("browserGetSelectedPathFromDialog: Failed to get file");
        g_object_unref(info);
        return NULL;
    }

    char *pathfile = g_file_get_path(file);
    if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
        pathfile = pathDirname(pathfile);

    gchar *pathname = pathJoin(pathfile, g_strdup_printf("%s%s", filename, ext), NULL);

    g_free(pathfile);
    // g_object_unref(file);
    // g_object_unref(info);

    gapp_widget_dialog_entry_text_destroy(dialog);

    return pathname;
}

static gboolean browserIsContentFolder(GappBrowser *browser, const gchar *path)
{
    gchar *content_path = browserGetContentPath(browser);
    gboolean result = g_strcmp0(path, content_path) == 0;
    g_free(content_path);
    return result;
}

static void browserSetIconForFileInfo(GtkImage *icon, GFileInfo *fileInfo)
{
    const char *ext_file = g_file_info_get_name(fileInfo);

    if (fsIsExtension(ext_file, GAPP_BROWSER_FILE_IMAGE) || fsIsExtension(ext_file, GAPP_BROWSER_FILE_IMAGE2))
        gtk_image_set_from_file(icon, g_file_get_path(gapp_browser_create_file_from_file_info(fileInfo)));
    else if (fsIsExtension(ext_file, GAPP_BROWSER_FILE_PREFAB))
        gtk_image_set_from_paintable(icon, gapp_get_resource_icon(GAPP_RESOURCE_ICON_PREFAB));
    else if (fsIsExtension(ext_file, GAPP_BROWSER_FILE_SCENE))
        gtk_image_set_from_paintable(icon, gapp_get_resource_icon(GAPP_RESOURCE_ICON_SCENE));
    else if (fsIsExtension(ext_file, GAPP_BROWSER_FILE_SCRIPT))
        gtk_image_set_from_paintable(icon, gapp_get_resource_icon(GAPP_RESOURCE_ICON_SCRIPT));
    else if (fsIsExtension(ext_file, GAPP_BROWSER_FILE_ANIMATION))
        gtk_image_set_from_paintable(icon, gapp_get_resource_icon(GAPP_RESOURCE_ICON_ANIM2D));
    else if (fsIsExtension(ext_file, GAPP_BROWSER_FILE_COMPONENT))
        gtk_image_set_from_paintable(icon, gapp_get_resource_icon(GAPP_RESOURCE_ICON_COMPS));
    else
        gtk_image_set_from_gicon(GTK_IMAGE(icon), g_file_info_get_icon(fileInfo));
}

// MARK: SIGNAL

static GListModel *onBrowserCreateChildModelForTreeRow(gpointer file_info, gpointer data)
{
    g_return_val_if_fail(G_FILE_INFO(file_info), NULL);
    GFileType type_a = g_file_info_get_file_type(G_FILE_INFO(file_info));

    if (type_a != G_FILE_TYPE_DIRECTORY)
        return NULL;

    GFile *file = gapp_browser_create_file_from_file_info(G_FILE_INFO(file_info));
    g_return_val_if_fail(file != NULL, NULL);

    return browserCreateGListFromGFile(file);
}

static int onBrowserCustomFileTypeComparator(GFileInfo *a, GFileInfo *b, gpointer data)
{
    GFileType type_a = g_file_info_get_file_type(a);
    GFileType type_b = g_file_info_get_file_type(b);

    if (type_a == G_FILE_TYPE_DIRECTORY && type_b != G_FILE_TYPE_DIRECTORY)
        return -1;
    else if (type_a != G_FILE_TYPE_DIRECTORY && type_b == G_FILE_TYPE_DIRECTORY)
        return 1;

    return 0;
}

static GdkContentProvider *onBrowserPrepareFileDrag(GtkDragSource *source, double x, double y, GtkListItem *list_item)
{
    GtkTreeListRow *row = gtk_list_item_get_item(list_item);
    GFileInfo *fileInfo = gtk_tree_list_row_get_item(row);

    return gdk_content_provider_new_union((GdkContentProvider *[1]){
                                              gdk_content_provider_new_typed(G_TYPE_FILE_INFO, fileInfo),
                                          },
                                          1);
}

static gboolean onBrowserFileDrop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer data)
{
    if (G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
    {
        GError *error = NULL;
        char *pathfile = NULL;

        // drop en el content browser
        if (GAPP_IS_BROWSER(data))
        {
            GappBrowser *browser = GAPP_BROWSER(data);
            pathfile = browserGetContentPath(browser);
        }
        else
        {
            GtkTreeListRow *row = gtk_list_item_get_item(GTK_LIST_ITEM(data));
            GFileInfo *info = gtk_tree_list_row_get_item(row);
            pathfile = g_file_get_path(gapp_browser_create_file_from_file_info(info));
        }

        if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
            pathfile = pathDirname(pathfile);

        GList *files = g_value_get_boxed(value);
        for (GList *l = files; l != NULL; l = l->next)
        {
            GFile *file_src = G_FILE(l->data);
            GFile *file_dst = g_file_new_for_path(pathJoin(pathfile, g_file_get_basename(file_src), NULL));
            if (!fsCopy(file_src, file_dst, &error))
            {
                g_warning("onBrowserFileDrop: Failed to copy file: %s", error->message);
                g_clear_error(&error);
            }
        }

        g_free(pathfile);

        return TRUE;
    }
    else if (G_VALUE_HOLDS(value, G_TYPE_FILE_INFO))
    {
        GError *error = NULL;

        // Folder destino
        GtkTreeListRow *row = gtk_list_item_get_item(GTK_LIST_ITEM(data));
        GFileInfo *info = gtk_tree_list_row_get_item(row);

        char *pathfile = g_file_get_path(gapp_browser_create_file_from_file_info(info));
        if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
            pathfile = pathDirname(pathfile);

        // File origen
        GFileInfo *file = g_value_get_object(value);
        GFile *file_src = gapp_browser_create_file_from_file_info(file);
        GFile *file_dst = g_file_new_for_path(pathJoin(pathfile, g_file_get_basename(file_src), NULL));
        if (!g_file_move(file_src, file_dst, G_FILE_COPY_NONE, NULL, NULL, NULL, &error))
        {
            g_warning("onBrowserFileDrop: Failed to move file: %s", error->message);
            g_clear_error(&error);
        }

        return TRUE;
    }

    return FALSE;
}

static void onBrowserCreateFolderInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *foldername = browserGetSelectedPathFromDialog(dialog, "");
    if (foldername != NULL)
        g_mkdir_with_parents(foldername, 0755);
}

static void onBrowserCreateSceneInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *filename = browserGetSelectedPathFromDialog(dialog, GAPP_BROWSER_FILE_SCENE);
    if (filename != NULL)
        g_file_set_contents(filename, TEMPLATE_SCENE_STR, -1, NULL);
}

static void onBrowserCreatePrefabInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *filename = browserGetSelectedPathFromDialog(dialog, GAPP_BROWSER_FILE_PREFAB);
    if (filename != NULL)
        g_file_set_contents(filename, TEMPLATE_PREFAB_STR, -1, NULL);
}

static void onBrowserCreateScriptInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *filename = browserGetSelectedPathFromDialog(dialog, GAPP_BROWSER_FILE_COMPONENT);
    if (filename != NULL)
        g_file_set_contents(filename, "", -1, NULL);
}

static void onBrowserCreateAnimateSpriteInDialog(GtkWidget *button, GappWidgetDialogEntry *dialog)
{
    const char *filename = browserGetSelectedPathFromDialog(dialog, GAPP_BROWSER_FILE_ANIMATION);
    if (filename != NULL)
        g_file_set_contents(filename, "", -1, NULL);
}

static void onBrowserToolbarCreateDialog(GtkWidget *widget, GappBrowser *self)
{
    const gchar *title = g_object_get_data(G_OBJECT(widget), "title");
    GCallback callback = G_CALLBACK(g_object_get_data(G_OBJECT(widget), "callback"));

    GappWidgetDialogEntry *dialog = gapp_widget_dialog_new_entry_text(gtk_widget_get_root(self), title, "Name");
    dialog->data = self->selection;
    g_signal_connect(dialog->button_ok, "clicked", callback, dialog);

    browserHidePopover(GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "popover")));
}

static void onBrowserRemoveFileInDialog(GtkAlertDialog *dialog, GAsyncResult *res, GappBrowser *self)
{
    int response = gtk_alert_dialog_choose_finish(dialog, res, NULL);

    if (response == 1) // Aceptar
    {
        GListModel *model = gtk_multi_selection_get_model(GTK_MULTI_SELECTION(self->selection));
        guint n = g_list_model_get_n_items(G_LIST_MODEL(model));

        for (guint i = 0; i < n; i++)
        {
            // Verifica si el elemento actual está seleccionado
            if (!gtk_selection_model_is_selected(GTK_SELECTION_MODEL(self->selection), i))
                continue;

            g_autoptr(GtkTreeListRow) row = g_list_model_get_item(G_LIST_MODEL(model), i);
            GFileInfo *info = G_FILE_INFO(gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row)));
            GFile *file = gapp_browser_create_file_from_file_info(info);

            // Verifica que no sea un directorio y que no sea la carpeta "Content"
            if (!browserIsContentFolder(self, g_file_get_path(file)))
                g_file_trash(gapp_browser_create_file_from_file_info(info), NULL, NULL);
        }
    }
}

static void onBrowserToolbarRemoveFileClicked(GtkWidget *widget, GappBrowser *self)
{
    gapp_widget_dialog_new_confirm_action(gtk_widget_get_root(self),
                                          "Delete Selected Assets",
                                          "Are you sure you want to delete the selected assets?",
                                          onBrowserRemoveFileInDialog, self);
}

// static void onBrowserToolbarOpenComponentsClicked(GtkWidget *widget, GappBrowser *self)
// {
//     GtkWidget *widget_module = gapp_component_new();
//     gapp_right_panel_append(GAPP_RESOURCE_ICON_COMPS, "Components", widget_module, TRUE);
// }

// -- -- --
// static void gapp_browser_s_file_popup(GtkGesture *gesture, int n_press, double x, double y, GappBrowser *self)
// {
// GtkWidget *child;

// GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
// child = gtk_widget_pick(widget, x, y, GTK_PICK_DEFAULT);

// if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY)
// {
//     gtk_popover_popup(GTK_POPOVER(popover));
// }
// }

static void onBrowserFileActivated(GtkListView *self, guint position, GappBrowser *browser)
{
    GappResourceIcon icon = GAPP_RESOURCE_ICON_SCENE;
    GtkWidget *widget_module = NULL;
    GListModel *model = gtk_multi_selection_get_model(GTK_MULTI_SELECTION(browser->selection));

    g_autoptr(GtkTreeListRow) row = g_list_model_get_item(G_LIST_MODEL(model), position);
    GFileInfo *info = G_FILE_INFO(gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row)));
    GFile *file = gapp_browser_create_file_from_file_info(info);

    const char *filename = g_file_info_get_name(info);
    char *name = fsGetName(filename, TRUE);
    char *pathFile = g_file_get_path(file);

    // if (fsIsExtension(filename, GAPP_BROWSER_FILE_SCRIPT))
    // {
    //     widget_module = gapp_script_new(pathFile);
    //     icon = GAPP_RESOURCE_ICON_SCRIPT;
    // }
    // else 
    if (fsIsExtension(filename, GAPP_BROWSER_FILE_COMPONENT))
    {
        widget_module = gapp_component_new();
        gapp_component_load_file(widget_module, pathFile);
        icon = GAPP_RESOURCE_ICON_COMPS;
    }
    else if (fsIsExtension(filename, GAPP_BROWSER_FILE_SCENE))
    {
        widget_module = gapp_scene_new(pathFile);
    }
    else if (fsIsExtension(filename, GAPP_BROWSER_FILE_PREFAB))
    {
        widget_module = gapp_scene_new(pathFile);
        icon = GAPP_RESOURCE_ICON_PREFAB;
    }

    if (widget_module != NULL)
        gapp_right_panel_append(icon, name, widget_module, TRUE);

    g_free(pathFile);
    g_free(name);
}

static void onBrowserViewFileSetupFactory(GtkListItemFactory *factory, GtkListItem *list_item, gpointer data)
{
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkWidget *expander = gtk_tree_expander_new();
    gtk_list_item_set_child(list_item, expander);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(box, 6);
    gtk_widget_set_margin_end(box, 6);
    gtk_widget_set_margin_top(box, 6);
    gtk_widget_set_margin_bottom(box, 6);
    gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), box);

    GtkWidget *icon = gtk_image_new();
    gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_INHERIT);
    gtk_box_append(GTK_BOX(box), icon);

    GtkWidget *label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_set_margin_start(label, 5);
    gtk_widget_set_margin_end(label, 5);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(box), label);

    GtkDropTarget *drop = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
    g_signal_connect(drop, "drop", G_CALLBACK(onBrowserFileDrop), list_item);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop));

    GtkDropTarget *drop_move = gtk_drop_target_new(G_TYPE_FILE_INFO, GDK_ACTION_MOVE);
    g_signal_connect(drop_move, "drop", G_CALLBACK(onBrowserFileDrop), list_item);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drop_move));

    GtkDragSource *drag = gtk_drag_source_new();
    gtk_drag_source_set_actions(drag, GDK_ACTION_MOVE);
    g_signal_connect(drag, "prepare", G_CALLBACK(onBrowserPrepareFileDrag), list_item);
    gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(drag));

    // GtkGesture *gesture = gtk_gesture_click_new();
    // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    // g_signal_connect(gesture, "pressed", G_CALLBACK(gapp_browser_s_file_popup), list_item);
    // gtk_widget_add_controller(expander, GTK_EVENT_CONTROLLER(gesture));
}

static void onBrowserViewFileBindFactory(GtkListItemFactory *factory, GtkListItem *list_item, GappBrowser *browser)
{
    g_return_if_fail(GTK_IS_LIST_ITEM(list_item));

    GtkTreeListRow *row = gtk_list_item_get_item(list_item);
    g_return_if_fail(GTK_IS_TREE_LIST_ROW(row));

    GFileInfo *fileInfo = gtk_tree_list_row_get_item(row);
    g_return_if_fail(G_IS_FILE_INFO(fileInfo));

    GtkWidget *expander = gtk_list_item_get_child(list_item);
    g_return_if_fail(GTK_IS_TREE_EXPANDER(expander));
    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(expander), row);

    GtkWidget *box = gtk_tree_expander_get_child(GTK_TREE_EXPANDER(expander));
    g_return_if_fail(GTK_IS_BOX(box));

    GtkWidget *icon = gtk_widget_get_first_child(box);
    GtkWidget *label = gtk_widget_get_last_child(box);
    g_return_if_fail(GTK_IS_IMAGE(icon) && GTK_IS_LABEL(label));

    browserSetIconForFileInfo(GTK_IMAGE(icon), fileInfo);

    const gchar *name = g_file_info_get_name(fileInfo);
    gtk_label_set_text(GTK_LABEL(label), fsGetName(name, TRUE));

    // Si es la carpeta Content
    if (browserIsContentFolder(browser, g_file_get_path(gapp_browser_create_file_from_file_info(fileInfo))))
        gtk_tree_list_row_set_expanded(row, TRUE);
}

// MARK: UI

// TODO: Hacer esta función más dinámica
static GtkWidget *browserToolbarSetupInterfacePopoverMenu(GtkWidget *parent, GappBrowser *self)
{
    const menuItemAddFileNew menuItems[] = {
        {"folder-new-symbolic", "New folder", G_CALLBACK(onBrowserCreateFolderInDialog)},
        {NULL, NULL, NULL},
        {"document-new-symbolic", "Scene", G_CALLBACK(onBrowserCreateSceneInDialog)},
        {"document-new-symbolic", "Prefab", G_CALLBACK(onBrowserCreatePrefabInDialog)},
        // {"document-new-symbolic", "Script", G_CALLBACK(onBrowserCreateScriptInDialog)},
        {"applications-science-symbolic", "Component", G_CALLBACK(onBrowserCreateScriptInDialog)},
        {NULL, NULL, NULL},
        {"document-new-symbolic", "Animate Sprite", G_CALLBACK(onBrowserCreateAnimateSpriteInDialog)},
    };

    GtkWidget *popover = gtk_popover_new();
    gtk_widget_set_parent(popover, parent);
    // gtk_widget_add_css_class(popover, "popover");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_set_child(GTK_POPOVER(popover), box);

    for (size_t i = 0; i < G_N_ELEMENTS(menuItems); i++)
    {
        if (menuItems[i].iconName == NULL)
        {
            gtk_box_append(GTK_BOX(box), gapp_widget_separator_h());
            continue;
        }

        GtkWidget *item = gapp_widget_button_new_icon_with_label(menuItems[i].iconName, menuItems[i].name);
        gtk_box_append(GTK_BOX(box), item);

        if (menuItems[i].callback)
        {
            g_object_set_data(G_OBJECT(item), "title", stringDup(menuItems[i].name));
            g_object_set_data(G_OBJECT(item), "callback", menuItems[i].callback);
            g_object_set_data(G_OBJECT(item), "popover", popover);
            g_signal_connect(item, "clicked", G_CALLBACK(onBrowserToolbarCreateDialog), self);
        }
        else
            gtk_widget_set_sensitive(item, FALSE);
    }

    return popover;
}

static void browserShowPopover(GtkWidget *widget, GtkWidget *popover)
{
    gtk_popover_popup(GTK_POPOVER(popover));
}

static void browserHidePopover(GtkWidget *popover)
{
    gtk_popover_popdown(GTK_POPOVER(popover));
}

static void gappBrowserConfigureInterface(GappBrowser *self)
{
    GtkWidget *item;

    GtkWidget *toolbar = gapp_widget_toolbar_new();
    gtk_box_append(GTK_BOX(self), toolbar);
    {

        item = gapp_widget_button_new_icon_with_label("list-add-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_widget_set_tooltip_text(item, "More options...");
        gtk_box_append(GTK_BOX(toolbar), item);
        {
            GtkWidget *menu_popover = browserToolbarSetupInterfacePopoverMenu(item, self);
            g_signal_connect(item, "clicked", G_CALLBACK(browserShowPopover), menu_popover);
        }

        item = gapp_widget_button_new_icon_with_label("user-trash-symbolic", NULL);
        gtk_widget_set_size_request(item, -1, 20);
        gtk_box_append(GTK_BOX(toolbar), item);
        gtk_widget_set_tooltip_text(item, "Delete selected item(s)");
        g_signal_connect(item, "clicked", G_CALLBACK(onBrowserToolbarRemoveFileClicked), self);

        // item = gapp_widget_button_new_icon_with_label("applications-science-symbolic", "Components");
        // gtk_box_append(GTK_BOX(toolbar), item);
        // gtk_widget_set_tooltip_text(item, "Administrador de componentes");
        // g_signal_connect(item, "clicked", G_CALLBACK(onBrowserToolbarOpenComponentsClicked), self);
    }

    gtk_box_append(self, gapp_widget_separator_h());

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(self), scroll);
    {
        self->directory = gtk_directory_list_new("standard::*", NULL);
        browserSetMonitoredDirectory(self->directory);

        GtkTreeListModel *tree_model = gtk_tree_list_model_new(browserGetGListModelFromDirectoryList(self->directory),
                                                               FALSE,
                                                               FALSE,
                                                               onBrowserCreateChildModelForTreeRow,
                                                               NULL,
                                                               NULL);

        self->selection = gtk_multi_selection_new(G_LIST_MODEL(tree_model));

        GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(onBrowserViewFileSetupFactory), self);
        g_signal_connect(factory, "bind", G_CALLBACK(onBrowserViewFileBindFactory), self);

        GtkWidget *list_view = gtk_list_view_new(self->selection, factory);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_view);
        g_signal_connect(list_view, "activate", G_CALLBACK(onBrowserFileActivated), self);

        GtkDropTarget *drop = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
        g_signal_connect(drop, "drop", G_CALLBACK(onBrowserFileDrop), self);
        gtk_widget_add_controller(list_view, GTK_EVENT_CONTROLLER(drop));

        // GtkGesture *gesture = gtk_gesture_click_new();
        // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
        // g_signal_connect(gesture, "pressed", G_CALLBACK(gapp_browser_s_file_popup), self);
        // gtk_widget_add_controller(list_view, GTK_EVENT_CONTROLLER(gesture));
    }
}

// MARK: API PUBLIC

void browserSetFolder(GappBrowser *browser, const gchar *path)
{
    g_return_if_fail(GAPP_IS_BROWSER(browser));
    g_return_if_fail(path != NULL && *path != '\0');

    GFile *file = g_file_new_for_path(path);
    g_return_if_fail(G_IS_FILE(file));

    g_return_if_fail(GTK_IS_DIRECTORY_LIST(browser->directory));
    gtk_directory_list_set_file(GTK_DIRECTORY_LIST(browser->directory), file);
    browserSetMonitoredDirectory(GTK_DIRECTORY_LIST(browser->directory));

    g_object_unref(file);
}

void browserSetFolderContent(GappBrowser *browser)
{
    g_return_if_fail(GAPP_IS_BROWSER(browser));

    gchar *content_path = browserGetContentPath(browser);
    browserSetFolder(browser, content_path);
    g_free(content_path);
}

gchar *gapp_browser_get_current(GappBrowser *browser)
{
    g_return_val_if_fail(GAPP_IS_BROWSER(browser), NULL);

    GFileInfo *info = gapp_browser_get_selected_file(browser->selection);
    g_autofree gchar *pathfile = g_file_get_path(gapp_browser_create_file_from_file_info(info));

    if (!g_file_test(pathfile, G_FILE_TEST_IS_DIR))
        pathfile = pathDirname(pathfile);

    return pathfile;
}

gchar *browserGetContentPath(GappBrowser *browser)
{
    g_return_val_if_fail(GAPP_IS_BROWSER(browser), NULL);
    gchar *content_path = pathJoin(gapp_get_project_path(), "Game", NULL);
    return content_path;
}
