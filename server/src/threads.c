#include "../inc/header.h"

bool s_to_c_info_exchange(t_thread_param *param, char ** table)
{
	if (!table || !*table)
	{
		printf("Nothing to search\n");
		if (send(param->socket, "N", 2, 0) <= 0)
		{
			printf("[ERROR] while sending NO to client\n");
		}
		return 1;
	}
	bool online = true;
	char query_buff[MESSAGE_MAX_LEN];
	int count_of_chats = mx_null_arr_len((char **)table);

	sprintf(query_buff, "%s%s%d", WAIT_FOR_CODE, QUERY_DELIM, count_of_chats);
	printf("taking chats query: %s\n", query_buff);
	if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) online = false; // send how many chats are in db(with user as member)
	printf("Send: %s\n", query_buff);
	memset(query_buff, '\0', strlen(query_buff));
	if (online == true) 
	{
		int status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0); // wait for responce , client sends when count of chats is delivered successfuly
		while (status == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
		{
			status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
		}
		if (status <= 0) online = false;
		printf("Recv: %s\n", query_buff);
	}
			
	if (query_buff[0] != 'Y') printf("RECIVED ABOBA\n"); // recived something wrong there are error in clients code!

	for (int i = 0; table[i]; i++) 
	{
		memset(query_buff, '\0', strlen(query_buff));
		if (online == true) 
		{
			sprintf(query_buff, "%s%s%s", MESSAGE_CODE, QUERY_DELIM, (char *)table[i]);
			if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) online = false; // send a messages with chats info one by one
			printf("Send: %s\n", query_buff);
	
			memset(query_buff, '\0', strlen(query_buff));
			if (online == true) 
			{
				int status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
				while (status == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
				{
					status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
				}
				if (status <= 0) online = false;
				printf("Recv: %s\n", query_buff);
			}
		}
	}
	if (send(param->socket, "Y", 2, 0) <= 0) online = false;
    printf("[INFO] Successfuly sent %d packages\n", count_of_chats);
	return online;
}

char *encode_login(char *code, t_thread_param *param, bool *online)
{
	if (!code || !param->db) return NULL;
	
	/*
		<---CODES--->
		L@NAME@PASSWORD_HASH - login
		R@NAME@PASSWORD_HASH - register
	*/
	printf("Login/signup query : %s\n", code);
	char ** table = NULL;
	//char query_buf[1000];
	char ** parts = mx_strsplit(code, QUERY_DELIM[0]);
	char code_num = parts[0][0];
	char *user = NULL;
	char * db_query = NULL;
	
	switch(code_num)
	{
		case LOGIN: // loging
			if (!validate_query(code, 2, "Loging query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "SELECT password FROM %s WHERE username='%s%s'";
				table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, USERS_TN, QUERY_DELIM, parts[1]);
				//printf("got password: %s, count of holders %d\n", table[0], HOLDERS(db_query));

				if (!table || !(*table)) // NO SUCH USERNAME IN DATABASE
				{
					printf("[ERROR] Invalid username or password\n");
					if (u_send(param, "N", 1) <= 0) *online = false;
				}
				else
				{
					if (!strcmp(table[0], parts[2]))
					{
						printf("[INFO] correct loging\n");
						user = mx_strjoin(QUERY_DELIM, parts[1]);
						*online = true;
						if (u_send(param, "Y", 1) <= 0) *online = false;
					}
					else
					{
						printf("[ERROR] Invalid username or password\n");
						if (u_send(param, "N", 1) <= 0) *online = false;	
					}
				}
				delete_table(&table);
			} else {
				if (u_send(param, "C", 1) <= 0) *online = false;
			}
			break;		
		case SIGNUP:
			if (!validate_query(code, 2, "SignUp query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "SELECT password FROM %s WHERE username='%s%s'";
				table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, USERS_TN, QUERY_DELIM, parts[1]);

				if (!table || !(*table))
				{
					db_query = "INSERT INTO %s(username, password) VALUES('%s%s', '%s')";
					format_and_execute(param->db, db_query, USERS_TN, QUERY_DELIM, parts[1], parts[2]);
					printf("[INFO] Successfuly signed up\n");
					*online = true;
					if (u_send(param, "Y", 1) <= 0) *online = false;
				}
				else
				{
					printf("[ERROR] Account with such username is already exists\n");
					if (u_send(param, "N", 1) <= 0) *online = false;
				}
				delete_table(&table);
			} else {
				if (u_send(param, "C", 1) <= 0) *online = false;
			}
			break;
	}
	mx_del_strarr(&parts);
	return user;
}


