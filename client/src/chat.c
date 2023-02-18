#include "../inc/header.h"

GtkWidget *open_main_window(void) {
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
    //mx_printstr(app->username_t);
    gtk_label_set_text(GTK_LABEL(app->username_label), app->username_t);
    //gtk_label_set_text(GTK_LABEL(app->chat_label_info), " ");
    //gtk_widget_hide(app->chat_icon);


    gtk_builder_connect_signals(ui_builder, NULL);

    gtk_widget_show(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

void send_message() {
    // create_chat();
    const char *message = mx_strtrim(gtk_entry_get_text(GTK_ENTRY(app->chat_entry)));
    create_message(message);
    gtk_entry_set_text(GTK_ENTRY(app->chat_entry), "");
}

bool change = true;

void create_message(const char *m) {
    GtkWidget *message, *icon, *username, *text, *datetime, *sticker;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;
    char *window_path = NULL, *icon_path = "../resources/icons/user_icon.png";
    char *timestr = NULL;
    char *title = NULL;

    if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_me.glade", &error)) {
        g_critical ("Couldn't load file: %s", window_path);
    }

    /*if (change) {
        if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_me.glade", &error)) {
            g_critical ("Couldn't load file: %s", window_path);
        }
        change = false;
        mx_printint(change);
    }
    else {
        if (!gtk_builder_add_from_file (builder, "../resources/ui/message_from_other.glade", &error)) {
            g_critical ("Couldn't load file: %s", window_path);
        }
        change = true;
        mx_printint(change); 
    }*/

    message = GTK_WIDGET (gtk_builder_get_object (builder, "message"));
    if (!message) {
        g_critical ("Window widget error");
    }

    icon = GTK_WIDGET(gtk_builder_get_object(builder, "message_icon"));
    username = GTK_WIDGET(gtk_builder_get_object(builder, "message_username"));
    text = GTK_WIDGET(gtk_builder_get_object(builder, "message_text"));
    datetime = GTK_WIDGET(gtk_builder_get_object(builder, "message_datetime"));

    // timestr = ctime(&mes.datetime);
    // timestr[strlen(timestr) - 1] = '\0';

    if(username != NULL) gtk_label_set_text(GTK_LABEL(username), title);
    gtk_label_set_text(GTK_LABEL(text), m);
    gtk_label_set_text(GTK_LABEL(datetime), "16:35");
    gtk_image_set_from_file(GTK_IMAGE(icon), icon_path);

    gtk_box_pack_start(GTK_BOX(app->messages_container), message, false, true, 0);
    g_object_unref (builder);
}

void create_chat() {
    //короче добавляются чаты в лист, но я хз конешно как переключать их

    GtkWidget *chat, *icon, *title, *status;
    GtkBuilder *builder = gtk_builder_new ();
    GError* error = NULL;

    //asprintf(&icon_path, "resource/img/chat_icon/chat_icon_%d.png", c.chat_icon);

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

    gtk_image_set_from_file(GTK_IMAGE(icon), "../../resources/icons/user_icon.png");
    gtk_label_set_text(GTK_LABEL(title), "hei");
    gtk_label_set_text(GTK_LABEL(status), "i am the danger skyler");

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
