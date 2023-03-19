#include "../inc/header.h"

void change_theme(GtkComboBoxText *box, gpointer user_data) {
    gchar *active_id = gtk_combo_box_get_active_id(box);
    delete_all_history();
    change_chat_by_id(START_PAGE);
    gtk_widget_queue_draw(app->chat_window);
    load_css(atoi(active_id));
    gtk_widget_queue_draw(app->chat_window);
}

void load_css_from_file() {
    FILE *file = fopen("../resources/settings.txt", "r");
    int theme = 0;
    if (file == NULL) {
        save_user_theme(0);
        file = fopen("../resources/settings.txt", "r");
    }
    fscanf(file, "%d", &theme);
    load_css(theme);
    fclose(file);
}

void load_css(int theme) {
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    switch (theme)
    {
    case 0:
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/dark/chat.css", NULL);
        break;
    case 1:
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/white/chat.css", NULL);
        break;
    case 2:
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/blue/chat.css", NULL);
        break;
    case 3:
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/pink/chat.css", NULL);
        break;
    case 4:
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/yellow/chat.css", NULL);
        break;
    case 5:
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/purple/chat.css", NULL);
        break;
    default:
        mx_printerr("Unknown theme. Loading dark...\n");
        theme = 0;
        gtk_css_provider_load_from_path(cssProvider, "../resources/css/dark/chat.css", NULL);
        break;
    }
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    save_user_theme(theme);
}

void save_user_theme(int theme) {
    FILE *file = fopen("../resources/settings.txt", "w");
    fprintf(file, "%d", theme);
    fclose(file);
    if (app->theme_combobox != NULL) gtk_combo_box_set_active(GTK_COMBO_BOX(app->theme_combobox), theme);
}
