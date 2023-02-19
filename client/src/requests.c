#include "../inc/header.h"
#include <errno.h>
/*
    S@TEXT@CONVERSATION_ID - send message
	C@NAME@USERNAME1@USERNAME2@... - create new chat 
	F@USERNAME@CONVERSATION_ID - renew chat
	B@MESSAGE_ID
	D@MESSAGE_ID
	E@USERNAME@CONVERSATION_ID
*/

code create_query_delim_separated(int count, ...)
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

int send_server_request(t_send_param *param, code query)
{
    if (send(param->socket, query, strlen(query) + 1, 0) <= 0) {
        perror(errno);
        return -1;
    }
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






