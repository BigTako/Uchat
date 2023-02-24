#include "../inc/header.h"

gboolean my_message_menu(GtkWidget *widget, GdkEventButton *event, gpointer data) {
   
    if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {
        // if (gtk_widget_has_focus(app->chat_entry)) send_message();
        // if (gtk_widget_has_focus(app->find_user_entry)) find_user();
        // return TRUE;
        mx_printstr("my\n");
        return TRUE;
    }
    return FALSE;
}

void other_message_menu() {
    mx_printstr("other\n");
}
