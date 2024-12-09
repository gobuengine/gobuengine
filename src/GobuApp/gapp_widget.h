#ifndef GB_WIDGET_H
#define GB_WIDGET_H

#include <gtk/gtk.h>
#include <adwaita.h>

#ifdef __cplusplus
extern "C"
{
#endif

    #define gapp_widget_button_new_icon_with_label(icon_name, label) gapp_widget_button_new_icon_with_label_custom(icon_name, label, GTK_ALIGN_START)

    typedef struct
    {
        GtkWidget *window;
        GtkWidget *entry;
        GtkWidget *button_ok;
        gpointer data;
    } GappWidgetDialogEntry;

    GtkWidget *gapp_widget_icon_svg_new(const gchar *icon_name);

    void gapp_widget_set_margin(GtkWidget *widget, gint margin);

    GtkWidget *gapp_widget_separator_h(void);

    GtkWidget *gapp_widget_toolbar_new(void);

    // button
    GtkWidget *gapp_widget_button_new_icon_svg_with_label(const gchar *icon_name, const gchar *label);
    GtkWidget *gapp_widget_button_new_icon_with_label_custom(const gchar *icon_name, const gchar *label, GtkAlign align);

    // notebook
    int gapp_widget_notebook_append_page(GtkWidget *notebook, GtkWidget *label, GtkWidget *child, gboolean isButtonClose);
    GtkWidget *gapp_widget_notebook_get_label(GtkWidget *notebook, GtkWidget *child);

    // dialog entry text
    GappWidgetDialogEntry *gapp_widget_dialog_new_entry_text(GtkWidget *parent, const gchar *title, const gchar *label);
    void gapp_widget_dialog_entry_text_destroy(GappWidgetDialogEntry *dialogEntry);

    // dialog remove file
    void gapp_widget_dialog_new_confirm_action(GtkWidget *parent, const gchar *title, const gchar *label, GAsyncReadyCallback *result, gpointer data);

    // expander button + label
    GtkWidget *gapp_widget_expander_new_label_icon(const gchar *label, const gchar *icon_name, gboolean expanded);
    GtkWidget *gapp_widget_expander_get_button(GtkWidget *expander);

    //
    void gapp_widget_text_view_set_text(GtkTextView *view, const gchar *text);
    gchar *gapp_widget_text_view_get_text(GtkTextBuffer *buffer);

    void gapp_widget_entry_set_text(GtkEntry *entry, const gchar *text);
    const gchar *gapp_widget_entry_get_text(GtkEntry *entry);

#ifdef __cplusplus
}
#endif

#endif // GB_WIDGET_H
