#include "../inc/header.h"

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
    app->find_user_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "user_search_bar"));
    //mx_printstr(app->username_t);

    g_signal_connect(window, "key_press_event", G_CALLBACK (enter_keypress), NULL);

    //gtk_label_set_text(GTK_LABEL(app->username_label), app->username_t);

    //gtk_label_set_text(GTK_LABEL(app->chat_label_info), " ");
    //gtk_widget_hide(app->chat_icon);

    //GET ALL CURRENT CONVERSATIONS
    char action = TAKE_CURRENT_CHATS;
    char * server_query = NULL;
    char responce_buff[MESSAGE_MAX_LEN];
    int num_of_chats = 0;
    bool online = true;
    char ** chat_info_parts = NULL;
    int members_rest_len = 0;
    //char ** chat_info = NULL;
    server_query = create_query_delim_separated(2, "F", "alex"); // have to store a hash password
    printf("Working with user (%s) (%s)\n", "alex", "sheesh");
    if (send(param->socket, server_query, strlen(server_query) + 1, 0) <= 0) online = false;
    if (recv(param->socket, responce_buff, MESSAGE_MAX_LEN, 0) <= 0) online = false;
    printf("Recived: %s\n", responce_buff);
    if (responce_buff[0] != 'W') printf("RECIVED ABOBA (%d)\n", responce_buff[0]); // recived something wrong there are error in clients code!
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
                    chat_info_parts = mx_strsplit(responce_buff, QUERY_DELIM[0]);                    
                    members_rest_len = strlen(chat_info_parts[0]) + strlen(chat_info_parts[1]) + strlen(chat_info_parts[2]) + 3;
                    create_chat(chat_info_parts[1], chat_info_parts[2] , responce_buff + members_rest_len);
                    if (send(param->socket, "Y", 1, 0) <= 0) online = false;
                    mx_del_strarr(&chat_info_parts);
                }
            }
        }
        printf("[INFO] Successfuly received %d packages\n", num_of_chats);
    }
    free(server_query);
    //GET ALL CURRENT CONVERSATIONS

    gtk_builder_connect_signals(ui_builder, NULL);

    gtk_widget_show(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}
