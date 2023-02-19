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

#include "../libraries/openssl/openssl/rsa.h"
#include "../libraries/openssl/openssl/evp.h"
#include "../libraries/openssl/openssl/pem.h"
#include "../libraries/openssl/openssl/sha.h"
#include "../libraries/libmx/inc/libmx.h"

typedef unsigned char * code;
#define QUERY_DELIM '@'

#define LOGIN 'L'
#define SIGNUP 'R'
#define SEND_MESSAGE 'S'
#define CREATE_CHAT 'C'
#define RENEW_CHAT 'A'
#define EDIT_MESSAGE 'B'
#define DELETE_MESSAGE 'D'
#define EXIT_CONVERSATION 'E'

#define DB_ROWS_MAX 10000
#define MESSAGE_MAX_LEN 10000
#define KEY_LENGHT 4096
typedef unsigned char * code;

typedef struct s_send_param
{
	int socket;
    int *cmdEXIT;
} t_send_param;

code create_query_delim_separated(int count, ...);
int send_server_request(t_send_param *param, code query);

#endif