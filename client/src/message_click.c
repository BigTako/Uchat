#include "../inc/header.h"

static gboolean popup_open = FALSE;
static bool isOther;
static int selected_id;
static GtkWidget *label_to_change;

static bool delete_popup_open = false;
static GtkListBoxRow *row_to_delete;

gboolean my_message_menu(GtkWidget *widget, GdkEventButton *event, t_message *data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {

        // GtkWidget *selected_message = get_message_by_id(41);
        // gtk_widget_hide(selected_message);
        //char *str = data->message_text;

        //edit_message_by_id(41, "lol");

        g_print("Button clicked with user data: %s    ID: %d\n", data->message_text, data->id);
        selected_id = data->id;
        label_to_change = data->message_label;
        app->active_message = data->message_text;
        app->active_widget = widget;
        if (popup_open) {
            gtk_widget_hide(app->my_options);
            popup_open = FALSE;
        }
        else {
            isOther = false;
            create_options_popover(widget, true); 
            popup_open = TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

gboolean other_message_menu(GtkWidget *widget, GdkEventButton *event, t_message *data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        //char *str = data->message_text;
        g_print("Button clicked with user data: %s    ID: %d\n", data->message_text, data->id);
        app->active_message = data->message_text;
        selected_id = data->id;
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

    load_css_from_file();

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

void delete_message(GtkWidget *widget, gpointer data) 
{
    //???????????????? ???????? ?????????? ?????????? ?????????????? ?????????????????? ?? ???????????????????? 228, ???? GtkWidget *mes_to_delete = get_message_by_id(228);
    //gtk_widget_hide(mes_to_delete);
    gtk_widget_hide(app->active_widget);
    app->active_widget = NULL;
    if (!isOther) gtk_widget_hide(app->my_options);
    else gtk_widget_hide(app->other_options);
    char server_query[1000];
    sprintf(server_query, "%c%s%d", DELETE_MESSAGE, QUERY_DELIM, selected_id);
    u_send(param, server_query, strlen(server_query) + 1);
    //send_server_request(param, server_query);
    printf("[INFO] Successfuly deleted message with id(%d)\n", selected_id);
    //free(server_query);
}

void edit_message(GtkWidget *widget, gpointer data)
{
    /*
        //B@MESSAGE_ID@NEW_CONTENT
        char * new_content = "its new content";
        char server_query[1000];
        sprintf(server_query, "%c%s%d%s%s", EDIT_MESSAGE, QUERY_DELIM, selected_id, QUERY_DELIM, new_content);
        send_server_request(param, server_query);
        printf("[INFO] Successfuly edited message with id(%d)\n", selected_id);
    */
    gtk_widget_grab_focus(app->chat_entry);

    //???????? ?????????? ?????????????? ?? ???????????????? ?????????? ?? ?????????????????????? ?????????????????? ????, GtkWidget *mes = get_message_by_id(228);
    //
    if (app->active_message != NULL) 
    { 
        app->active_widget = NULL;
        if (!isOther) gtk_widget_hide(app->my_options);
        else gtk_widget_hide(app->other_options);
        gtk_entry_set_text(GTK_ENTRY(app->chat_entry), app->active_message);

        app->edit_message = true;
        //???????????????????? ?????????????? ???????????? ?? ?????????? ??????????
        g_signal_handlers_disconnect_by_func(app->send_message_button, (gpointer)send_message, NULL);
        g_signal_connect(app->send_message_button, "clicked", G_CALLBACK(set_text), NULL);
    }
}

void set_text() 
{
    gtk_label_set_text(GTK_LABEL(label_to_change), gtk_entry_get_text(GTK_ENTRY(app->chat_entry)));
    gtk_entry_set_text(GTK_ENTRY(app->chat_entry), "");

    char * new_message_content = gtk_label_get_text(GTK_LABEL(label_to_change));
    printf("Want to change content to %s, selected id is %d\n", new_message_content, selected_id);
    char query_buff[1000];
    sprintf(query_buff, "%c%s%d%s%s", EDIT_MESSAGE, QUERY_DELIM, selected_id, QUERY_DELIM, new_message_content);

    u_send(param, query_buff, strlen(query_buff) + 1);

    g_signal_handlers_disconnect_by_func(app->send_message_button, (gpointer)set_text, NULL);
    g_signal_connect(app->send_message_button, "clicked", G_CALLBACK(send_message), NULL);

    app->edit_message = false;
}

// gboolean chat_actions_menu(GtkWidget *widget, GdkEventButton *event, gpointer data) {
//     if (event->button == GDK_BUTTON_SECONDARY) { 
//         const char *name = gtk_widget_get_name(GTK_WIDGET(widget));
//         printf("\n\nID OF THIS: %s\n\n", name);
//         return TRUE;
//     }
//     else if (event->button == GDK_BUTTON_PRIMARY) {
//         return FALSE; // Allow the default behavior to proceed
//     }
//     return TRUE;
// }

gboolean chat_actions_menu(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->button == GDK_BUTTON_SECONDARY) { 
        //GtkWidget *selected_chat = get_chat_by_id(2);
        //gtk_widget_hide(selected_chat);

        GtkListBox *listbox = GTK_LIST_BOX(widget);
        GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(listbox);
        if (selected_row != NULL) {
            gint row_number = gtk_list_box_row_get_index(selected_row);
            row_to_delete = selected_row;

            const char *name = gtk_widget_get_name(GTK_WIDGET(selected_row));
            g_print("\n\nSelected row number: %d %s\n\n", row_number, name);
            
            if (delete_popup_open) {
                gtk_widget_hide(app->chat_options);
                delete_popup_open = false;
            }
            else {
                create_chat_options_popover();
                delete_popup_open = true;
            }
        }
        return TRUE;
    }
    else if (event->button == GDK_BUTTON_PRIMARY) return FALSE; 
    return TRUE;
}

void create_chat_options_popover() {
    GtkWidget *window;
    GtkBuilder * ui_builder;
    GError *err = NULL;

    ui_builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/chat_list_popup.glade", &err)) {
        g_critical ("Couldn't download the UI file : %s", err->message);
        g_error_free (err);
    }


    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "my_options_popup"));
    if (!window) {
        g_critical ("Window widget error");
    }
    gtk_widget_set_name(GTK_WIDGET(window), "popup");

    load_css_from_file();

    gtk_builder_connect_signals(ui_builder, NULL);

    app->chat_options =  GTK_WIDGET(gtk_builder_get_object(ui_builder, "my_options_popup"));

    gtk_widget_show(window);

    //g_signal_connect(window, "destroy", G_CALLBACK(on_popup_closed), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void delete_chat() {
    //???????????????? ???????? ?????????? ?????????? ?????????????? ?????? ?? ???????????????????? 228, ???? GtkWidget *chat_to_delete = get_chat_by_id(228);
    //gtk_widget_hide(chat_to_delete);
    gtk_widget_hide(GTK_WIDGET(row_to_delete));
    gtk_widget_hide(app->chat_options);
    delete_popup_open = false;
    delete_all_history();
    change_chat_by_id(START_PAGE);
    //gtk_container_remove(GTK_CONTAINER(listbox_delete), );
}
