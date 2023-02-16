#include "../inc/header.h"

int main(int argc, char ** argv) {
    if (argc != 3)
    {
        mx_printerr("Usage: ./uclient <server IP> <server port>\n");
        return 0;
    }
    
    /*my god*/

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
    
    t_send_param *param;
    param->cmdEXIT = &cmdEXIT;
    param->socket = clientSocket;

    printf("%d", login(param, "MAKS", "admin"));
    close(clientSocket);
    return 0;
}




