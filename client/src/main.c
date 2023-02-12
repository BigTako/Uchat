#include "../inc/header.h"

app_t *app;

int main(int argc, char * argv[]) {
    app = app_init();

    if (argc != 3) {
        mx_printerr("usage: ./uchat [IP] [PORT]\n");
        return 0;
    }
    
    gtk_init(&argc, &argv);

    show_login_form();

    gtk_main();

    return 0;
}
