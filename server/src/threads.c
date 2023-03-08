#include "../inc/header.h"

bool s_to_c_info_exchange(t_thread_param *param, char ** table)
{
	char error_buff[1000];
	if (!table || !*table)
	{
		return 1;
	}
	bool online = true;
	char query_buff[MESSAGE_MAX_LEN];
	int count_of_chats = mx_null_arr_len((char **)table);

	sprintf(query_buff, "%s%s%d", WAIT_FOR_CODE, QUERY_DELIM, count_of_chats);
	printf("Send: %s\n", query_buff);
	u_send(param, query_buff, strlen(query_buff) + 1);
	memset(query_buff, '\0', strlen(query_buff));

	for (int i = 0; table[i]; i++) 
	{
		memset(query_buff, '\0', strlen(query_buff));
		if (online == true) 
		{
			sprintf(query_buff, "%s%s%s", OK_CODE, QUERY_DELIM, (char *)table[i]);
			printf("Send: %s\n", query_buff);
			u_send(param, query_buff, strlen(query_buff) + 1);
			memset(query_buff, '\0', strlen(query_buff));
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
						format_and_execute(param->db, "UPDATE %s SET online='online' WHERE username='%s'", USERS_TN, user);
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
				if (u_send(param, "N", 1) <= 0) *online = false;
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
				if (u_send(param, "N", 1) <= 0) *online = false;
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
	//printf("Encode code string: %s\n", code);
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
	char error_buf[1000];
	switch(code_num)
	{
		case SEND_MESSAGE: //S@TEXT@TIME@chat_ID - send message
			printf("[INFO] Want to send a message\n");
			if (!validate_query(code, 3, "Message sending query is wrong, incorrent delimiter count!\n"))
      		{
				db_query = "INSERT INTO %s(from_username, message_text, send_datetime, chat_id) VALUES('%s', '%s', %s, %s)";
				str_timestamp = mx_itoa(time(NULL));
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, user, parts[1], str_timestamp , parts[3]);
				printf("Execution status = %d\n", executing_status);
				free(str_timestamp);
				
				db_query = "SELECT max(message_id) FROM %s";
				table = get_db_data_table(param->db, db_query, 1, 1, MESSAGES_TN);

				if (executing_status != 0 || !table || !*table)
				{
					if (u_send(param, ERROR_CODE, 2) <= 0) *online = false;			
				}
				else
				{
					if (u_send(param, table[0], strlen(table[0])+ 1) <= 0) *online = false;
				}
				printf("[INFO] Successfuly sent a message\n");
				delete_table(&table);
			}
			else
			{
				sprintf(error_buf, "%s%sMessage sending query is wrong, incorrent delimiter count!", ERROR_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			break;
		case CREATE_CHAT: //C@NAME@USERNAME1@USERNAME2@... - create new chat 
			/*
				parts[0] - action
				parts[1] - chat name
				parts[2]...parts[n] - members
			*/
			printf("[INFO] Want to create a chat\n");
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
						sprintf(error_buf, "%s%sNo such user", ERROR_CODE, QUERY_DELIM);
						if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
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
							sprintf(query_buff, "%s%s%s", RECORD_EXISTS_CODE, QUERY_DELIM, table[0]);
							if (u_send(param, query_buff, strlen(query_buff) + 1) <= 0) *online = false;
							//if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) *online = false;
							delete_table(&table);
							free(members_str);
							return;
						}	
					}
				}

				db_query = "SELECT max(chat_id) FROM %s";
				table = get_db_data_table(param->db, db_query, 1, 1, CHATS_TN);
				
				db_query = "INSERT INTO %s VALUES(%s + 1, '%s', '%s', 'unloaded')";
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
				
				if (executing_status != 0)
				{
					sprintf(error_buf, "%s%s0", ERROR_CODE, QUERY_DELIM);
					if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
					//if (send(param->socket, "0", 1, 0) <= 0) *online = false;				
				}
				else
				{
					//query: M@chat_id@chat_name@LM_from_username@LM_message_text@LM_message_status@chat_members
					memset(query_buff, '\0', strlen(query_buff));
					sprintf(query_buff, "%s%s%d%s%s%sYou%s...%sread%s", OK_CODE, 
																		QUERY_DELIM, 
																		newchat_id, 
																		QUERY_DELIM, parts[1], 
																		QUERY_DELIM, 
																		QUERY_DELIM, 
																		QUERY_DELIM, 
																		members_str);
					//sprintf(query_buff, "%s%s%d%s", OK_CODE, QUERY_DELIM, newchat_id, members_str);
					printf("Sending responce(%s) to user\n", query_buff);
					if (u_send(param, query_buff, strlen(query_buff) + 1) <= 0) *online = false;
					//if (send(param->socket, query_buff, strlen(query_buff) + 1, 0) <= 0) *online = false;
				}
				printf("[INFO] Successfuly created chat\n");
				if (table)
				{
					delete_table(&table);
				}
			}
			else
			{
				sprintf(error_buf, "%s%sFailed to create a chat", ERROR_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			free(members_str);
			break;
		case GET_CHATS_HISTORY: 
		
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
			else
			{
				sprintf(error_buf, "%s%sEmpty chat history", NO_DATA_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			delete_table(&table);
			// GET LAST 20 MESSAGES IGNORING STATUS
			//message_id@from_username@message_text
			printf("[INFO] Want to get initial chat messages\n");
			
			break;
		case GET_NEW_MESSAGES: // CONTINUALY GET MESSAGES WITH UNREAD STATUS
			//M@message_id@from_username@message_text@send_datetime@chat_id
			db_query = "SELECT * \
						FROM (SELECT m.message_id, m.from_username, m.message_text, m.send_datetime, m.chat_id \
							  FROM %s c \
							  INNER JOIN %s m \
							  ON c.chat_id = m.chat_id \
							  WHERE m.from_username != '%s' AND chat_members LIKE '%%%s%%' AND m.chat_id=%s AND m.status='unread'\
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
			else
			{
				sprintf(error_buf, "%s%sNo new messages to receive", NO_DATA_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			delete_table(&table);
			break;
		case GET_ALL_CHATS:
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
				sprintf(error_buf, "%s%sFailed to get new messages", ERROR_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			delete_table(&table);
			break;
		case GET_NEW_CHATS:
			db_query = "SELECT  c.chat_id, \
								c.chat_name, \
								CASE MAX_MID IS NULL \
									WHEN 1 THEN 'You' || ' @ ' || '...' || '@' || '...' \
									ELSE (SELECT m3.from_username || '@' || m3.message_text || '@' || m3.status \
										  FROM %s m3 WHERE m3.message_id=MAX_MID) \
								END MESSAGE_DATA \
								, \
								c.chat_members \
						FROM (SELECT *, (SELECT max(message_id) FROM %s m WHERE m.chat_id = c2.chat_id) AS MAX_MID FROM %s c2) c \ 
						WHERE c.load_status = 'unloaded' AND c.chat_members NOT LIKE '%s%%' AND c.chat_members LIKE '%%%s%%'";

			table = get_db_data_table(param->db, db_query, 4, DB_ROWS_MAX, MESSAGES_TN, MESSAGES_TN, CHATS_TN, user, user);
			if (table && *table)
			{
				printf("Have something to work with\n");
				*online = s_to_c_info_exchange(param, table);
				for (int i = 0; table[i]; i++)
				{
					db_query = "UPDATE %s SET load_status='loaded' WHERE chat_id=%s";
					executing_status = format_and_execute(param->db, db_query, CHATS_TN, strtok(table[i], QUERY_DELIM));
				}
			}
			else
			{
				sprintf(error_buf, "%s%sNo new chats to send", NO_DATA_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			delete_table(&table);
			break;
		case GET_COLLOCUTOR_INFO: //K@chat_id
			// we have an id of chat, we have to get username of collocutor, his online and avatar
			if (!validate_query(code, 1, "Message sending query is wrong, incorrent delimiter count!\n"))
      		{
				db_query = "SELECT username, online, avatar \
							FROM %s u \
							INNER JOIN %s c \
							WHERE c.chat_id=%s AND REPLACE(c.chat_members, '%s', '')=username";

				printf(db_query, USERS_TN, CHATS_TN, parts[1], user);
				table = get_db_data_table(param->db, db_query, 3, 1, USERS_TN, CHATS_TN, parts[1], user);
				if (table && *table)
				{
					sprintf(error_buf, "%s%s%s", OK_CODE, QUERY_DELIM, table[0]);
					if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
				}
				else
				{
					sprintf(error_buf, "%s%sNo data", NO_DATA_CODE, QUERY_DELIM);
					if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
				} 
			}
			else
			{
				sprintf(error_buf, "%s%sMessage sending query is wrong, incorrent delimiter count!", ERROR_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) *online = false;
			}
			break;
		case EDIT_MESSAGE: //B@message_id@new_message_text
			printf("[INFO] Want to edit a message\n"); 
			if (!validate_query(code, 2, "Message edition query is wrong, incorrent delimiter count!\n"))
			{
				printf("Message id(%s), new message text(%s)\n", parts[1], parts[2]);
				db_query = "UPDATE %s SET message_text='%s' WHERE message_id=%s";
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, parts[2], parts[1]);
				if (executing_status != 0)
				{
					printf("[ERROR] Failed while formating ane executing query\n");
				}
			}
			break;
		case DELETE_MESSAGE:
			if (!validate_query(code, 1, "Message deleting query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "DELETE FROM %s WHERE message_id=%s";
				executing_status = format_and_execute(param->db, db_query, MESSAGES_TN, parts[1]);
				if (executing_status != 0)
				{
					printf("[ERROR] Failed while formating ane executing query\n");
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
							printf("[ERROR] Failed while formating ane executing query\n");
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
			break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
		pthread_mutex_lock(param->mutex_R);
		encode(buffer, param, &online, user);
		pthread_mutex_unlock(param->mutex_R);
		memset(buffer, '\0', MESSAGE_MAX_LEN);
    }
	printf("DISCONNECTING User: %s\n", user);
	format_and_execute(param->db, "UPDATE %s SET online='offline' WHERE username='%s'", USERS_TN, user);
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



