#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "header.h"

typedef unsigned char * code;
#define QUERY_DELIM "@"

#define START_PAGE "0"

//GET REQUESTS
#define GET_CHATS "V"
#define GET_MESSAGES "T"
#define GET_COLLOCUTOR_INFO 'K'
#define GET_USER_INFO "H"

//POST REQUESTS
#define SEND_MESSAGE 'S'
#define CREATE_CHAT 'C'
#define EDIT_MESSAGE 'B'
#define DELETE_MESSAGE 'D'
#define LEAVE_CHAT 'E'
#define EXIT_APP 'Q'
#define RESET_MESSAGES_STATUS "G"
#define UPDATE_USER_INFO "A"

//STATUS CODES
#define ACTIVE_STATUS "A"
#define EDITED_STATUS "E"
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

#define CONNECTED 0
#define NO_LOGIN -1
#define NO_CONNECTED -2
#define LOST_CONNECT -3

#define AVATAR_PATH "../resources/icons/user_icons/%s.png"

#endif
