#include "../inc/header.h"
#include <errno.h>

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

    pthread_mutex_t mutex_R;
    pthread_mutex_init(&mutex_R, NULL);

    int cmdEXIT = 0;
    int count_of_threads = 0;
    //creating of exit_thread to correct exit
    pthread_t e_thread;
    t_thread_param* param = (t_thread_param*) malloc(sizeof(t_thread_param));
    param->cmdEXIT = &cmdEXIT;
    param->count_of_threads = &count_of_threads;
    int status = pthread_create(&e_thread, NULL, exit_thread, param);
    if (status != 0) 
    {
        printf("main error: can't create thread");
        exit(1);
    }

    while (cmdEXIT == 0) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client = accept(welcomeSocket, (struct sockaddr *)&addr, &len);
        while (client == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			if(cmdEXIT > 0) {
				break;
			}
			client = accept(welcomeSocket, (struct sockaddr *)&addr, &len);
		}
        if (client == -1) {
            printf("[ERROR]\n");
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
        param->cmdEXIT = &cmdEXIT;
        param->count_of_threads = &count_of_threads;
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
    while(count_of_threads > 0);
    sqlite3_close(db);
    close(welcomeSocket);
    SSL_CTX_free(ctx);
    printf("bye\n");
    return 0;
}



