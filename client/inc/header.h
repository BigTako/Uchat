#ifndef HEADER_H
#define HEADER_H

#include "../../libs/libmx/inc/libmx.h"
#include <gtk/gtk.h>

void open_login_window(void);
GtkWidget *open_signup_window(void);
GtkWidget* show_login_form();

typedef struct app_s
{
    GtkWidget *login_window;
    GtkWidget *signup_window;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;
}              app_t;

app_t *app_init();
extern app_t *app;

#endif
