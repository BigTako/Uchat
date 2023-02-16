#include "../inc/header.h"

code create_network_query(int count, ...)
{
	code query = mx_strnew(100000);
    va_list ptr;
    // Initializing argument to the
    // list pointer
    va_start(ptr, count);
	char delim = QUERY_DELIM;
    for (int i = 0; i < count; i++)
	{
		query = mx_strcat(query, va_arg(ptr, unsigned char *));
		if (i != count - 1)
		{
			query = mx_strcat(query, &delim);
		}
	}
    // Ending argument list traversal
	va_end(ptr);
	return query;
}

int login(t_send_param *param, char *name, char *password) {
    code r = create_network_query(3, "L", name, password);
    if (send(param->socket, r, strlen(r) + 4, 0) <= 0) {
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
        case 'N':
            return 0;
        case 'C':
            return -2;
        default:
            return -2;
    }
}

int signup(t_send_param *param, char *name, char *password) {
    code r = create_network_query(3, "R", name, password);
    if (send(param->socket, r, strlen(r) + 4, 0) <= 0) {
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
        case 'N':
            return 0;
        case 'C':
            return -2; 
        default:
            return -2;
    }
}

int start_chat()
{
    
}

