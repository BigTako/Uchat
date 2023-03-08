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
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <resolv.h>

#include "../libraries/openssl/openssl/ssl.h"
#include "../libraries/openssl/openssl/err.h"

#include "../libraries/Sqlite3/sqlite3.h"
#include "../libraries/libmx/inc/libmx.h"
//#include <poll.h>

#define LOAD_MESSAGES_COUNT 20
#define DB_ROWS_MAX 10000
#define MESSAGE_MAX_LEN 10000
#define KEY_LENGHT 4096

#define QUERY_DELIM "@"

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
#define EXIT_CONVERSATION 'E'
#define GO_ONLINE "O"
#define GO_OFFLINE "P"

//OTHERS
#define LOGIN 'L'
#define SIGNUP 'R'

//CODES
#define WAIT_FOR_CODE "W"
#define OK_CODE "Y"
#define ERROR_CODE "N"
#define NO_DATA_CODE "E"
#define RECORD_EXISTS_CODE "J"

//DATABASE TABLES NAMES
#define USERS_TN "users"
#define MESSAGES_TN "messages"
#define CHATS_TN "chats"
#define DB_FILENAME "database.db"
#define CETRIFS_FILENAME "mycert.pem"

typedef unsigned char * code;

typedef struct s_thread_param
{
	sqlite3 *db;
    int socket;
    SSL_CTX *ctx;
    SSL *ssl;
    int *cmdEXIT;
    pthread_mutex_t *mutex_R;
    int *count_of_threads;
} t_thread_param;

//DATABASE UTILS
sqlite3 * db_init();
int count_placeholders(char * str, char c);
void clear_inner_list(void * ptr);
void delete_table(char *** table);
char * mx_strstr_front(const char *haystack, const char *needle);
t_list * get_db_data_list(sqlite3 * db, char * selection_query, int cols_count);

//CRYPTOGRAPHY
code sha256_string(char *string);

//SERVER DATABASE RELATIONSHIP UTILS
char ** get_db_data_table(sqlite3 * db, char * template, int colums, int rows, ...);
char * create_query_delim_separated(int count, ...);
int validate_query(char * code, int delims_count, char * err_message);
int format_and_execute (sqlite3 * db, char * template, ...);

//SERVER UTILS
void* client_thread(void* vparam);
void* exit_thread(void* vparam);
char *encode_login(char *code, t_thread_param *param, bool *online);
void encode(char * code, t_thread_param *param, bool *online, char *userID);

//CONNECTION
int openListener(int port);
SSL_CTX* InitServerCTX(void);
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);
void ShowCerts(SSL* ssl);
int u_recv(t_thread_param *param, void* buf, int len);
int u_send(t_thread_param *param, void* buf, int len);

#endif

