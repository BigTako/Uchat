#include "../inc/header.h"

static gboolean popup_open = FALSE;
static bool isOther;

gboolean my_message_menu(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    //const char *label_text = gtk_label_get_text(GTK_BUTTON(widget));
    // char *str = (char*)data;
    // printf("Text: %s\n", str);
    // char *str = (char*)data;
    // g_print("!!!Button clicked with user data: %s\n", str);
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        char *str = data;
        g_print("Button clicked with user data: %s\n", str);
        app->active_message = mx_strdup(str);
        // if (gtk_widget_has_focus(app->chat_entry)) send_message();
        // if (gtk_widget_has_focus(app->find_user_entry)) find_user();
        // return TRUE;
        if (popup_open) {
            gtk_widget_hide(app->my_options);
            popup_open = FALSE;
        }
        else {
            isOther = false;
            create_options_popover(widget, true); 
            popup_open = TRUE;
        }
        //mx_printstr("my\n");
        // popup_open = TRUE;
        // create_my_options_popover(widget);
        return TRUE;
    }
    return FALSE;
}

gboolean other_message_menu(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        char *str = data;
        g_print("Button clicked with user data: %s\n", str);
        app->active_message = mx_strdup(str);
        if (popup_open) {
            gtk_widget_hide(app->other_options);
            popup_open = FALSE;
        }
        else {
            isOther = true;
            create_options_popover(widget, false);
            popup_open = TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

void create_options_popover(GtkWidget *widget, bool isMy) {
    GtkWidget *window;
    GtkBuilder * ui_builder;
    GError *err = NULL;

    ui_builder = gtk_builder_new();
    if (isMy) {
        if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/my_options_popup.glade", &err)) {
            g_critical ("Couldn't download the UI file : %s", err->message);
            g_error_free (err);
        }
    }
    else {
        if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/other_options_popup.glade", &err)) {
            g_critical ("Couldn't download the UI file : %s", err->message);
            g_error_free (err);
        }
    }

    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "my_options_popup"));
    if (!window) {
        g_critical ("Window widget error");
    }
    gtk_widget_set_name(GTK_WIDGET(window), "popup");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "../resources/css/popup.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_builder_connect_signals(ui_builder, NULL);

    if (isMy) app->my_options =  GTK_WIDGET(gtk_builder_get_object(ui_builder, "my_options_popup"));
    else app->other_options =  GTK_WIDGET(gtk_builder_get_object(ui_builder, "my_options_popup"));

    gtk_widget_show(window);

    //g_signal_connect(window, "destroy", G_CALLBACK(on_popup_closed), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void copy_text(GtkWidget *widget, gpointer data) {
    if (app->active_message != NULL) {
        g_print("Copy text: %s\n", app->active_message);
        GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
        gtk_clipboard_clear(GTK_CLIPBOARD(clipboard));
        gtk_clipboard_set_text(GTK_CLIPBOARD(clipboard), app->active_message, mx_strlen(app->active_message));
    }
    app->active_message = NULL;
    if (!isOther) gtk_widget_hide(app->my_options);
    else gtk_widget_hide(app->other_options);
}

void delete_message() 
{

}

void edit_message() {

}
