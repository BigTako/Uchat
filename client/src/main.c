#include "../inc/header.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char ** argv) {
    printf("PID %d\n", getpid());
    if (argc != 3)
    {
        mx_printerr("Usage: ./uclient <server IP> <server port>\n");
        return 0;
    }

    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    addr_size = sizeof serverAddr;

    int status = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    if (status < 0) {
        printf("ERROR in connect %d\n", errno);
        printf("====== \n");
        exit(0);
    }

    int cmdEXIT = 0;
    int status_addr;
    
    t_send_param *param = malloc(sizeof(t_send_param*));
    param->cmdEXIT = &cmdEXIT;
    param->socket = clientSocket;

    char username[10000];
	char password[10000];
	char action[2];
    int online = 0;
    char * server_query = NULL;
    do
    {
        printf("Enter action(L - login, R - signup, Q - exit): ");
	    scanf("%s", action);
        if (action[0] == 'Q') {
            cmdEXIT = 1;
            break;
        }
	
	    printf("Enter username and password(space separated): ");
	    scanf("%s %s", username, password);

        switch(action[0])
        {
            case 'L':
                server_query = create_query_delim_separated(3, "L", username, password); // have to store a hash password
                printf("Enter action(L - login, R - signup): ");
                online = send_server_request(param, server_query);
                if (online < 0) {
                    perror(errno);
                }
                free(server_query);
                break;
            case 'R':
                server_query = create_query_delim_separated(3, "R", username, password); // have to store a hash password
                online = send_server_request(param, server_query);
                if (online < 0) {
                    perror(errno);
                }
                free(server_query);
                break;
        }
    }while(!online || online < 0);
    


    do
    {
        if(cmdEXIT > 0) {
            break;
        }
        /*
            S@TEXT@CONVERSATION_ID - send message
            C@NAME@USERNAME1@USERNAME2@... - create new chat 
            F@USERNAME@CONVERSATION_ID - renew chat
            B@MESSAGE_ID - exit message
            D@MESSAGE_ID - delete message
            E@CONVERSATION_ID - exit conversation(delete myself from conversation)
        */
        printf("S - send message\n\
                C - create new chat\n\
                A - renew chat\n\
                B - edit message\n\
                D - delete message\n\
                E - exit conversation\n\
                Q - leave app\n\
                Enter action: ");

	    scanf("%s", action);
        if (action[0] == 'Q') {
            cmdEXIT = 1;
            break;
        }

        /*
            #define SEND_MESSAGE 'S'
            #define CREATE_CHAT 'C'
            #define RENEW_CHAT 'A'
            #define EDIT_MESSAGE 'B'
            #define DELETE_MESSAGE 'D'
            #define EXIT_CONVERSATION 'E'
        */
        char message[10000];
        char conversation_id[1000];
        char chat_name[1000];
        char chat_members[10000];
        char message_id[1000];

        switch(action[0])
        {
            case SEND_MESSAGE: 
                //S@TEXT@TIME@CONVERSATION_ID    
                //server_query = create_query_delim_separated(3, "L", username, password); // have to store a hash password
                //send_server_request(param, server_query);
                //free(server_query);
                printf("Enter conversation_id and message(space separated): ");
	            scanf("%s %[^\n]", conversation_id, message);
                server_query = create_query_delim_separated(4, action, message, mx_itoa((time(NULL))), conversation_id); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case CREATE_CHAT: // create chat
                //C@NAME@USERNAME1@USERNAME2@... - create new chat
                printf("Enter conversation_name and chat members(%suser1%suser2%suser3...): ", QUERY_DELIM , QUERY_DELIM, QUERY_DELIM);
	            scanf("%s %s", chat_name, chat_members);
                server_query = create_query_delim_separated(3, action, chat_name, chat_members+1); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case RENEW_CHAT: //  renew chat
                //F@CONVERSATION_ID
                printf("Enter conversation ID: ");
	            scanf("%s", chat_name);
                server_query = create_query_delim_separated(2, action, chat_name); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case EDIT_MESSAGE:
                printf("Enter message ID and new content: ");
	            scanf("%s %[^\n]", message_id, message);
                server_query = create_query_delim_separated(3, action, message_id, message); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case DELETE_MESSAGE:
                printf("Enter message ID: ");
	            scanf("%s", message_id);
                server_query = create_query_delim_separated(2, action, message_id); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case EXIT_CONVERSATION:
                printf("Enter conversation ID: ");
	            scanf("%s", conversation_id);
                server_query = create_query_delim_separated(2, action, conversation_id); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
        }
    }while(action[0] != 'Q');

    send(clientSocket, "X", 1, 0);
    close(clientSocket);
    free(param);
    return 0;
}




