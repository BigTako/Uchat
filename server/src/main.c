#include "../inc/header.h"
#include <errno.h>

int cmdEXIT;
int count_of_threads;

int main(int argc, char ** argv) 
{
    if (argv[1] == NULL) 
    {
		mx_printerr("usage: ./userver [port]\n");
        exit(EXIT_FAILURE);
	}
    //SQL DATABASE TABLES INITIALISATION
    sqlite3 * db = db_init();
    
    SSL_CTX *ctx;
    // Initialize the SSL library
    SSL_library_init();
    ctx = InitServerCTX();        /* initialize SSL */
    LoadCertificates(ctx, CETRIFS_FILENAME, CETRIFS_FILENAME);
    int welcomeSocket = openListener(atoi(argv[1]));
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(welcomeSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    pthread_mutex_t mutex_R;
    pthread_mutex_init(&mutex_R, NULL);

    cmdEXIT = 0;
    count_of_threads = 0;
    //creating of exit_thread to correct exit
    pthread_t e_thread;
    t_thread_param* param = (t_thread_param*) malloc(sizeof(t_thread_param));
    int status = pthread_create(&e_thread, NULL, exit_thread, param);
    if (status != 0) 
    {
        printf("main error: can't create thread");
        exit(1);
    }

    while (cmdEXIT == 0) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);

        int client = -1;
        while (cmdEXIT == 0) {
            struct timeval tv;
            tv.tv_sec = 1; // таймаут у секундах
            tv.tv_usec = 0;

            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(welcomeSocket, &rfds);

            int ret = select(welcomeSocket + 1, &rfds, NULL, NULL, &tv);
            if (ret == -1) {
                perror("select");
                exit(EXIT_FAILURE);
            } else if (ret == 0) {
                fflush(stdout);
                continue;
            }
            client = accept(welcomeSocket, (struct sockaddr *)&addr, &len);
            if (client == -1) {
                continue;
            }
            break;
        }
        if (client == -1) {
            continue;
        }
        t_thread_param* param = (t_thread_param*) malloc(sizeof(t_thread_param));
        param->ssl = SSL_new(ctx);              // get new SSL state with context 
        SSL_set_fd(param->ssl, client);
        if(SSL_accept(param->ssl) == -1) 
        {
            close(client);
            free(param);
            ERR_print_errors_fp(stderr);
            continue;
        }
        printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        param->socket = client;
        param->mutex_R = &mutex_R;
        param->db = db;
        pthread_t thread;
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
    while(count_of_threads > 0) {
        //printf("count_of_threads %d\n", count_of_threads);
    }
    sqlite3_close(db);
    close(welcomeSocket);
    SSL_CTX_free(ctx);
    printf("bye\n");
    return 0;
}

