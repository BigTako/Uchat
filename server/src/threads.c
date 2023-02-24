#include "../inc/header.h"

bool s_to_c_info_exchange(t_thread_param *param, void ** table)
{
	bool online = true;
	char query_buff[MESSAGE_MAX_LEN];
	int count_of_chats = mx_null_arr_len(table);

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
			sprintf(query_buff, "%s%s%s", MESSAGE_CODE, QUERY_DELIM, table[i]);
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
	void *** table = NULL;
	char query_buf[1000];
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

				printf("got password: %s, count of holders %d\n", table[0], HOLDERS(db_query));

				if (!table || !(*table)) // NO SUCH USERNAME IN DATABASE
				{
					printf("[ERROR] Invalid username or password\n");
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;
				}
				else
				{
					if (!strcmp(table[0], parts[2]))
					{
						printf("[INFO] correct loging\n");
						user = mx_strjoin(QUERY_DELIM, parts[1]);
						*online = true;
						if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
					}
					else
					{
						printf("[ERROR] Invalid username or password\n");
						if (send(param->socket, "N", 1, 0) <= 0) *online = false;	
					}
				}
				delete_table(&table);
			} else {
				if (send(param->socket, "C", 1, 0) <= 0) *online = false;
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
					if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
				}
				else
				{
					printf("[ERROR] Account with such username is already exists\n");
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;
				}
				delete_table(&table);
			} else {
				if (send(param->socket, "C", 1, 0) <= 0) *online = false;
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
		S@TEXT@TIME@CONVERSATION_ID - send message
		C@NAME@USERNAME1@USERNAME2@... - create new chat 
		A@CONVERSATION_ID - renew chat
		B@MESSAGE_ID
		D@MESSAGE_ID
		E@CONVERSATION_ID
		X
	*/
	printf("Encode code string: %s\n", code);
	void ** table = NULL;
	char ** parts = mx_strsplit(code, QUERY_DELIM[0]);
	char code_num = parts[0][0];
	char query_buff[MESSAGE_MAX_LEN];
	char * db_query = NULL;
	char * str_timestamp = NULL;
	char * new_members_str = NULL;
	char * members_str = NULL;
	int members_int = 0;
	int executing_status = 0;
	int count_of_chats = 0;
	int newchat_id = 0;
	char * another_user = NULL;
	switch(code_num)
	{
		case SEND_MESSAGE: //S@TEXT@TIME@CONVERSATION_ID - send message
			if (!validate_query(code, 3, "Message sending query is wrong, incorrent delimiter count!\n"))
      		{
				db_query = "INSERT INTO %s(from_username, message_text, send_datetime, conversation_id) VALUES('%s', '%s', %s, %s)";
				str_timestamp = mx_itoa(time(NULL));
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, user, parts[1], str_timestamp , parts[3]);
				printf("Execution status = %d\n", executing_status);
				free(str_timestamp);
				if (executing_status != 0)
				{
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;				
				}
				else
				{
					if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
				}
			}
			break;
		case CREATE_CHAT: //C@NAME@USERNAME1@USERNAME2@... - create new chat 
			/*
				parts[0] - action
				parts[1] - chat name
				parts[2]...parts[n] - members
			*/

			members_int = mx_null_arr_len(parts + 2) + 1;
			members_str = create_query_delim_separated(2, user, code + strlen(parts[0]) + strlen(parts[1]) + 2);
				
			printf("Chat members: %s\n, count %d\n", members_str, members_int);
			if (char_count(members_str, QUERY_DELIM[0]) == members_int)
			{
				if (executing_status != 0)
				{
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;				
				}
				else
				{
					if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
				}
				printf("[INFO] Successfuly created chat\n");
			}
			else
			{
				printf("[ERROR] Wrong chat members string\n");	
				if (send(param->socket, "N", 1, 0) <= 0) *online = false;
			}
			free(members_str);
			break;
		case RENEW_CHAT:
			if (!validate_query(code, 1, "Chat renewing query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "SELECT * FROM %s WHERE conversation_id=%s ORDER BY message_id DESC";
				table = get_db_data_table(param->db, db_query, 6, LOAD_MESSAGES_COUNT, MESSAGES_TN, parts[1]);
				online = s_to_c_info_exchange(param, table);
				
				/*sprintf(query_buff, "%s%s%d", WAIT_FOR_CODE, QUERY_DELIM, mx_null_arr_len(table));
				if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) *online = false;
				printf("Send: %s\n", query_buff);
				memset(query_buff, '\0', MESSAGE_MAX_LEN);
				if (*online == true) {
					int status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
					while (status == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
						if(*param->cmdEXIT > 0) {
							*online = false;
							break;
						}
						status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
					}
					if (status <= 0) *online = false;
					printf("Recv: %s\n", query_buff);
				}
				if (query_buff[0] != 'Y') printf("RECIVED ABOBA\n"); // recived something wrong there are error in clients code!
				for (int i = 0; table[i]; i++) {
					memset(query_buff, '\0', MESSAGE_MAX_LEN);
					if (*online == true) {
						sprintf(query_buff, "%s%s%s", MESSAGE_CODE, QUERY_DELIM, table[i]);
						if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) *online = false;
						printf("Send: %s\n", query_buff);
						memset(query_buff, '\0', MESSAGE_MAX_LEN);
						if (*online == true) {
							int status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
							while (status == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
								if(*param->cmdEXIT > 0) {
									*online = false;
									break;
								}
								status = recv(param->socket, query_buff, MESSAGE_MAX_LEN, 0);
							}
							if (status <= 0) *online = false;
							printf("Recv: %s\n", query_buff);
						}
					}
				}*/
				delete_table(&table);
			}
			else
			{
				printf("RECIVED ABOBA\n"); // recived something wrong there are error in clients code!
			}
			break;
		case TAKE_CURRENT_CHATS:
			//parts[1] - username member of conversation
			db_query = "SELECT * FROM %s WHERE chat_members LIKE '%%%s%s%%'"; // SELECT * FROM CONVERSATIONS_TN WHERE chat_members LIKE '%username%';
			table = get_db_data_table(param->db, db_query, 3, DB_ROWS_MAX, CONVERSATIONS_TN, QUERY_DELIM, parts[1]);
			online = s_to_c_info_exchange(param, table);
			delete_table(&table);
			break;
		case EDIT_MESSAGE:
			 if (!validate_query(code, 2, "Message edition query is wrong, incorrent delimiter count!\n"))
			{
				printf("Message id(%s), new message text(%s)\n", parts[1], parts[2]);
				db_query = "UPDATE %s SET message_text='%s' WHERE message_id=%s";
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, parts[2], parts[1]);
				if (executing_status != 0)
				{
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;				
				}
				else
				{
					if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
				}
				printf("Execution status = %d\n", executing_status);
				printf("[INFO] Successfuly edited message with id %s\n", parts[1]);
				//send(param->socket, "Y", 1, 0);
			}
			break;
		case DELETE_MESSAGE:
			if (!validate_query(code, 1, "Message deleting query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "DELETE FROM %s WHERE message_id=%s";
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, parts[1]);
				if (executing_status != 0)
				{
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;				
				}
				else
				{
					if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
					printf("[INFO] Successfuly deleted message with id %s\n", parts[1]);
				}
			}
			break;
		case EXIT_CONVERSATION:
			if (!validate_query(code, 1, "Conversation exit query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "SELECT chat_members FROM %s WHERE conversation_id=%s";
				table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, CONVERSATIONS_TN, parts[1]);
				if (!table || !*table)
				{
					printf("[ERROR] Data not found\n");
				}
				else
				{
					printf("Members of chat with conv. ID(%s): %s\n", parts[1], table[0]);
					if (strstr(table[0], user)) // founded occurence @username
					{
						new_members_str = mx_replace_substr(table[0], user, "");
						printf("New members of chat are: %s\n", new_members_str);
						db_query = "UPDATE %s SET chat_members='%s' WHERE conversation_id=%s";
						executing_status = format_and_execute(param->db, db_query, CONVERSATIONS_TN, new_members_str, parts[1]);
						if (executing_status != 0)
						{
							printf("[ERROR] error while executing or formating query\n");
							if (send(param->socket, "N", 1, 0) <= 0) *online = false;				
						}
						else
						{
							printf("[INFO] Successfuly leaved chat with id %s\n", parts[1]);
							if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
						}
					}
					free(new_members_str);
				}
				delete_table(&table);
			}
			break;
	}
	mx_del_strarr(&parts);
}

