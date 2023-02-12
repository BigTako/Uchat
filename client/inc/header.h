#ifndef HEADER_H
#define HEADER_H

#include "../../libs/libmx/inc/libmx.h"
#include <gtk/gtk.h>
#include <string.h>

void open_login_window(void);
GtkWidget *open_signup_window(void);
GtkWidget* show_login_form();

//error handling
GtkWidget *open_error_window(char *error_message);
bool contains_wrong_char(const char *string);
int check_login_data_for_errors(const char *username, const char *password);
int check_signup_data_for_errors(const char *username, const char *password, const char *c_password);

typedef struct app_s
{
    GtkWidget *login_window;
    GtkWidget *signup_window;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;

    GtkWidget *signup_username_entry;
    GtkWidget *signup_password_entry;
    GtkWidget *signup_confirm_password_entry;

    GtkWidget *error_window;
    GtkWidget *error_label;
    GtkWidget *error_button;

    char *username;
    char *password;
}              app_t;

app_t *app_init();
extern app_t *app;

#endif
