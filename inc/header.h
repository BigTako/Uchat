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
#include <stdarg.h>

#define QUERY_DELIM '@'

#define LOGIN 'L'
#define SIGNUP 'R'
#define SEND_MESSAGE 'S'
#define CREATE_CHAT 'C'
#define RENEW_CHAT 'A'
#define EDIT_MESSAGE 'B'
#define DELETE_MESSAGE 'D'
#define EXIT_CONVERSATION 'E'


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
int sqlite_execute(sqlite3 * db, char * query);
char * to_sql_string(char * str);

//CRYPTOGRAPHY
code sha256_string(char *string);
EVP_PKEY * generate_key_pair();
code PRIVKEY_to_str(EVP_PKEY * keypair);
code PUBKEY_to_str(EVP_PKEY * keypair);
code encipher(code rsaPublicKeyChar, char * message, int * emLen, int * ekLen_v, code * ek_v, code * iv_v);
code decipher(code rsaPrivateKeyChar, code encryptedMessage, unsigned long emLen, code ek, code iv, int ekLen);

//SERVER DATABASE RELATIONSHIP UTILS
void create_account(sqlite3 * db, char * username, char * password);
int login(code db_hash, code input_password);
code create_network_query(int count, ...);
int validate_query(char * code, int delims_count, char * err_message);
void format_and_execute (sqlite3 * db, char * template, int count, ...);
#endif


