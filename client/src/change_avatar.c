#include "../inc/header.h"

void change_avatar(GtkWidget *widget, gpointer data) {
    const gchar *id = gtk_widget_get_name(GTK_BUTTON(widget));
    g_print("Button with ID %s is clicked\n", id);

    char *path = NULL;
    asprintf(&path, "../resources/icons/user_icons/%s.png", id);

    app->user_avatar = path;

    printf("\n%s\n", path);

    gtk_image_set_from_file(GTK_IMAGE(app->user_icon), app->user_avatar);
    gtk_image_set_from_file(GTK_IMAGE(app->settings_current_icon), app->user_avatar);
}

// void *make_avatar_path(int avatar_id) {
//     char *icon_path = NULL;
//     asprintf(&icon_path, "../resources/icons/user_icons/%d.png", avatar_id);
//     return icon_path;
// }
