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

				printf("got password: %s, count of holders %d\n", table[0], HOLDERS(db_query));

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
			if (!validate_query(code, 3, "Message sending query is wrong, incorrent delimiter count!\n"))
      		{
				db_query = "INSERT INTO %s(from_username, message_text, send_datetime, chat_id) VALUES('%s', '%s', %s, %s)";
				str_timestamp = mx_itoa(time(NULL));
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, user, parts[1], str_timestamp , parts[3]);
				printf("Execution status = %d\n", executing_status);
				free(str_timestamp);
				
				db_query = "SELECT max(message_id) FROM %s";
				table = get_db_data_table(param->db, db_query, 1, 1, MESSAGES_TN);

				if (executing_status != 0)
				{
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;				
				}
				else
				{
					if (send(param->socket, table[0], strlen(table[0]) + 1, 0) <= 0) *online = false;
				}
				delete_table(&table);
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
				if (char_count(members_str, QUERY_DELIM[0]) == 2) // dialog
				{
					if (!strcmp(user, parts[2]))
						another_user = parts[3];    
					else
						another_user = parts[2];

					db_query = "SELECT 1 FROM %s WHERE username='%s%s'";
					table = get_db_data_table(param->db, db_query, 1, 1, USERS_TN, QUERY_DELIM, another_user);

					if (!table || !*table)
					{
						printf("[ERROR] No such user\n");
						//free(members_str);
						if (send(param->socket, "N@No such user", 1000, 0) <= 0) *online = false;
						free(members_str);
						break;
					}
					else
					{
						printf("[INFO] User exists\n");
						delete_table(&table);
						db_query = "SELECT chat_id \
									FROM %s \
									WHERE chat_members LIKE '%%%s%%' AND chat_members LIKE '%%%s%%'";
	
						table = get_db_data_table(param->db, db_query, 1, 1, CHATS_TN, user, another_user);

						if (table && *table)
						{
							printf("[INFO] Chat already exist\n");
							memset(query_buff, '\0', strlen(query_buff));
							sprintf(query_buff, "I@%s", table[0]);
							if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) *online = false;
							delete_table(&table);
							free(members_str);
							return;
						}	
					}
				}
				db_query = "SELECT max(chat_id) FROM %s";
				table = get_db_data_table(param->db, db_query, 1, 1, CHATS_TN);
				printf("problem is in here\n");
				db_query = "INSERT INTO %s VALUES(%s + 1, '%s', '%s')";
				
				if (!table || !*table) // table is empty
				{
					printf("[INFO] Table is empty, max conv_id is 0\n");
					newchat_id = 1;
					executing_status = format_and_execute(param->db, db_query, CHATS_TN, "0", parts[1], members_str);
				}
				else
				{	
					printf("[INFO] Got max chats id: %s\n", table[0]);
					newchat_id += atoi(table[0]) + 1;
					executing_status = format_and_execute(param->db, db_query, CHATS_TN, table[0], parts[1], members_str);
				}
				memset(query_buff, '\0', strlen(query_buff));
				if (executing_status != 0)
				{
					if (send(param->socket, "0", 1, 0) <= 0) *online = false;				
				}
				else
				{
					//query: M@chat_id@chat_name@LM_from_username@LM_message_text@LM_message_status@chat_members
					sprintf(query_buff, "%s%s%d%s%s%sYou%s...%sread%s", MESSAGE_CODE, QUERY_DELIM, newchat_id, QUERY_DELIM, parts[1], QUERY_DELIM, QUERY_DELIM, QUERY_DELIM, members_str);
					//sprintf(query_buff, "%s%s%d%s", MESSAGE_CODE, QUERY_DELIM, newchat_id, members_str);
					printf("Sending responce(%s) to user\n", query_buff);
					if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) *online = false;
				}
				printf("[INFO] Successfuly created chat\n");
				if (table)
				{
					delete_table(&table);
				}
			}
			else
			{
				printf("[ERROR] Wrong chat members string\n");	
				if (send(param->socket, "N", 1, 0) <= 0) *online = false;
			}
			free(members_str);
			break;
		case GET_CHATS_HISTORY: // GET LAST 20 MESSAGES IGNORING STATUS
			//message_id@from_username@message_text
			db_query = "SELECT * \
						FROM (SELECT m.message_id, m.from_username, m.message_text, m.send_datetime, m.chat_id \
							  FROM %s c \
							  INNER JOIN %s m \
							  ON c.chat_id = m.chat_id \
							  WHERE chat_members LIKE '%%%s%%' AND m.chat_id=%s \
							  ORDER BY m.message_id DESC) \
						ORDER BY message_id";

			table = get_db_data_table(param->db, db_query, 5, DB_ROWS_MAX, CHATS_TN, MESSAGES_TN, user, parts[1]);

			//UPDATE status of messages have got
			*online = s_to_c_info_exchange(param, table);
			if (*online && table && *table)
			{
				for (int i = 0; table[i]; i++)
				{
					db_query = "UPDATE %s SET status='read' WHERE message_id=%s";
					executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, strtok(table[i], QUERY_DELIM));
					printf("Execution status = %d\n", executing_status);
				}
			}
			delete_table(&table);
			break;
		case GET_NEW_MESSAGES: // CONTINUALY GET MESSAGES WITH UNREAD STATUS
			//M@message_id@from_username@message_text@send_datetime@chat_id
			db_query = "SELECT * \
						FROM (SELECT m.message_id, m.from_username, m.message_text, m.send_datetime, m.chat_id \
							  FROM %s c \
							  INNER JOIN %s m \
							  ON c.chat_id = m.chat_id \
							  WHERE m.frop_username != '%s' AND chat_members LIKE '%%%s%%' AND m.chat_id=%s AND m.status='unread'\
							  ORDER BY m.message_id DESC) \
						ORDER BY message_id";
			
			table = get_db_data_table(param->db, db_query, 5,   DB_ROWS_MAX, 
																CHATS_TN, 
																MESSAGES_TN, 
																user,
																user,
																parts[1]);
			*online = s_to_c_info_exchange(param, table);
			if (*online && table && *table)
			{
				for (int i = 0; table[i]; i++)
				{
					db_query = "UPDATE %s SET status='read' WHERE message_id=%s";
					executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, strtok(table[i], QUERY_DELIM));
				}
			}
			delete_table(&table);
			break;
		case GET_CURRENT_CHATS:
			db_query = "SELECT  c.chat_id, \
								c.chat_name, \
								CASE (SELECT max(message_id) FROM %s m WHERE m.chat_id=c.chat_id) IS NULL \
									WHEN 1 THEN 'You'||' @ '||'...'||'@'||'...' \
									ELSE (SELECT m3.from_username || '@' || m3.message_text || '@' || m3.status  \
									FROM %s m3 WHERE m3.message_id=(SELECT max(message_id) FROM %s m2 WHERE m2.chat_id=c.chat_id)) \
        						END MESSAGE_DATA \
        						, \
								c.chat_members \ 
						FROM %s c \
						WHERE chat_members LIKE '%%%s%%'";

			table = get_db_data_table(param->db, db_query, 4, DB_ROWS_MAX, MESSAGES_TN, MESSAGES_TN, MESSAGES_TN, CHATS_TN, user);
			if (table && *table)
			{
				printf("Have something to work with\n");
				*online = s_to_c_info_exchange(param, table);
			}
			else
			{
				printf("Nothing to handle\n");
				if (send(param->socket, "N", 1, 0) <= 0) *online = false;		
			}
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
			if (!validate_query(code, 1, "chat exit query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "SELECT chat_members FROM %s WHERE chat_id=%s";
				table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, CHATS_TN, parts[1]);
				if (!table || !*table)
				{
					printf("[ERROR] Data not found\n");
				}
				else
				{
					printf("Members of chat with conv. ID(%s): %s\n", parts[1], (char *)table[0]);
					if (strstr(table[0], user)) // founded occurence @username
					{
						new_members_str = mx_replace_substr(table[0], user, "");
						printf("New members of chat are: %s\n", new_members_str);
						db_query = "UPDATE %s SET chat_members='%s' WHERE chat_id=%s";
						executing_status = format_and_execute(param->db, db_query, CHATS_TN, new_members_str, parts[1]);
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
	*(param->count_of_threads)++;
    printf("---Start-recving-from-new-client---thread-N-%d---\n", *param->count_of_threads);
    bool online = true;
    char *user = NULL;
    char buffer[MESSAGE_MAX_LEN]; //!!!
    while (user == NULL) 
	{
		printf("WAIT\n");
		if (u_recv(param, buffer, MESSAGE_MAX_LEN) <= 0) {
			printf("hmm\n");
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
    while (online) 
	{
        if (u_recv(param, buffer, MESSAGE_MAX_LEN) <= 0) 
		{
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



