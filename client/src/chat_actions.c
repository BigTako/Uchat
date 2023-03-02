#include "../inc/header.h"



void collect_messages(char * action)
{
    char responce_buff[MESSAGE_MAX_LEN + 100];
    int count_of_messages = 0;
    bool online = true;
    pthread_mutex_lock(param->mutex_R);
    printf("in collect function\n");
    if (send(param->socket, action, 1, 0) <= 0) 
    {
        perror(strerror(errno));
    }

    if (recv(param->socket, responce_buff, 1000, 0) <= 0) // receive a repsonce with count of messages
    {
        online = false;
        printf("[ERROR] Something went wrong while GET MESSAGES request handling\n");
    }

    if (send(param->socket, "Y", 2, 0) <= 0)  // send a responce to server with info that count is received
    {
        online = false;
        perror(strerror(errno));
    }

    printf("After send-receive\n");
    
    if (responce_buff[0] == WAIT_FOR_CODE[0])
    {
        printf("Ready to recieve %s messages\n", responce_buff + 2);
        count_of_messages = atoi(responce_buff + 2);
        memset(responce_buff, '\0', strlen(responce_buff));
        for (int a = 0; a < count_of_messages; a++) 
        {
            if (online) 
            {
                memset(responce_buff, '\0', strlen(responce_buff));
                if (recv(param->socket, responce_buff, MESSAGE_MAX_LEN, 0) <= 0) online = false;
                if (online) 
                {
                    //M@message_id@from_username@message_text@send_datetime@conversation_id
                    create_message(responce_buff + 2, 0);
                    if (send(param->socket, "Y", 2, 0) <= 0) online = false;
                }
            }
	    }
        printf("[INFO] Successfuly received %d packages\n", count_of_messages);
    }
    else if (responce_buff[0] == 'N')
    {
        printf("[INFO] No messages to receive\n");
    }
    else
    {
        printf("[ERROR] Undefined expression\n");
    }
    if (recv(param->socket, responce_buff, MESSAGE_MAX_LEN, 0) <= 0) online = false;
    pthread_mutex_unlock(param->mutex_R);
}

void find_user() {
    //переписать под нормальный поиск)
    //user1 searches user2 in the searching panel
    //user1 gets user2 username
    const char *username = gtk_entry_get_text(GTK_ENTRY(app->find_user_entry));
    char request_buff[4096];
    char responce_buff[4096];
    char chat_query_buff[4096];

    char ** info_parts = NULL;
    //user1 makes a request to server(C@NAME@USERNAME1@USERNAME2@... - create new chat) with task to create a chat
    sprintf(request_buff, "%c%s?%s%s", CREATE_CHAT, QUERY_DELIM, QUERY_DELIM, username);
    // ? sign is putted to identify that name of chat have to be equal to username of user2(or to user1 if the user2 account)
    pthread_mutex_lock(param->mutex_R);
    printf("server query: %s\n", request_buff);
        
    if (send(param->socket, request_buff, strlen(request_buff) + 1, 0) <= 0) {
        perror(strerror(errno));
    }
    if (recv(param->socket, responce_buff, 4096, 0) <= 0) {
        printf("[ERROR] Something went wrong while creating a chat with user %s\n", username);
    }
    else
    {
        if (responce_buff[0] == MESSAGE_CODE[0])
        {
            create_chat(responce_buff + 2);
            /*info_parts = mx_strsplit(responce_buff, QUERY_DELIM[0]);
            printf("Got a server responce: %s\n", responce_buff);
            create_chat(info_parts[1], "?", info_parts + 2);
            printf("[INFO] Chat successfuly created(%s)\n", username);
            mx_del_strarr(&info_parts);*/
            printf("[INFO] Chat successfuly created(%s)\n", username);
        }
        else if (responce_buff[0] == 'N')
        {
            open_error_window(responce_buff + 2);
        }
        else
        {
            open_error_window("Undefined expression");
        }
    }
    pthread_mutex_unlock(param->mutex_R);
    mx_printstr(username);
}

void change_chat(GtkListBox* self, GtkListBoxRow* row, gpointer data) {
    const char *name = gtk_widget_get_name(GTK_WIDGET(row));

    app->current_chat_id = atoi(name);

    change_chat_by_id(app->current_chat_id);
    
    printf("Name: %s %d\n",name, atoi(name));
    // GtkWidget *chat = GTK_WIDGET(gtk_builder_get_object(builder, "chat_id"));
    mx_printstr("changed chat\n");
    // app->chat_id = atoi(name);
    // change_chat(app->chat_id);
    // reshow_old_messages(app->chat_id);
    // g_print("Chat changed: %d\n", app->chat_id);
    // call_show_chat();
}

char **fake_chat1 = {
    "hello world",
    "test chat1",
    "amogus",
    "a",
    "sus"
};

