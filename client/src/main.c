#include "../inc/header.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char ** argv) {
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

    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

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
        printf("Enter action(L - login, R - signup): ");
	    scanf("%s", action);
	
	    printf("Enter username and password(space separated): ");
	    scanf("%s %s", username, password);
        switch(action[0])
        {
            case 'L':
                server_query = create_network_query(3, "L", username, password); // have to store a hash password
                online = send_server_request(param, server_query);
                free(server_query);
                break;
            case 'R':
                server_query = create_network_query(3, "R", username, password); // have to store a hash password
                online = send_server_request(param, server_query);
                free(server_query);
                break;
        }
    }while(!online || online < 0);
    
    do
    {
        /*
            S@TEXT@CONVERSATION_ID - send message
            C@NAME@USERNAME1@USERNAME2@... - create new chat 
            F@USERNAME@CONVERSATION_ID - renew chat
            B@MESSAGE_ID - exit message
            D@MESSAGE_ID - delete message
            E@USERNAME@CONVERSATION_ID - exit conversation(delete myself from conversation)
        */
        printf("Enter action:\nS - send message\n\
                               C - create new chat\n\
                               F - renew chat\n\
                               B - edit message\n\
                               D - delete message\n\
                               E - exit conversation\n\
                               Q - leave app\n");
	    scanf("%s", action);

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
                
        switch(action[0])
        {
            case SEND_MESSAGE: 
                //S@TEXT@TIME@CONVERSATION_ID    
                //server_query = create_network_query(3, "L", username, password); // have to store a hash password
                //send_server_request(param, server_query);
                //free(server_query);
                printf("Enter conversation_id and message(space separated): ");
	            scanf("%s %[^\n]", conversation_id, message);
                server_query = create_network_query(4, "S", message, mx_itoa((time(NULL))), conversation_id); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case CREATE_CHAT: // create chat
                //C@NAME@USERNAME1@USERNAME2@... - create new chat
                printf("Enter conversation_name and chat members(%c separated): ", QUERY_DELIM);
	            scanf("%s %s", chat_name, chat_members);
                server_query = create_network_query(3, "C", chat_name, chat_members); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);
                break;
            case RENEW_CHAT: //  renew chat
                /*server_query = create_network_query(3, "R", username, password); // have to store a hash password
                send_server_request(param, server_query);
                free(server_query);*/
                break;
            case EDIT_MESSAGE:
                break;
            case DELETE_MESSAGE:
                break;
            case EXIT_CONVERSATION:
                break; 
        }
    }while(action[0] != 'Q');

    close(clientSocket);
    free(param);
    return 0;
}