void encode(char * code, t_thread_param *param, bool *online, char *user)
{
	if (!code || !param->db) return;
	
	/*
		<---CODES--->
		S@TEXT@TIME@chat_ID - send message
		C@NAME@USERNAME1@USERNAME2@... - create new chat 
		A@chat_ID - renew chat
		B@MESSAGE_ID
		D@MESSAGE_ID
		E@chat_ID
		X
	*/
	printf("Encode code string: %s\n", code);
	char ** table = NULL;
	char ** parts = mx_strsplit(code, QUERY_DELIM[0]);
	char code_num = parts[0][0];
	char query_buff[MESSAGE_MAX_LEN];
	char * db_query = NULL;
	char * str_timestamp = NULL;
	char * new_members_str = NULL;
	char * members_str = NULL;
	int members_int = 0;
	int executing_status = 0;
	int newchat_id = 0;
	char * another_user = NULL;
	switch(code_num)
	{
		case SEND_MESSAGE: //S@TEXT@TIME@chat_ID - send message
			printf("[INFO] Want to send a message\n");
			break;
		case CREATE_CHAT: //C@NAME@USERNAME1@USERNAME2@... - create new chat 
			/*
				parts[0] - action
				parts[1] - chat name
				parts[2]...parts[n] - members
			*/
			printf("[INFO] Want to create a chat\n");
			break;
		case GET_CHATS_HISTORY: // GET LAST 20 MESSAGES IGNORING STATUS
			//message_id@from_username@message_text
			printf("[INFO] Want to get initial chat messages\n");
			break;
		case GET_NEW_MESSAGES: // CONTINUALY GET MESSAGES WITH UNREAD STATUS
			//M@message_id@from_username@message_text@send_datetime@chat_id
			printf("[INFO] Want to get messaeges with status unread\n");
			break;
		case GET_ALL_CHATS:
			printf("[INFO] Want to get current chats\n");
			break;
		case GET_NEW_CHATS:
			printf("[INFO] Want to get new chats\n");
			break;
		case EDIT_MESSAGE:
			printf("[INFO] Want to edit a message\n"); 
			break;
		case DELETE_MESSAGE:
			printf("[INFO] Want to delete a message\n");
			break;
		case EXIT_CONVERSATION:
			printf("[INFO] Want to exit a conversation\n");
			break;
	}
	mx_del_strarr(&parts);
}

void* client_thread(void* vparam) 
{
    t_thread_param *param = (t_thread_param*) vparam;
	*(param->count_of_threads)++;
    printf("---Start-recving-from-new-client---thread-N-%d---\n", *param->count_of_threads);
    bool online = true;
    char *user = NULL;
    char buffer[MESSAGE_MAX_LEN]; //!!!
    while (user == NULL) 
	{
		printf("WAIT\n");
		if (u_recv(param, buffer, MESSAGE_MAX_LEN) <= 0) {
            online = false;
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
        if(buffer[0] == LOGIN || buffer[0] == SIGNUP) {
			pthread_mutex_lock(param->mutex_R);
            user = encode_login(buffer, param, &online);
			pthread_mutex_unlock(param->mutex_R);
        } else {
            //send that u need to login first
        }
		memset(buffer, '\0', MESSAGE_MAX_LEN);
    }
	
	memset(buffer, '\0', MESSAGE_MAX_LEN);
	printf("got a username: %s\n", user);
    while (online) 
	{
        if (u_recv(param, buffer, MESSAGE_MAX_LEN) <= 0) 
		{
            online = false;
            //OFLINE
			printf("have to go\n");
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
		pthread_mutex_lock(param->mutex_R);
		encode(buffer, param, &online, user);
		pthread_mutex_unlock(param->mutex_R);
		memset(buffer, '\0', MESSAGE_MAX_LEN);
    }
	printf("DISCONNECTING User: %s\n", user);
	free(user);
    close(param->socket);
	int *count_of_threads = param->count_of_threads;
    free(param);
	*count_of_threads--;
    pthread_exit(0);
}

void* exit_thread(void* vparam) {
	t_thread_param *param = (t_thread_param*) vparam;
	//*param->count_of_threads++;
	printf("Print \"exit\" to terminal to exit)\n");
	struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI , 0};
    char string[10];
	while(1) {
		if( poll(&mypoll, 1, 10) ) {
        	scanf(" %[^\n]s", string);
        	if (mx_strcmp_ic(string, "exit")==0) {
            	*(param->cmdEXIT) = 1;
				break;
        	}
    	}
	}
	//int *count_of_threads = param->count_of_threads;
	free(param);
	//*count_of_threads--;
    pthread_exit(0);
}



