#include "GobuUiMenuBar.h"

GMenu *gobu_editor_menubar_new(void)
{
    GMenu *menu_bar = g_menu_new();
    {
        GMenu *menu_file = g_menu_new();
        g_menu_append_submenu(menu_bar, "File", G_MENU_MODEL(menu_file));
        {
            GMenuItem *file_project = g_menu_item_new("New Project", "app.project");
            g_menu_append_item(menu_file, file_project);
            
            GMenuItem *file_exit = g_menu_item_new("Exit", "win.quit");
            g_menu_append_item(menu_file, file_exit);
        }
    }

    return menu_bar;
}