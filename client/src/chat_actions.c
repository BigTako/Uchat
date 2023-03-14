#include "../inc/header.h"

static guint threadID = 0;

void create_message_widget(char * message_query, bool to_end) 
{
    /*
        MESSAGE QUERY FORMAT: message_status@m_id@m_sender_username@m_text@m_send_datetime@m_chat_id
        parts[0] - message id
        parts[1] - from_username
        parts[2] - message_text
        parts[3] - send_datetime
        parts[4] - chat_id
    */
    GtkWidget *message, *icon, *username, *text, *datetime, *sticker;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;
    char *window_path = NULL, *icon_path = "../resources/icons/user_icon.png";
    char *timestr = NULL;
    char *title = NULL;
    char ** parts = mx_strsplit(message_query, QUERY_DELIM[0]);
    printf("Got a message query: %s\n", message_query);
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
        parts[0] - message id
        parts[1] - from_username
        parts[2] - message_text
        parts[3] - send_datetime
        parts[4] - chat_id
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
    if (is_user) message_struct->message_label = text;
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

void create_chat_widget(char * chat_info_query) 
{
    //короче добавляются чаты в лист, но я хз конешно как переключать их
    GtkWidget *chat, *icon, *title, *status, *id;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;
    printf("Got chat info query: %s\n", chat_info_query);
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

void process_message_info(char * message_info)
{
    //m_status@m_id@m_sender_username@m_text@m_send_datetime@m_chat_id
    char text_buf[1000];
    char m_status = *(tokenize(message_info, QUERY_DELIM[0], text_buf, 1));
    if (m_status == ACTIVE_STATUS[0])
    {
        create_message_widget(message_info + 2, 0);
    }
    else if (m_status == DELETED_STATUS[0])
    {
        printf("want to delete message widget with id(%s)\n", tokenize(message_info, QUERY_DELIM[0], text_buf, 2));
        /*
            delete widget of chat here
            ID = tokenize(message_info, QUERY_DELIM[0], text_buf, 2);
        */
    }
    else if (m_status == EDITED_STATUS[0])
    {
        printf("want to edit message widget with id (%s)\n", tokenize(message_info, QUERY_DELIM[0], text_buf, 2));
        /*
            update message widget labels by it's ID
            ID = tokenize(message_info, QUERY_DELIM[0], text_buf, 2);
            NEW_MESSAGE_CONTENT = tokenize(message_info, QUERY_DELIM, text_buf, 4);
        */
    }
}


void process_chat_info(char * chat_info)
{
    //chat_status@chat_name@LM_sender@LM_text@LM_status@chat_members
    char text_buf[10];
    char c_status = *(tokenize(chat_info, QUERY_DELIM[0], text_buf, 1));
    if (c_status == ACTIVE_STATUS[0])
    {
        create_chat_widget(chat_info + 2);
    }
    else if (c_status == DELETED_STATUS[0])
    {
        printf("Want to delete a chat with id %s\n", tokenize(chat_info, QUERY_DELIM[0], text_buf, 2));
        /*
            delete widget of chat here
            ID = tokenize(chat_info, QUERY_DELIM[0], text_buf, 2); 
        */
    }
}

void find_user() {
    const char *username = gtk_entry_get_text(GTK_ENTRY(app->find_user_entry));
    char request_buff[4096];
    char responce_buff[4096];
    char chat_query_buff[4096];

    char ** info_parts = NULL;
    sprintf(request_buff, "%c%s?%s%s", CREATE_CHAT, QUERY_DELIM, QUERY_DELIM, username);
    printf("server query: %s\n", request_buff);
        
    u_send(param, request_buff, strlen(request_buff) + 1); // send a request to server

    if (u_recv(param, responce_buff, MESSAGE_MAX_LEN) > 0)
    {
        printf("[INFO] Received buff(%s)\n", responce_buff);
        if (responce_buff[0] == OK_CODE[0])
        {
            //process_chat_info(responce_buff + 2);
            printf("[INFO] Chat successfuly created(%s)\n", username);
        }
        else if (responce_buff[0] == RECORD_EXISTS_CODE[0])
        {
            printf("Chat id to transfter to: %s\n", responce_buff + 2);
            change_chat_by_id(responce_buff + 2);
        }
        else if (responce_buff[0] == ERROR_CODE[0])
        {
            open_error_window(responce_buff + 2);
        }
    }
    mx_printstr(username);
}

void change_chat(GtkListBox* self, GtkListBoxRow* row, gpointer data) 
{
    change_chat_by_id(gtk_widget_get_name(GTK_WIDGET(row)));
}


void change_chat_by_id(char * new_chat_id) 
{
    if (!new_chat_id) return;
    char query_buff[1000]; 
    free(app->current_chat_id);
    app->current_chat_id = mx_strdup(new_chat_id);

    if (new_chat_id[0] != '0') 
    {
        gtk_widget_hide(app->welcome_message);
        gtk_widget_show(app->chat_entry_box);
        gtk_widget_show(app->chat_label_info);
        gtk_widget_show(app->chat_icon);
        gtk_widget_show(app->status);
        if (threadID != 0)
        {
            g_source_remove(threadID);
        }   
        delete_all_history();
        sprintf(query_buff, "%s%s%s", RESET_MESSAGES_STATUS, QUERY_DELIM, app->current_chat_id);
        u_send(param, query_buff, strlen(query_buff) + 1);
        //apply_collocutor_info();
        threadID = g_timeout_add(100, collect_user_info, (gpointer)"T");
    }
    else {
        gtk_widget_show(app->welcome_message);
        gtk_widget_hide(app->chat_entry_box);
        gtk_widget_hide(app->chat_label_info);
        gtk_widget_hide(app->chat_icon);
        gtk_widget_hide(app->status);
        delete_all_history();
    }
    scroll();
}

void delete_all_history() {
    GtkContainer *box_container = GTK_CONTAINER(app->messages_container); // assuming 'box' is your GtkBox container
    GList *children, *iter;
    children = gtk_container_get_children(box_container);
    for(iter = children; iter != NULL; iter = iter->next) 
    {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

void clear_chat_list() 
{
    GtkContainer *box_container = GTK_CONTAINER(app->chat_list); // assuming 'box' is your GtkBox container
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
    //S@TEXT@TIME@CONVERSATION_ID - send message
    //m_status@m_id@m_sender_username@m_text@m_send_datetime@m_chat_id
    printf("Got a message from input fiend: %s\n", message);
    char action[2] = {SEND_MESSAGE, '\0'};
    char * cur_time = mx_itoa((time(NULL)));
    char * server_query = create_query_delim_separated(4, action, message, cur_time, app->current_chat_id);
    char * message_query = NULL;
    char responce_buff[5100];
    
    printf("Created server query: %s\n", server_query);
    
    u_send(param, server_query, strlen(server_query) + 1);
    gtk_entry_set_text(GTK_ENTRY(app->chat_entry), "");
    scroll();
    free(server_query);
    free(cur_time);
}

