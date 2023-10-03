#include "GobuUiComponentSystem.h"
#include <gtksourceview/gtksource.h>
#include "config.h"
#include "GobuWidgets.h"
#include "GobuSignalListComponent.h"
#include "GobuSignalCreateComponent.h"
#include "GobuSignalListComponentSelected.h"
#include "GobuSignalListComponentSave.h"
#include "GobuSignalListComponentSaveVirtual.h"
#include "GobuSignalListComponentBuild.h"

static GtkWidget *GobuUiComponentSystem_Toolbar(void);
static GtkWidget *GobuUiComponentSystem_SourceView(void);
static GtkWidget *GobuUiComponentSystem_ListComponent(void);

GtkWidget *GobuUiComponentSystem(GobuProjectEditor *ctx)
{
    GtkWidget *self;
    GtkWidget *toolbar, *hpaned, *docked_v_left, *docked_h_right, *notebook;

    self = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    {
        // -----------------------------------------------------------
        // TOOLBAR
        // -----------------------------------------------------------
        {
            toolbar = GobuUiComponentSystem_Toolbar();
            gtk_box_append(self, toolbar);
            gtk_box_append(self, GobuWidgetSeparatorH());
        }

        hpaned = GobuWidgetPanedNew(GTK_ORIENTATION_HORIZONTAL, TRUE);
        gtk_box_append(self, hpaned);
        {
            docked_v_left = GobuWidgetPanedNotebookNew(GTK_ORIENTATION_VERTICAL, TRUE,
                                                       gtk_label_new("Components"), GobuUiComponentSystem_ListComponent(),
                                                       NULL, NULL);
            gtk_paned_set_start_child(GTK_PANED(hpaned), docked_v_left);

            // -----------------------------------------------------------
            // Una lista de sistemas separado de los componentes
            // -----------------------------------------------------------
            // notebook = gtk_notebook_new();
            // gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
            // gtk_widget_set_size_request(GTK_WIDGET(notebook), DOCK_L_MIN_SIZE, -1);
            // gtk_paned_set_end_child(GTK_PANED(docked_v_left), notebook);

            // system = GobuUiComponentSystem_ListComponent();
            // gtk_notebook_append_page(GTK_NOTEBOOK(notebook), system, gtk_label_new("Systems"));
            // -----------------------------------------------------------

            docked_h_right = GobuWidgetPanedNotebookNew(GTK_ORIENTATION_HORIZONTAL, FALSE,
                                                        gtk_label_new("View"), GobuUiComponentSystem_SourceView(),
                                                        NULL, NULL);
            gtk_paned_set_end_child(GTK_PANED(hpaned), docked_h_right);
        }
    }

    return self;
}

static GtkWidget *GobuUiComponentSystem_Toolbar(void)
{
    GtkWidget *toolbar, *item;

    GobuComponentSystem *component = GobuComponentSystemGet();

    toolbar = GobuWidgetToolbarNew();

    item = GobuWidgetButtonIconLabelNew("media-removable-symbolic", "Save");
    gtk_box_append(toolbar, item);
    g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalListComponentSave), component);

    GobuWidgetToolbarSeparatorW(toolbar);

    item = GobuWidgetButtonIconLabelNew("media-view-subtitles-symbolic", "Build");
    gtk_box_append(toolbar, item);
    g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalListComponentBuild), component);

    GobuWidgetToolbarSeparatorW(toolbar);

    item = GobuWidgetButtonIconLabelNew("input-gaming-symbolic", "Preview");
    gtk_box_append(toolbar, item);

    return toolbar;
}

static GtkWidget *GobuUiComponentSystem_SourceView(void)
{
    GtkWidget *source, *scroll;
    const char *const *scheme_list;
    // const char *const *lang_list;

    GobuComponentSystem *component = GobuComponentSystemGet();

    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    scheme_list = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);
    GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, scheme_list[1]);
    // printf("Theme: %s\n",ids[7]);
    // GtkWidget *preview = gtk_source_style_scheme_preview_new(scheme);

    GtkSourceLanguageManager *managerLanguage = gtk_source_language_manager_get_default();
    // lang_list = gtk_source_language_manager_get_language_ids(managerLanguage);
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language(managerLanguage, "c");
    // const gchar *const *p;
    // for (p = gtk_source_language_manager_get_language_ids(manager); p != NULL && *p != NULL; p++)
    // {
    //     printf("Lang: %s\n", *p);
    // }

    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(scroll), DOCK_L_MIN_SIZE, -1);

    source = gtk_source_view_new();
    gtk_source_view_set_show_line_numbers(source, TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(source), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(source), 10);
    {
        component->sobuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source)));
        gtk_source_buffer_set_style_scheme(component->sobuffer, scheme);
        gtk_source_buffer_set_language(component->sobuffer, lang);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), source);
        g_signal_connect(component->sobuffer, "changed", G_CALLBACK(GobuSignalListComponentSaveVirtual), component);
    }

    return scroll;
}

static GtkWidget *GobuUiComponentSystem_ListComponent(void)
{
    GtkWidget *self;
    GtkWidget *view, *toolbar, *scroll, *item;
    GtkSingleSelection *selection;
    GtkListItemFactory *factory;
    GListStore *store;

    GobuComponentSystem *component = GobuComponentSystemGet();

    self = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    {
        toolbar = GobuWidgetToolbarNew();
        gtk_box_append(self, toolbar);
        {
            item = GobuWidgetButtonIconLabelNew("list-add-symbolic", "Add");
            gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
            gtk_box_append(GTK_BOX(toolbar), item);
            g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalCreateComponent), component);

            GobuWidgetToolbarSeparatorW(toolbar);

            item = GobuWidgetButtonIconLabelNew("user-trash-full-symbolic", "Remove");
            gtk_button_set_has_frame(GTK_BUTTON(item), FALSE);
            gtk_box_append(GTK_BOX(toolbar), item);
            g_signal_connect(item, "clicked", G_CALLBACK(GobuSignalCreateComponent), component);
        }
    }

    gtk_box_append(self, GobuWidgetSeparatorH());

    {
        scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(scroll), DOCK_L_MIN_SIZE, -1);
        gtk_box_append(self, scroll);

        selection = gtk_single_selection_new(G_LIST_MODEL(component->components));
        // gtk_single_selection_set_autoselect(selection, TRUE);
        // gtk_single_selection_set_can_unselect(selection, FALSE);
        // gtk_single_selection_set_selected(selection, GTK_INVALID_LIST_POSITION);
        // g_signal_connect(selection, "notify::selected", G_CALLBACK(GobuSignalListComponentSelectedChanged), component);

        factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(GobuSignalListComponentSetup), NULL);
        g_signal_connect(factory, "bind", G_CALLBACK(GobuSignalListComponentBind), NULL);

        view = gtk_list_view_new(selection, factory);
        // gtk_list_view_set_single_click_activate(view, TRUE);
        // gtk_list_view_set_show_separators(view, TRUE);
        gtk_widget_add_css_class(view, "navigation-sidebar");
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
        g_signal_connect(view, "activate", G_CALLBACK(GobuSignalListComponentSelected), component);
    }

    return self;
}