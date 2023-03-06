#include "../inc/header.h"

/*void get_and_show_user_chats(char action)
{
    char * server_query = NULL;
    char responce_buff[MESSAGE_MAX_LEN];
    int num_of_chats = 0;
    bool online = true;
    int members_rest_len = 0;
    //char ** chat_info = NULL;
    
    server_query = create_query_delim_separated(2, &action, app->username_t); // have to store a hash password
    //printf("Working with user (%s) (%s)\n", app->username, app->password);
    if (send(param->socket, server_query, strlen(server_query) + 1, 0) <= 0) online = false;
    if (recv(param->socket, responce_buff, MESSAGE_MAX_LEN, 0) <= 0) online = false;
    printf("Recived: %s\n", responce_buff);
    if (responce_buff[0] != 'W') 
    {
        printf("RECIVED ABOBA (%d)\n", responce_buff[0]); // recived something wrong there are error in clients code!
        return;
    }
    num_of_chats = atoi(responce_buff + 2);
    printf("Recived: %d\n", num_of_chats);
    if (online == true) 
    {
        if (send(param->socket, "Y", 1, 0) <= 0) online = false;
    }
    if (num_of_chats == 0)
    {
        printf("[INFO] No chats to receive\n");
    }
    else
    {
        for (int a = 0; a < num_of_chats; a++) 
        {
            if (online) 
            {
                memset(responce_buff, '\0', strlen(responce_buff));
                if (recv(param->socket, responce_buff, MESSAGE_MAX_LEN, 0) <= 0) online = false;
                if (online) 
                {
                    //M@chat_id@chat_name@LM_from_username@LM_message_text@LM_message_status@chat_members
                    printf("Got a chat info: %s\n", responce_buff);
                    create_chat(responce_buff + 2);
                    if (send(param->socket, "Y", 1, 0) <= 0) online = false;
                }
            }
        }
        printf("[INFO] Successfuly received %d packages\n", num_of_chats);
    }
    if (recv(param->socket, responce_buff, MESSAGE_MAX_LEN, 0) <= 0) online = false;
    free(server_query);
}*/


GtkWidget *open_main_window(void) 
{

    gtk_widget_hide(app->login_window);
    GtkWidget *window;
    GtkBuilder * ui_builder;
    GError * err = NULL;

    ui_builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/chat.glade", &err)) {
        g_critical ("Couldn't download the UI file : %s", err->message);
        g_error_free (err);
    }

    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "main_window"));
    if (!window) {
        g_critical ("Window widget error");
    }

    gtk_widget_set_name(GTK_WIDGET(window), "chat");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "../resources/css/chat.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    app->username_label = GTK_WIDGET(gtk_builder_get_object(ui_builder, "username_label"));
    app->chat_icon = GTK_WIDGET(gtk_builder_get_object(ui_builder, "other_user_icon"));
    app->chat_label_info = GTK_WIDGET(gtk_builder_get_object(ui_builder, "other_user_name"));

    app->chats_sidebar = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chats_sidebar"));
    gtk_widget_set_name(GTK_WIDGET(app->chats_sidebar), "chats_sidebar");

    app->chat_info = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_info"));
    gtk_widget_set_name(GTK_WIDGET(app->chat_info), "chat_info");

    GtkWidget *settings_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "settings_button"));
    gtk_widget_set_name(GTK_WIDGET(settings_button), "settings_button");

    GtkWidget *back_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "back_button"));
    gtk_widget_set_name(GTK_WIDGET(back_button), "back_button");

    app->chat_list = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_list"));
    gtk_widget_set_name(GTK_WIDGET(app->chat_list), "chat_list");

    app->messages_container = GTK_WIDGET(gtk_builder_get_object(ui_builder, "messages_container"));
    gtk_widget_set_name(GTK_WIDGET(app->messages_container), "messages_container");

    app->chat_box = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_box"));
    gtk_widget_set_name(GTK_WIDGET(app->chat_box), "chat_box");

    app->settings_box = GTK_WIDGET(gtk_builder_get_object(ui_builder, "settings_box"));
    gtk_widget_set_name(GTK_WIDGET(app->settings_box), "settings_box");

    app->chat_scroller = GTK_WIDGET(gtk_builder_get_object(ui_builder, "chat_scroller"));
    gtk_widget_set_name(GTK_WIDGET(app->chat_scroller), "chat_scroller");

    app->chat_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "new_message_entry"));
    app->chat_entry_box = GTK_WIDGET(gtk_builder_get_object(ui_builder, "message_entry"));
    app->find_user_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "user_search_bar"));
    //mx_printstr(app->username_t);
    app->welcome_message = GTK_WIDGET(gtk_builder_get_object(ui_builder, "welcome_message"));
    gtk_widget_set_name(GTK_WIDGET(app->welcome_message), "welcome_message");
    app->send_message_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "send_message_button"));

    //gtk_entry_set_text(GTK_ENTRY(app->username_label), app->username);
    gtk_label_set_text(GTK_LABEL(app->username_label), app->username_t);

    //gtk_label_set_text(GTK_LABEL(app->chat_label_info), " ");
    //gtk_widget_hide(app->chat_icon);

    //GET ALL CURRENT CONVERSATIONS
    
    //threadID = g_timeout_add(100, collect_messages, data);
    //get_and_show_user_chats(GET_CURRENT_CHATS);
    //g_timeout_add(10, renew_chat_list, NULL);
    //GET ALL CURRENT CONVERSATIONS
    //char action[] = {GET_CHATS_HISTORY, '\0'};
    //GET CHAT HISTORY IGNORING THE STATUS
    //collect_messages(action);
    //GET CHAT HISTORY IGNORING THE STATUS
    
    //GET NEW MESSAGES
    //action[0] = GET_NEW_MESSAGES;
    //guint threadID = g_timeout_add(100, collect_messages, (gpointer)"G");
    //GET NEW MESSAGES
    //START GETTING NEW MESSAGES CYCLE
    //START GETTING NEW MESSAGES CYCLE

    gtk_builder_connect_signals(ui_builder, NULL);

    //change_chat_by_id(0);

    gtk_widget_show(window);

    //signals
    g_signal_connect(window, "key_press_event", G_CALLBACK (enter_keypress), NULL);
    g_signal_connect(window, "key_press_event", G_CALLBACK (enter_escape), NULL);
    g_signal_connect(G_OBJECT(app->chat_list), "row-selected", G_CALLBACK(change_chat), "1");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //scroll();
    return window;
}