void* client_thread(void* vparam) 
{
    t_thread_param *param = (t_thread_param*) vparam;
	*param->count_of_threads++;
    printf("---Start-recving-from-new-client---\n");
    bool online = true;
    char *user = NULL;
    char buffer[MESSAGE_MAX_LEN]; //!!!
    while (user == NULL) 
	{
		int status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
		while (status == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			if(*param->cmdEXIT > 0) {
				online = false;
				break;
			}
			status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
		}
        if (status <= 0) {
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
	printf("got a username: %s\n", user);
    while (online) {
		int status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
		while (status == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
		}
        if (status <= 0) {
            online = false;
            //OFLINE
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
		pthread_mutex_lock(param->mutex_R);
		encode(buffer, param, &online, user);
		pthread_mutex_unlock(param->mutex_R);
		memset(buffer, '\0', MESSAGE_MAX_LEN);
    }
	free(user);
    close(param->socket);
	int *count_of_threads = param->count_of_threads;
    free(param);
	*count_of_threads--;
    pthread_exit(0);
}

void* exit_thread(void* vparam) {
	t_thread_param *param = (t_thread_param*) vparam;
	*param->count_of_threads++;
	printf("Print \"exit\" to terminal to exit)\n");
	struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
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
	int *count_of_threads = param->count_of_threads;
	free(param);
	*count_of_threads--;
    pthread_exit(0);
}



