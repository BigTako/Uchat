#ifndef HEADER_H
#define HEADER_H
#include "../libmx/inc/libmx.h"
#include "../Sqlite3/sqlite3.h"
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdbool.h>

#define LOGIN 'L'
#define SIGNUP 'R'
#define SEND_MESSAGE 'S'
#define CREATE_CHAT 'C'
#define RENEW_CHAT 'F'
#define USERS_TN "users"
#define MESSAGES_TN "messages"
#define GROUP_MEMBERS_TN "gropMembers"
#define DB_ROWS_MAX 10000

#define MESSAGE_MAX_LEN 10000000
#define KEY_LENGHT 4096

typedef unsigned char * code;

//DATABASE UTILS
void clear_inner_list(void * ptr);
void delete_table(void **** table);
char * mx_strstr_front(const char *haystack, const char *needle);
char * execute_query(sqlite3 * db, char * query);
t_list * get_db_data_list(sqlite3 * db, char * selection_query, int cols_count);
void *** get_db_data_vector(sqlite3 * db, char * selection_query, int cols_count);
char * format_query(char * template, t_list * values);

//CRYPTOGRAPHY
code sha256_string(char *string);
#endif


