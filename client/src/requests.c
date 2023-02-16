#include "../inc/header.h"

int login(t_send_param *param, char *name, char *password) {
    int len = mx_strlen(name) + mx_strlen(password) + 4;
    char *r = mx_strnew(len);
	r = mx_strcat(r, "L@");
    r = mx_strcat(r, name);
    r = mx_strcat(r, "@");
	r = mx_strcat(r, password);
    if (send(param->socket, r, len, 0) <= 0) {
        mx_strdel(&r);
        return -1;
    }
    mx_strdel(&r);
    char a[1];
    if (recv(param->socket, a, 1, 0) <= 0) {
        return -1;
    }
    switch (a[0])
    {
    case 'Y':
        return 1;
        break;
    case 'N':
        return 0;
        break;
    case 'C':
        return -2;
        break;
    }
    return -2;
}

