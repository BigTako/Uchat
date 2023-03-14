#include "../inc/header.h"

GtkWidget *get_message_by_id(int id) {
    GtkContainer *box_container = GTK_CONTAINER(app->messages_container); // assuming 'box' is your GtkBox container
    GList *children, *iter;
    children = gtk_container_get_children(box_container);
    for(iter = children; iter != NULL; iter = iter->next) 
    {
        const char *name = gtk_widget_get_name(GTK_WIDGET(iter->data));
        //g_print("\nMessage id: %s\n", name);
        if (strcmp(mx_itoa(id), name) == 0) {
            //printf("\nThere is my id!!\n");
            return iter->data;
        }
    }
    g_list_free(children);
    return NULL;
}

GtkWidget *get_chat_by_id(int id) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(app->chat_list));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        const char *child_name = gtk_widget_get_name(child);
        if (child_name != NULL && strcmp(child_name, mx_itoa(id)) == 0) {
            // Found the widget, do something with it
            //g_print("Found chat with id: %s\n", mx_itoa(id));
            return child;
        }
    }
    return NULL;
    g_list_free(children);
}

// void edit_message_by_id(int id, char *text) {
//     GtkWidget *message_to_change =  get_message_by_id(id);


    
//     //gtk_label_set_text(GTK_LABEL(second_label), "sosat");
// }
