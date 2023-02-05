#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "../inc/header.h"
#include <arpa/inet.h>

int compare_strings(char a[], char b[])
{
    int c = 0;
    while (a[c] == b[c]) 
    {
        if (a[c] == '\0' || b[c] == '\0')
        break;
        c++;
    }
    if (a[c] == '\0' && b[c] == '\0')
    return 0;
    else
    return -1;
}

int main(int argc, char ** argv) 
{
    if (argc != 3)
    {
        printf("Usage: ./userver <server IP> <server port>\n");
        return 0;
    }

    int welcomeSocket, Client1, Client2;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    char buffer[1024];

    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (listen(welcomeSocket,5)==0)
    {
        printf("Listening\n");
    }
    else
    {
        printf("Error\n");
    }

    addr_size = sizeof serverStorage;
    Client1 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    Client2 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

    int cmdEXIT = 0;
    while (cmdEXIT == 0)
    {
        fflush(stdin);
        fflush(stdout);   
        recv(Client1, buffer, 1024, 0);
        printf ("Got a message(%s) from user1, sending to user2\n", buffer);
        send(Client2,buffer,1024,0);
        if (compare_strings(buffer, "exit")==0)
        {   
            cmdEXIT = 1;
        }
        else 
        {
            memset(&buffer[0], 0, sizeof(buffer));
            recv(Client2, buffer, 1024, 0);
            printf ("Sending a responce(%s) to client1\n", buffer);
            send(Client1,buffer,1024,0);
            if (compare_strings(buffer, "exit")==0)
            {
                cmdEXIT = 1;
            }
        }
        fflush(stdin);
        fflush(stdout);
    }

    return 0;
}

