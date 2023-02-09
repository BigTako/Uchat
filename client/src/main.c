#include "../inc/header.h"

void helloworld (GtkButton *button, gpointer   label) {
        gtk_label_set_text (label, "Nu i xueta etot vash uchat...");
}

int main(int argc, char * argv[]) {

    if (argc != 3) {
        mx_printerr("usage: ./uchat [IP] [PORT]\n");
        return 0;
    }
    GtkWidget *window;
    gtk_init(&argc, &argv);
    GtkBuilder * ui_builder;
    GError * err = NULL;

    ui_builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(ui_builder, "../resources/ui/registration.glade", &err)) {
        g_critical ("Не вышло загрузить файл с UI : %s", err->message);
        g_error_free (err);
    }

    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "registration_form"));
    if (!window) {
        g_critical ("Window widget error");
    }
    gtk_widget_set_name(GTK_WIDGET(window), "sign_up");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "../resources/ui/registration.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    gtk_builder_connect_signals(ui_builder, NULL);

    gtk_widget_show(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    

    return 0;
}
