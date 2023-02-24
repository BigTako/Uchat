#include "../inc/header.h"

app_t *app_init() 
{
	app_t *app = NULL;
 
	// allocate memory for app struct
	app = (app_t *) malloc(sizeof(app_t));
	if (app == NULL) return NULL;
	memset(app, 0, sizeof(app_t));
	
    app->username = NULL;
    app->password = NULL;

	return app;
}

gboolean enter_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Return) {
        if (gtk_widget_has_focus(app->chat_entry)) send_message();
        if (gtk_widget_has_focus(app->find_user_entry)) find_user();
        return TRUE;
    }
    return FALSE;
}

void scroll() {
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(app->chat_scroller));
    gtk_adjustment_set_page_size(adjustment, 0);
    double value = gtk_adjustment_get_upper(adjustment);
    gtk_adjustment_set_value(adjustment, value);
}

void find_user() {
    //переписать под нормальный поиск)
    //user1 searches user2 in the searching panel
    //user1 gets user2 username
    const char *username = gtk_entry_get_text(GTK_ENTRY(app->find_user_entry));
    char request_buff[4096];
    char responce_buff[4096];
    char ** info_parts = NULL;
    //user1 makes a request to server(C@NAME@USERNAME1@USERNAME2@... - create new chat) with task to create a chat
    sprintf(request_buff, "%c%s?%s%s", CREATE_CHAT, QUERY_DELIM, QUERY_DELIM, username);
    // ? sign is putted to identify that name of chat have to be equal to username of user2(or to user1 if the user2 account)
    printf("server query: %s\n", request_buff);

    if (send(param->socket, request_buff, strlen(request_buff) + 1, 0) <= 0) {
        perror(errno);
    }
    if (recv(param->socket, responce_buff, 4096, 0) <= 0) {
        printf("[ERROR] Something went wrong while creating a chat with user %s\n", username);
    }
    else
    {
        switch (responce_buff[0])
        {
            case MESSAGE_CODE[0]:
                info_parts = mx_strsplit(responce_buff, QUERY_DELIM[0]);
                printf("Got a server responce: %s\n", responce_buff);
                create_chat(info_parts[1], "?", info_parts + 2);
                printf("[INFO] Chat successfuly created\n", username);
                mx_del_strarr(&info_parts);
                break;
            case 'N':
                open_error_window(responce_buff + 2);
                //printf("[ERROR] %s\n", );
                break;       
        }
    }

    /*if (send_server_request(param, request_buff) != 1)
    {
        printf("[ERROR] Something went wrong while creating a chat with user %s\n", username);
    }
    else
    {
        printf("[INFO] Chat successfuly created\n", username);
        create_chat("89", "mychat", "@alex@sheesh");
    }*/
    //server receives a request
    // if count of QUERY_DELIM in "members" part of request == 1(create dialog), then we have to check if this chat is already exists
    // if it does send a message to client, to make him know this
    // if it doesn't create it and send a client information about this chat (M@chat_id@chat_name@chat_members)

    mx_printstr(username);
}

void change_chat(GtkListBox* self, GtkListBoxRow* row, gpointer data) {
    //const char *name = gtk_widget_get_name(GTK_WIDGET(row));
    mx_printstr("changed chat\n");
    // app->chat_id = atoi(name);
    // change_chat(app->chat_id);
    // reshow_old_messages(app->chat_id);
    // g_print("Chat changed: %d\n", app->chat_id);
    // call_show_chat();
}

bool change = true;

void send_message() {
    //create_chat();
    const char *message = gtk_entry_get_text(GTK_ENTRY(app->chat_entry));
    //тут нужно сделать проверку на пробелы и обрезать сообщение от них, но я хз как потому что типы разные
    if (strlen(gtk_entry_get_text(GTK_ENTRY(app->chat_entry))) != 0) {
        if (change) create_message(message, true); 
        else create_message(message, false);
        gtk_entry_set_text(GTK_ENTRY(app->chat_entry), "");
        scroll();
    }
}

// void create_message(const char *m, bool is_user) {
//     GtkWidget *message, *icon, *username, *text, *datetime, *sticker;
//     GtkBuilder *builder = gtk_builder_new ();
//     GError* error = NULL;
//     char *window_path = NULL, *icon_path = "../resources/icons/user_icon.png";
//     char *timestr = NULL;
//     char *title = NULL;

//     // if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_other.glade", &error)) {
//     //     g_critical ("Couldn't load file: %s", window_path);
//     // }

//     if (change) {
//         if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_me.glade", &error)) {
//             g_critical ("Couldn't load file: %s", window_path);
//         }
//         change = false;
        
//     }
//     else {
//         if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_other.glade", &error)) {
//             g_critical ("Couldn't load file: %s", window_path);
//         }
//         change = true;
        
//     }

//     message = GTK_WIDGET(gtk_builder_get_object (builder, "message"));
//     if (!message) {
//         g_critical("Window widget error!");
//     }

//     if (!is_user) {
//         icon = GTK_WIDGET(gtk_builder_get_object(builder, "message_icon"));
//         username = GTK_WIDGET(gtk_builder_get_object(builder, "message_sender"));
//         text = GTK_WIDGET(gtk_builder_get_object(builder, "message_text"));
//         datetime = GTK_WIDGET(gtk_builder_get_object(builder, "message_time"));

