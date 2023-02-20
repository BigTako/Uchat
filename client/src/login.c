#include "../inc/header.h"

app_t *app_init() {
	app_t *app = NULL;
 
	// allocate memory for app struct
	app = (app_t *) malloc(sizeof(app_t));
	if (app == NULL) return NULL;
	memset(app, 0, sizeof(app_t));
	
    app->username = NULL;
    app->password = NULL;

	return app;
}

void open_login_window(void) {
    gtk_widget_hide(app->signup_window);
    show_login_form();
}

GtkWidget *show_login_form() {
    GtkWidget *window;
    GtkBuilder * ui_builder;
    GError * err = NULL;

    ui_builder = gtk_builder_new();
    //загружаю юай из файла
    if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/login.glade", &err)) {
        g_critical ("Couldn't download the UI file : %s", err->message);
        g_error_free (err);
    }
    //беру окно по айди из глейда
    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "login_window"));
    if (!window) {
        g_critical ("Window widget error");
    }
    //имя для цсс
    gtk_widget_set_name(GTK_WIDGET(window), "login");
    //подключаю цсс
    GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "../resources/css/registration.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    gtk_builder_connect_signals(ui_builder, NULL);

    //записываю окна и виджеты в переменные структуры апп, чтоб потом брать из них глобально переменные
    app->login_window = window;
    app->username_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "username_entry"));
    app->password_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "password_entry"));

    app->login_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "login_button"));

    //gtk_entry_set_text(GTK_ENTRY(app->username_entry), );

    // gtk_entry_set_text(GTK_ENTRY(app->password_entry), "hello"); можно будет потом сделать регистрацию через сосноль
    gtk_widget_show(window); //показываю окно
 
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //для кнопки закрыть окно

    return window;
}

GtkWidget *open_signup_window(void) {
    gtk_widget_hide(app->login_window);
    GtkWidget *window;
    GtkBuilder * ui_builder;
    GError * err = NULL;

    ui_builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/signup.glade", &err)) {
        g_critical ("Couldn't download the UI file : %s", err->message);
        g_error_free (err);
    }

    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "signup_window"));
    if (!window) {
        g_critical ("Window widget error");
    }
    gtk_widget_set_name(GTK_WIDGET(window), "signup");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "../resources/css/registration.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    app->signup_window = window;
    app->signup_username_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "username_entry"));
    app->signup_password_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "password_entry"));
    app->signup_confirm_password_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "confirm_password_entry"));

    gtk_builder_connect_signals(ui_builder, NULL);

    gtk_widget_show(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

void log_in(void) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(app->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(app->password_entry));

    app->username_t = username;

    int login_error = check_login_data_for_errors(username, password);

    switch (login_error)
    {
    case 0:
        //функция для логирования и входа в чат
        open_main_window();
        break;
    case 1:
        open_error_window("Fields cannot be empty.");
        break;
    case 2:
        open_error_window("Login error: Incorrect symbols in username or password.");
        break;
    case 3:
        open_error_window("Login error: Incorrect password or username.");
        break;
    }
}

void create_account(void) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(app->signup_username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(app->signup_password_entry));
    const char *confirm_password = gtk_entry_get_text(GTK_ENTRY(app->signup_confirm_password_entry));

    int signup_error = check_signup_data_for_errors(username, password, confirm_password);

    switch (signup_error)
    {
    case 0:
        //функция для создания аккаунта и входа в чат
        mx_printstr("create account with data: ");
        mx_printstr(username);
        mx_printchar('\t');
        mx_printstr(password);
        mx_printchar('\n');
        open_login_window();
        break;
    case 1:
        open_error_window("Fields cannot be empty.");
        break;
    case 2:
        open_error_window("Signup error: Incorrect symbols in username or password.");
        break;
    case 3:
        open_error_window("Signup error: Username already exists.");
        break;
    case 4:
        open_error_window("Signup error: The passwords must match.");
        break;
    }

}
