#include "../inc/header.h"

app_t *app;
t_send_param *param;

int main(int argc, char * argv[]) {
    printf("PID %d\n", getpid());
    if (argc != 3)
    {
        mx_printerr("Usage: ./uclient <server IP> <server port>\n");
        return 0;
    }
    //char buffer[1024];
  
    app = app_init();
   
    int cmdEXIT = 0;
    //int status_addr;
    param = malloc(sizeof(t_send_param*));
    param->server_IP = argv[1];
    param->server_port = argv[2];
    printf("Input params: %s %s\n", param->server_IP, param->server_port);

    printf("Connecting");
    while (connect_to_server(param) < 0);
    printf("\nSUCCES\n");
    
    gtk_init(&argc, &argv);

    show_login_form();

    gtk_main();
    

    return 0;
}