//         gtk_widget_set_name(GTK_WIDGET(username), "message_sender");
//     }
//     else {
//         text = GTK_WIDGET(gtk_builder_get_object(builder, "message_text"));
//         datetime = GTK_WIDGET(gtk_builder_get_object(builder, "message_time"));
//     }

//     //берет текущее время, но его нужно будет хранить в базе данных чтоб потом правильно отображать время прошлых сообщений
//     time_t current_time;
//     struct tm *time_info;
//     char time_string[9]; // HH:MM\0

//     time(&current_time);
//     time_info = localtime(&current_time);

//     strftime(time_string, sizeof(time_string), "%H:%M", time_info);

//     gtk_widget_set_name(GTK_WIDGET(text), "message_text");

//     if(!is_user) {
//         gtk_label_set_text(GTK_LABEL(username), title);
//         gtk_image_set_from_file(GTK_IMAGE(icon), icon_path);
//     } 
//     gtk_label_set_text(GTK_LABEL(text), m);
//     gtk_label_set_text(GTK_LABEL(datetime), time_string);
//     //gtk_widget_set_sensitive(GTK_BOX(message), TRUE);

//     gtk_box_pack_start(GTK_BOX(app->messages_container), message, false, true, 0);
//     //g_signal_connect(G_OBJECT(message), "button-press-event", G_CALLBACK(button_press_cb), m);
//     g_object_unref(builder);
// }

void create_message(const char *m, bool is_user) {
    GtkWidget *message, *icon, *username, *text, *datetime, *sticker;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;
    char *window_path = NULL, *icon_path = "../resources/icons/user_icon.png";
    char *timestr = NULL;
    char *title = NULL;

    // if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_other.glade", &error)) {
    //     g_critical ("Couldn't load file: %s", window_path);
    // }

    if (change) {
        if (!gtk_builder_add_from_file (builder, "../resources/ui/my_message.glade", &error)) {
            g_critical ("Couldn't load file: %s", window_path);
        }
        change = false;
        
    }
    else {
        if (!gtk_builder_add_from_file (builder, "../resources/ui/other_message.glade", &error)) {
            g_critical ("Couldn't load file: %s", window_path);
        }
        change = true;
        
    }

    message = GTK_WIDGET(gtk_builder_get_object(builder, "message_button"));
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

    //берет текущее время, но его нужно будет хранить в базе данных чтоб потом правильно отображать время прошлых сообщений
    time_t current_time;
    struct tm *time_info;
    char time_string[9]; // HH:MM\0

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(time_string, sizeof(time_string), "%H:%M", time_info);

    gtk_widget_set_name(GTK_WIDGET(text), "message_text");
    gtk_widget_set_name(GTK_WIDGET(datetime), "message_time");

    if(!is_user) {
        gtk_label_set_text(GTK_LABEL(username), title);
        gtk_image_set_from_file(GTK_IMAGE(icon), icon_path);
    } 
    gtk_label_set_text(GTK_LABEL(text), m);
    gtk_label_set_text(GTK_LABEL(datetime), time_string);
    //gtk_widget_set_sensitive(GTK_BOX(message), TRUE);

    //gtk_builder_connect_signals(builder, NULL);

    g_signal_connect(message, "button-press-event", G_CALLBACK(my_message_menu), NULL);

    gtk_box_pack_start(GTK_BOX(app->messages_container), message, false, true, 0);
    //g_signal_connect(G_OBJECT(message), "button-press-event", G_CALLBACK(button_press_cb), m);
    g_object_unref(builder);
}

static gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    gtk_widget_grab_focus(widget);
    mx_printstr("hi");
    return TRUE;
}

void create_chat(char * chat_id, char * chat_name, char * chat_members) 
{
    //короче добавляются чаты в лист, но я хз конешно как переключать их
    GtkWidget *chat, *icon, *title, *status;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;

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

    char query_buff[1000];
    sprintf(query_buff,"My id is %s", chat_id);
    gtk_image_set_from_file(GTK_IMAGE(icon), "../resources/icons/message_icon.png");
    if (!strcmp(chat_name, "?"))
    {
        if (!strcmp(app->username_t, chat_members[0]))
            gtk_label_set_text(GTK_LABEL(title), chat_members[1]);    
        else
            gtk_label_set_text(GTK_LABEL(title), chat_members[0]);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(title), chat_name);
    }
    gtk_label_set_text(GTK_LABEL(status), query_buff);

    //gtk_widget_set_name(chat, itoa(c.chat_icon));

    gtk_list_box_insert(GTK_LIST_BOX(app->chat_list), chat, -1);
        
    g_object_unref(builder);
}

void show_settings(void) {
    gtk_widget_hide(app->chat_box);
    gtk_widget_hide(app->chats_sidebar);
    gtk_widget_show(app->settings_box);
}

void back_to_chat(void) {
    gtk_widget_show(app->chat_box);
    gtk_widget_show(app->chats_sidebar);
    gtk_widget_hide(app->settings_box);
}
