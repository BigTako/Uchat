#include "../inc/header.h"
#include <errno.h>

int main(int argc, char ** argv) 
{
    printf("PID %d\n", getpid());
    if (argc != 3)
    {
        printf("Usage: ./userver <server IP> <server port>\n");
        return 0;
    }
    /*SQL DATABASE TABLES INITIALISATION*/
    sqlite3 * db = NULL;
	sqlite3_open("database.db", &db);
	
    format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(\
                            username TEXT NOT NULL UNIQUE, \
                            password TEXT NOT NULL)", USERS_TN);

    //creating table with conversations
    format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(\
                            conversation_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            name TEXT NOT NULL,\
                            chat_members TEXT NOT NULL)", CONVERSATIONS_TN);

    //creating table with messages
	format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(\
                            message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            from_username TEXT NOT NULL, \
                            message_text TEXT NOT NULL, \
                            send_datetime INTEGER NOT NULL, \
                            conversation_id INTEGER NOT NULL, \
                            status TEXT NOT NULL DEFAULT 'unread')", MESSAGES_TN);

    /*GENERATION OF RSA KEYS*/
    EVP_PKEY * key = generate_key_pair();
    code privateKeyChar = PRIVKEY_to_str(key);
    code publicKeyChar = PUBKEY_to_str(key);
    EVP_PKEY_free(key);

    int welcomeSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(fcntl(welcomeSocket, F_SETFL, fcntl(welcomeSocket, F_GETFL) | O_NONBLOCK) < 0) {
        printf("ERROR cant Put welcomeSocket in non-blocking mode\n");
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    int status_ = bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(status_ < 0) {
        printf("ERROR in bind %d\n", errno);
        printf("====== \n");
        exit(0);
    }

    if (listen(welcomeSocket,5)==0)
    {
        printf("Listening\n");
    }
    else
    {
        printf("Error\n");
    }

    addr_size = sizeof serverStorage;

    pthread_mutex_t mutex_R;

    int cmdEXIT = 0;
    int count_of_threads = 0;
    //creating of exit_thread to correct exit
    pthread_t e_thread;
    t_thread_param* param = (t_thread_param*) malloc(sizeof(t_thread_param));
    param->cmdEXIT = &cmdEXIT;
    param->count_of_threads = &count_of_threads;
    int status = pthread_create(&e_thread, NULL, exit_thread, param);
    if (status != 0) {
        printf("main error: can't create thread");
        exit(1);
    }

    while (cmdEXIT == 0) {
        int Client = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
        while (Client == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			if(cmdEXIT > 0) {
				break;
			}
			Client = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
		}
        if (Client == -1) {
            break;
        }
        // Put the socket in non-blocking mode:
        if(fcntl(Client, F_SETFL, fcntl(Client, F_GETFL) | O_NONBLOCK) < 0) {
            printf("ERROR cant Put the socket in non-blocking mode\n");
        }
        pthread_t thread;
        t_thread_param* param = (t_thread_param*) malloc(sizeof(t_thread_param));

        param->cmdEXIT = &cmdEXIT;
        param->count_of_threads = &count_of_threads;
        param->socket = Client;
        param->mutex_R = &mutex_R;
        param->db = db;
        int status = pthread_create(&thread, NULL, client_thread, param);
        if (status != 0) {
            printf("main error: can't create thread");
            exit(1);
        }
    }
    //int status_addr;

    
    //status = pthread_join(thread, (void**)&status_addr);
    //if (status != 0) {
    //    printf("main error: can't join thread");
    //    exit(1);
    //}
    while(count_of_threads > 0);
    free(privateKeyChar);
    free(publicKeyChar);
    sqlite3_close(db);
    close(welcomeSocket);
    printf("bye\n");
    return 0;
}

