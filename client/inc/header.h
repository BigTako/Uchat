#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "../libs/openssl/rsa.h"
#include "../libs/openssl/evp.h"
#include "../libs/openssl/pem.h"
#include "../libs/openssl/sha.h"
#include "../libs/libmx/inc/libmx.h"
///Users/alex/Desktop/studing/Uchat-danil/client/libs/openssl/pem.h

typedef unsigned char * code;
#define QUERY_DELIM "@"

#define LOGIN 'L'
#define SIGNUP 'R'
#define SEND_MESSAGE 'S'
#define CREATE_CHAT 'C'
#define GET_CHATS_HISTORY 'A'
#define EDIT_MESSAGE 'B'
#define DELETE_MESSAGE 'D'
#define EXIT_CONVERSATION 'E'
#define GET_CURRENT_CHATS 'F'
#define GET_NEW_MESSAGES 'G'

#define WAIT_FOR_CODE "W"
#define MESSAGE_CODE "M"

#define DB_ROWS_MAX 10000
#define MESSAGE_MAX_LEN 4096
#define KEY_LENGHT 4096
typedef unsigned char * code;


typedef struct s_send_param
{
	int socket;
    char *server_IP;
    char *server_port;
    pthread_mutex_t *mutex_R;
    int * cmdEXIT;
} t_send_param;


//action functions
void show_settings(void);
void send_message();
void create_message(char * message_query, bool to_end);
void find_user();
gboolean enter_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data);
void scroll();
void change_chat(GtkListBox* self, GtkListBoxRow* row, gpointer data);

gboolean my_message_menu(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean other_message_menu(GtkWidget *widget, GdkEventButton *event, gpointer data);
void create_options_popover(GtkWidget *widget, bool isMy);

//window functions
void open_login_window(void);
GtkWidget *open_signup_window(void);
GtkWidget* show_login_form();
GtkWidget *open_main_window(void);

//error handling
GtkWidget *open_error_window(char *error_message);
bool contains_wrong_char(const char *string);
int check_login_data_for_errors(const char *username, const char *password);
int check_signup_data_for_errors(const char *username, const char *password, const char *c_password);

//working with chats
void create_chat(char * chat_id, char * chat_name, char ** chat_members);
void collect_messages(char * action);

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

    GtkWidget *chat_window;
    GtkWidget *chat_entry;
    GtkWidget *find_user_entry;
    GtkWidget *chats_sidebar;
    GtkWidget *username_label;
    GtkWidget *user_icon;
    GtkWidget *chat_icon;
    GtkWidget *messages_container;
    GtkWidget *chat_label_info;
    GtkWidget *chat_list;
    GtkWidget *chat_box;
    GtkWidget *settings_box;
    GtkWidget *chat_scroller;
    GtkWidget *chat_info;

    GtkWidget *my_options;
    GtkWidget *other_options;

    char *username;
    char *password;
    const char *username_t;

    char *active_message;
    char * current_chat; // HARDCODED IN APP_INIT!!!!!!!!!!!!!!!!!!!!!!
}              app_t;

app_t *app_init();
extern app_t *app;
extern t_send_param *param;

//request utils
int connect_to_server(t_send_param *param);
code create_query_delim_separated(int count, ...);
int send_server_request(t_send_param *param, code query);

#endif


