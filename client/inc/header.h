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
#include <stdlib.h>
#include <gtk/gtk.h>

#include "../libs/openssl/ssl.h"
#include "../libs/openssl/err.h"

#include "../libs/libmx/inc/libmx.h"
///Users/alex/Desktop/studing/Uchat-danil/client/libs/openssl/pem.h

typedef unsigned char * code;
#define QUERY_DELIM "@"

#define START_PAGE "0"

//GET REQUESTS
#define GET_ALL_CHATS 'F'
#define GET_NEW_CHATS 'H'
#define GET_NEW_MESSAGES 'G'
#define GET_CHATS_HISTORY 'A'
#define GET_COLLOCUTOR_INFO 'K'

//POST REQUESTS
#define SEND_MESSAGE 'S'
#define CREATE_CHAT 'C'
#define EDIT_MESSAGE 'B'
#define DELETE_MESSAGE 'D'
#define LEAVE_CHAT 'E'
#define EXIT_APP 'Q'

//STATUS CODES
#define UNLOADED_STATUS "U"
#define LOADED_STATUS "L"
#define UNDELETED_STATUS "T"
#define DELETED_STATUS "D"

//OTHERS
#define LOGIN 'L'
#define SIGNUP 'R'

//CODES
#define WAIT_FOR_CODE "W"
#define OK_CODE "Y"
#define ERROR_CODE "N"
#define NO_DATA_CODE "E"
#define RECORD_EXISTS_CODE "J"

#define DB_ROWS_MAX 10000
#define MESSAGE_MAX_LEN 4096
#define KEY_LENGHT 4096
typedef unsigned char * code;


typedef struct s_send_param
{
	int socket;
    char *server_IP;
    char *server_port;
    SSL_CTX *ctx;
    SSL *ssl;
    int * cmdEXIT;
} t_send_param;

typedef struct s_message {
    int id;
    char *username;
    const char *message_text;
    GtkWidget *message_label;
} t_message;

//action functions
void log_in(void);
void create_account(void);
void show_settings(void);
void send_message();
void create_message_widget(char * message_query, bool to_end);
void find_user();
gboolean chat_enter_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean login_enter_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean signup_enter_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean enter_escape(GtkWidget *widget, GdkEventKey *event, gpointer data);
void change_chat_by_id(char * new_chat_id);
void scroll();
void change_chat(GtkListBox* self, GtkListBoxRow* row, gpointer data);
void set_text();
void edit_message(GtkWidget *widget, gpointer data);
//gboolean chat_actions_menu(GtkWidget *widget, GdkEventButton *event, gpointer data);

void collect_user_info(void * info);

void delete_all_history();
void clear_chat_list() ;
void renew_chat_list();
void apply_collocutor_info();

gboolean my_message_menu(GtkWidget *widget, GdkEventButton *event, t_message *data);
gboolean other_message_menu(GtkWidget *widget, GdkEventButton *event, t_message *data);
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

gboolean chat_actions_menu(GtkWidget *widget, GdkEventButton *event, gpointer data);
void delete_chat();
void create_chat_options_popover();

void load_css(int theme);
void save_user_theme(int theme);
void load_css_from_file();

char * correct_input(char * str);
char * restore_input(char * str);

//working with chats
void create_chat(char * chat_info_query);
void collect_messages(void * info);

void process_message_info(char * message_info);
void process_chat_info(char * chat_info);

typedef struct app_s
{
    GtkLabel *status;
    //login widgets
    GtkWidget *login_window;
    GtkWidget *signup_window;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;
    //signup widgets
    GtkWidget *signup_username_entry;
    GtkWidget *signup_password_entry;
    GtkWidget *signup_confirm_password_entry;
    //error hangling widgets
    GtkWidget *error_window;
    GtkWidget *error_label;
    GtkWidget *error_button;
    //chat widgets
    GtkWidget *chat_window;
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
    GtkWidget *welcome_message;
    GtkWidget *send_message_button;
    GtkWidget *chat_entry_box;
    GtkWidget *chat_entry;
    GtkWidget *find_user_entry;
    GtkWidget *theme_combobox;
    //widgets for right mouse click
    GtkWidget *my_options;
    GtkWidget *other_options;
    GtkWidget *chat_options;

    char *username;
    char *password;
    const char *username_t;

    GtkWidget *active_widget;
    char *active_message;
    char * current_chat_id; // HARDCODED IN APP_INIT!!!!!!!!!!!!!!!!!!!!!!
    //char * collocutor_username;
    bool edit_message;
}              app_t;

app_t *app_init();
extern app_t *app;
extern t_send_param *param;
extern t_message *message_data;

//CONECTION
int connect_to_server(t_send_param *param);
int ssl_connect(t_send_param *param);
SSL_CTX* initCTX(void);
void ShowCerts(SSL *ssl);
int u_recv(t_send_param *param, void* buf, int len);
int u_send(t_send_param *param, void* buf, int len);

//request utils
char * create_query_delim_separated(int count, ...);
int send_server_request(t_send_param *param, char * query);

#endif


