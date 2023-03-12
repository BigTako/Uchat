#include "../inc/header.h"

void edit_name() {
    char *new_name = gtk_entry_get_text(GTK_ENTRY(app->new_name_entry));
    if (strlen(new_name) != 0) {
        //нужно присвоить new_name на юзера если такого больше нету
        //if (все успешно) {
            gtk_label_set_text(GTK_LABEL(app->username_label), new_name);
            gtk_entry_set_text(GTK_ENTRY(app->new_name_entry), "");
            open_error_window("Username has been successfully changed!");
        //}
        //else {
            // gtk_entry_set_text(GTK_ENTRY(app->new_name_entry), "");
            // open_error_window("The username is already taken :(");
        //}
    }
}
