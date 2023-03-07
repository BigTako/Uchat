#include "../inc/header.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>

#define FAIL    -1
#define CETRIFS_FILENAME "mycert.pem"


#define SUCCESS_CODE "Y"
#define FAILURE_CODE "N"
#define NODATA_CODE "E"

// Create the SSL socket and intialize the socket address structure
int openListener(int port)
{
    int sd;
    struct sockaddr_in addr;
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Can't configure listening port");
        abort();
    }
    return sd;
}

/*int isRoot()
{
    if (getuid() != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}*/

SSL_CTX* InitServerCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

void ShowCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
    {
        printf("No certificates.\n");
    }
}

int skip_bytes(t_thread_param *param, int num_bytes_to_skip) {
    char buf[1024];
    int bytes_read = 0;
    int total_bytes_read = 0;
    while (total_bytes_read < num_bytes_to_skip) {
        int bytes_to_read = sizeof(buf);
        if (bytes_to_read > num_bytes_to_skip - total_bytes_read) {
            bytes_to_read = num_bytes_to_skip - total_bytes_read;
        }
        bytes_read = SSL_read(param->ssl, buf, bytes_to_read);
        if (bytes_read <= 0) {
            return -1;
        }
        total_bytes_read += bytes_read;
    }
    return 0;
}

int u_recv(t_thread_param *param, void* buf, int len) {
    int actualLen;
    if (SSL_read(param->ssl, &actualLen, sizeof(actualLen)) <= 0) 
    {
        return -1;
    }
    actualLen = ntohl(actualLen);
    if(actualLen > len) 
    {
        printf("WARNING: you received not all message\n");
        if (SSL_read(param->ssl, buf, len) <= 0) {
            return -1;
        }
        if (skip_bytes(param, actualLen - len) < 0) {
            return -1;
        }
    }
    else 
    {
        if (SSL_read(param->ssl, buf, actualLen) <= 0) 
        {
            return -1;
        }
    }
    if (SSL_write(param->ssl, OK_CODE, 2) <= 0) {
        return -1;
    }
    printf("[INFO] Successfully recv message.\n");
    return actualLen;
}

int u_send(t_thread_param *param, void* buf, int len) {
    int len_n = htonl(len);
    if (SSL_write(param->ssl, &len_n, sizeof(len_n)) <= 0) {
        return -1;
    }
    if (SSL_write(param->ssl, buf, len) <= 0) {
        return -1;
    }
    char response_buff[2];
    if (SSL_read(param->ssl, response_buff, 2) <= 0) 
    {
        return -1;
    }
    if (response_buff[0] == OK_CODE[0]) 
    {
        //printf("[INFO] u_send success).\n");
    }
    else if (response_buff[0] == NO_DATA_CODE[0]) 
    {
        printf("[INFO] No data to receive(%s)\n", response_buff);
    }
    else
    {
        printf("[ERROR] usend failure(Undefined package=%s)\n", response_buff);
        return -1;
    }
    return len;
}

/*int main(int argc, char ** argv) 
{
    if (argv[1] == NULL) 
    {
		mx_printerr("usage: ./userver [port]\n");
        exit(EXIT_FAILURE);
	}
    // //SQL DATABASE TABLES INITIALISATION
    // sqlite3 * db = NULL;
	// sqlite3_open("database.db", &db);
	
    // format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(\
    //                         username TEXT NOT NULL UNIQUE, \
    //                         password TEXT NOT NULL)", USERS_TN);

    // //creating table with conversations
    // format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(\
    //                         chat_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
    //                         chat_name TEXT NOT NULL,\
    //                         chat_members TEXT NOT NULL)", CHATS_TN);

    // //creating table with messages
	// format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(\
    //                         message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
    //                         from_username TEXT NOT NULL, \
    //                         message_text TEXT NOT NULL, \
    //                         send_datetime INTEGER NOT NULL, \
    //                         chat_id INTEGER NOT NULL, \
    //                         status TEXT NOT NULL DEFAULT 'unread')", MESSAGES_TN);
    SSL_CTX *ctx;
    // Initialize the SSL library
    SSL_library_init();
    ctx = InitServerCTX();        // initialize SSL 
    LoadCertificates(ctx, CETRIFS_FILENAME, CETRIFS_FILENAME);
    int welcomeSocket = openListener(atoi(argv[1]));

    pthread_mutex_t mutex_R;

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
        if(SSL_accept(param->ssl) == -1) {
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
        char buff[1000] = "привіт я сервер";
        u_send(param, buff, strlen(buff));
        u_recv(param, buff, 1000);
        printf("Отримано: %s\n", buff);
        //param->db = db;
        //pthread_t thread;
        //int status = pthread_create(&thread, NULL, client_thread, param);
        //if (status != 0) {
        //    printf("main error: can't create thread");
        //    exit(1);
        //}

    }
    //int status_addr;

    
    //status = pthread_join(thread, (void**)&status_addr);
    //if (status != 0) {
    //    printf("main error: can't join thread");
    //    exit(1);
    //}
    while(count_of_threads > 0);
    //sqlite3_close(db);
    close(welcomeSocket);
    SSL_CTX_free(ctx); 
    printf("bye\n");
    return 0;
}
*/
