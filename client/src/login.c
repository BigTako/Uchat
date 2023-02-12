#include "../inc/header.h"

app_t *app_init() {
	app_t *app = NULL;
 
	// allocate memory for app struct
	app = (app_t *) malloc(sizeof(app_t));
	if (app == NULL) return NULL;
	memset(app, 0, sizeof(app_t));
	
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
    if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/login.glade", &err)) {
        g_critical ("Couldn't download the UI file : %s", err->message);
        g_error_free (err);
    }

    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "login_window"));
    if (!window) {
        g_critical ("Window widget error");
    }
    gtk_widget_set_name(GTK_WIDGET(window), "login");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "../resources/css/registration.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    gtk_builder_connect_signals(ui_builder, NULL);

    app->login_window = window;
    app->username_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "username_entry"));
    app->password_entry = GTK_WIDGET(gtk_builder_get_object(ui_builder, "password_entry"));

    app->login_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "login_button"));

    // gtk_entry_set_text(GTK_ENTRY(app->password_entry), "hello"); можно будет потом сделать регистрацию через сосноль

    gtk_widget_show(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

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

    gtk_builder_connect_signals(ui_builder, NULL);

    gtk_widget_show(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

void log_in(void) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(app->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(app->password_entry));

    mx_printstr(username);
    mx_printstr("\t");
    mx_printstr(password);
    mx_printstr("\n");
}

void create_account(void) {
    mx_printstr("created acc\n");
}
