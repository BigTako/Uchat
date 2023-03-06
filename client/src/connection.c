#include "../inc/header.h"

int connect_to_server(t_send_param *param) 
{
    struct sockaddr_in serverAddr;
    param->socket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(param->server_port));
    serverAddr.sin_addr.s_addr = inet_addr(param->server_IP);
    return connect(param->socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
}
int ssl_connect(t_send_param *param) {
    param->ssl = SSL_new(param->ctx);      /* create new SSL connection state */
    SSL_set_fd(param->ssl, param->socket);    /* attach the socket descriptor */
    if ( SSL_connect(param->ssl) == -1 ) { /* perform the connection */
        ERR_print_errors_fp(stderr);
        return 1;
    }
    else
    {
        printf("\n\nConnected with %s encryption\n", SSL_get_cipher(param->ssl));
        //ShowCerts(ssl);        /* get any certs */
    }
    return 0;
}

SSL_CTX* initCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   // Create new context SSL_CTX_free()!!!
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

int skip_bytes(t_send_param *param, int num_bytes_to_skip) {
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

int u_recv(t_send_param *param, void* buf, int len) {
    int actualLen;
    if (SSL_read(param->ssl, &actualLen, sizeof(actualLen)) <= 0) {
        return -1;
    }
    actualLen = ntohl(actualLen);
    if(actualLen > len) {
        printf("WARNING: you received not all message\n");
        if (SSL_read(param->ssl, buf, len) <= 0) {
            return -1;
        }
        if (skip_bytes(param, actualLen - len) < 0) {
            return -1;
        }
    }
    else {
        if (SSL_read(param->ssl, buf, actualLen) <= 0) {
            return -1;
        }
    }
    if (SSL_write(param->ssl, "Y", 2) <= 0) {
        return -1;
    }
    printf("[INFO] Successfully recv message.\n");
    return actualLen;
}

int u_send(t_send_param *param, void* buf, int len) {
    int len_n = htonl(len);
    if (SSL_write(param->ssl, &len_n, sizeof(len_n)) <= 0) {
        return -1;
    }
    if (SSL_write(param->ssl, buf, len) <= 0) {
        return -1;
    }
    char response_buff[2];
    if (SSL_read(param->ssl, response_buff, 2) <= 0) {
        return -1;
    }
    if (response_buff[0] == 'Y') {
        printf("[INFO] Successfully sent message.\n");
    }
    else {
        printf("[ERROR] Undefined package.\n");
        return -1;
    }
    return len;
}


/*int main(int argc, char * argv[])
{
    if ( argc != 3 )
    {
        printf("usage: %s <server IP> <portnum>\n", argv[0]);
        exit(0);
    }
    param = malloc(sizeof(t_send_param));
    param->server_IP = argv[1];
    param->server_port = argv[2];
    printf("Input params: %s %s\n", param->server_IP, param->server_port);
    SSL_library_init();
    param->ctx = initCTX();
    printf("Connecting");
    while (connect_to_server(param) < 0);
    ssl_connect(param);
    printf("\nSUCCES\n");

    char acClientRequest[1000];
    char buff[1000];
    u_recv(param, buff, 1000);
    printf("Отримано: %s\n", buff);
    char buff2[1000] = "привіт я клієнт";
    u_send(param, buff2, strlen(buff2));
    close(param->socket);
    SSL_CTX_free(param->ctx);
    return 0;
}*/