char **fake_chat2 = {
    "hello world",
    "test chat2",
    "amogus",
    "a",
    "sus"
};

void change_chat_by_id(int chat_id) {
    if (chat_id != 0) {
        gtk_widget_hide(app->welcome_message);
        gtk_widget_show(app->chat_entry_box);
        gtk_widget_show(app->chat_label_info);
        gtk_widget_show(app->chat_icon);
        delete_all_history();
        show_chat_history(app->current_chat_id);
    }
    else {
        gtk_widget_show(app->welcome_message);
        gtk_widget_hide(app->chat_entry_box);
        gtk_widget_hide(app->chat_label_info);
        gtk_widget_hide(app->chat_icon);
        delete_all_history();
    }
}

void show_chat_history(int chat_id) {
    //M@message_id@from_username@message_text@send_datetime@conversation_id
    //printf("\n\nCHAT ID: %d")
    if (chat_id != 0) {
        for (int i = 0; i < 5; i++) {
            char *message = NULL;
            asprintf(&message, "M@100@%d@M@18:01@1", chat_id);
            create_message(message, false);
        }
    }
}

void delete_all_history() {
    GtkContainer *box_container = GTK_CONTAINER(app->messages_container); // assuming 'box' is your GtkBox container
    GList *children, *iter;

    children = gtk_container_get_children(box_container);
    for(iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

void send_message() 
{
    const char *message = correct_input(gtk_entry_get_text(GTK_ENTRY(app->chat_entry)));
    //when we'he got a content, message information can be transfered to database
    //S@TEXT@TIME@CONVERSATION_ID - send message
    //M@message_id@from_username@message_text@send_datetime@conversation_id
    printf("Got a message from input fiend: %s\n", message);
    char action[2] = {SEND_MESSAGE, '\0'};
    char * cur_time = mx_itoa((time(NULL)));
    char * server_query = create_query_delim_separated(4, action, message, cur_time, app->current_chat);
    char * message_query = NULL;
    char responce_buff[5100];
    
    printf("Created server query: %s\n", server_query);
    pthread_mutex_lock(param->mutex_R);
    if (send(param->socket, server_query, strlen(server_query) + 1, 0) <= 0) 
    {
        perror(strerror(errno));
    }

    if (recv(param->socket, responce_buff, 4096, 0) <= 0 || responce_buff[0] == 'N') 
    {
        printf("[ERROR] Something went wrong while inserting message to db, buff=%s\n", responce_buff);
    }
    else
    {
        printf("[INFO]] Successfuly inserted message to database, responce_buff =%s\n", responce_buff);
    }
    free(server_query);
    
    message_query = create_query_delim_separated(5, responce_buff, app->username_t, message, cur_time, app->current_chat);

    if (strlen(gtk_entry_get_text(GTK_ENTRY(app->chat_entry))) != 0) 
    {
        //message_id@from_username@message_text@send_datetime@conversation_id
        create_message(server_query, 0);
        gtk_entry_set_text(GTK_ENTRY(app->chat_entry), "");
        scroll();
    }
    free(message_query);
    free(cur_time);
    pthread_mutex_unlock(param->mutex_R);
}


void create_message(char * message_query, bool to_end) 
{
    //M@message_id@from_username@message_text@send_datetime@conversation_id
    GtkWidget *message, *icon, *username, *text, *datetime, *sticker;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;
    char *window_path = NULL, *icon_path = "../resources/icons/user_icon.png";
    char *timestr = NULL;
    char *title = NULL;
    char ** parts = mx_strsplit(message_query, QUERY_DELIM[0]);
    printf("create message query: %s\n", message_query);
    printf("ID of message: %s\n", parts[0]);
    bool is_user = !strcmp(app->username_t, parts[1]);

    t_message *message_struct;
    message_struct = (t_message*)malloc(sizeof(t_message));

    if (is_user)
    {
        if (!gtk_builder_add_from_file (builder, "../resources/ui/my_message.glade", &error)) {
            g_critical ("Couldn't load file: %s", window_path);
        }
    }
    else
    {
        if (!gtk_builder_add_from_file (builder, "../resources/ui/other_message.glade", &error)) {
            g_critical ("Couldn't load file: %s", window_path);
        }
    }

    message = GTK_WIDGET(gtk_builder_get_object (builder, "message_button"));
    if (!message) {
        g_critical("Window widget error!");
    }

    if (!is_user) {
        icon = GTK_WIDGET(gtk_builder_get_object(builder, "message_icon"));
        username = GTK_WIDGET(gtk_builder_get_object(builder, "message_sender"));
        text = GTK_WIDGET(gtk_builder_get_object(builder, "message_text"));
        datetime = GTK_WIDGET(gtk_builder_get_object(builder, "message_time"));

        gtk_widget_set_name(GTK_WIDGET(username), "message_sender");
    }
    else {
        text = GTK_WIDGET(gtk_builder_get_object(builder, "message_text"));
        datetime = GTK_WIDGET(gtk_builder_get_object(builder, "message_time"));
    }
    time_t current_time = (time_t)atoi(parts[3]);
    struct tm *time_info;
    char time_string[9]; // HH:MM\0

    time_info = localtime(&current_time);

    strftime(time_string, sizeof(time_string), "%H:%M", time_info);

    gtk_widget_set_name(GTK_WIDGET(text), "message_text");

    if(!is_user) {
        gtk_label_set_text(GTK_LABEL(username), title);
        gtk_image_set_from_file(GTK_IMAGE(icon), icon_path);
    } 

    GtkWidget *message_id = GTK_WIDGET(gtk_builder_get_object(builder, "message_id"));
    gtk_label_set_text(GTK_LABEL(message_id), parts[0]);

    /*
        M@message_id@from_username@message_text@send_datetime@conversation_id
        parts[0] - message_id
        parts[1] - from_username
        parts[2] - message_text
        parts[3] - send_datetime
        parts[4] - conversation_id
    */

    gtk_label_set_text(GTK_LABEL(text), restore_input(parts[2]));
    gtk_label_set_text(GTK_LABEL(datetime), time_string);

    const char *label_text;
    label_text = gtk_label_get_text(GTK_LABEL(text));

    const char *id_text;
    id_text = gtk_label_get_text(GTK_LABEL(message_id));
    //printf("\nGet text from message widget with ID: %s\n", id_text);

    message_struct->id = atoi(id_text);
    message_struct->message_text = gtk_label_get_text(GTK_LABEL(text));

    // if (is_user) g_signal_connect(message, "button-press-event", G_CALLBACK(my_message_menu), (gpointer)label_text);
    // else g_signal_connect(message, "button-press-event", G_CALLBACK(other_message_menu), (gpointer)label_text);

    if (is_user) g_signal_connect(message, "button-press-event", G_CALLBACK(my_message_menu), message_struct);
    else g_signal_connect(message, "button-press-event", G_CALLBACK(other_message_menu), message_struct);

    //message_data->id = NULL;
    //message_data->message_text = NULL;

    //play_music();

    if(to_end)
    {
        gtk_box_pack_end(GTK_BOX(app->messages_container), message, false, true, 0);
    }
    else
    {
        gtk_box_pack_start(GTK_BOX(app->messages_container), message, false, true, 0);
    }
    scroll();
    g_object_unref(builder);
    mx_del_strarr(&parts);
}

void create_chat(char * chat_info_query) 
{
    //короче добавляются чаты в лист, но я хз конешно как переключать их
    GtkWidget *chat, *icon, *title, *status, *id;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;
    char ** parts = mx_strsplit(chat_info_query, QUERY_DELIM[0]);

    if (!gtk_builder_add_from_file (builder, "../resources/ui/chat_list.glade", &error)) {
        g_critical ("Couldn't load file: message_other.ui");
    }

    chat = GTK_WIDGET (gtk_builder_get_object (builder, "chat"));
    if (!chat) {
        g_critical ("Window widget error");
    }

    icon = GTK_WIDGET(gtk_builder_get_object(builder, "chat_icon"));
    title = GTK_WIDGET(gtk_builder_get_object(builder, "chat_title"));
    status = GTK_WIDGET(gtk_builder_get_object(builder, "chat_status"));
    id = GTK_WIDGET(gtk_builder_get_object(builder, "chat_id"));

    char query_buff[1000];
    sprintf(query_buff, "%s:%s", parts[2], parts[3]);
    //M@chat_id@chat_name@LM_from_username@LM_message_text@LM_message_status@chat_members
    /*
        parts[0] - chat_id
        parts[1] - chat_name
        parts[2] - LM_from_username
        parts[3] - LM_message_text
        parts[4] - LM_message_status
        parts + 5 - chat members
    */

    gtk_image_set_from_file(GTK_IMAGE(icon), "../resources/icons/message_icon.png");
    if (!strcmp(parts[1], "?"))
    {
        if (!strcmp(app->username_t, parts[5]))
            gtk_label_set_text(GTK_LABEL(title), parts[6]);    
        else
            gtk_label_set_text(GTK_LABEL(title), parts[5]);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(title), parts[1]);
    }
    gtk_label_set_text(GTK_LABEL(status), query_buff);
    //gtk_label_set_text(GTK_LABEL(id), chat_id);

    gtk_widget_set_name(chat, parts[0]);

    gtk_list_box_insert(GTK_LIST_BOX(app->chat_list), chat, -1);
        
    g_object_unref(builder);
}
