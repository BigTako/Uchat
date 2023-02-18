#include "../inc/header.h"
#define MAX_MESSAGE_ID_QUERY "SELECT max(message_id) FROM %s"

char *encode_login(char *code, t_thread_param *param, bool *online)
{
	if (!code || !param->db) return NULL;
	
	/*
		<---CODES--->
		L@NAME@PASSWORD_HASH - login
		R@NAME@PASSWORD_HASH - register
	*/

	void *** table = NULL;
	char query_buf[1000];
	char * sql_username_str = NULL;
	char ** parts = mx_strsplit(code, QUERY_DELIM);
	char code_num = parts[0][0];
	int err = 0;
	char *user = NULL;
	char * db_query = NULL;
	
	switch(code_num)
	{
		case LOGIN: // loging
			if (!validate_query(code, 2, "Loging query is wrong, incorrent delimiter count!\n"))
			{
				db_query = "SELECT password FROM %s WHERE username='%s'";
				table = get_db_data_table(param->db, db_query, 1, 2, USERS_TN, parts[1]);

				if (!table || !(*table)) // NO SUCH USERNAME IN DATABASE
				{
					printf("[ERROR] Invalid username or password\n");
					if (send(param->socket, "N", 1, 0) <= 0) *online = false;
				}
				else
				{
					if (!strcmp(table[0][0], parts[2]))
					{
						printf("[INFO] correct loging\n");
						user = mx_strdup(parts[1]);
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
				db_query = "SELECT password FROM %s WHERE username='%s'";
				table = get_db_data_table(param->db, db_query, 1, 2, USERS_TN, parts[1]);

				if (!table || !(*table))
				{
					db_query = "INSERT INTO %s(username, password) VALUES('%s', '%s')";
					format_and_execute(param->db, db_query, 3, USERS_TN, parts[1], parts[2]);
					printf("[INFO] Successfuly signed up\n");
					user = mx_strdup(parts[1]);
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
		F@CONVERSATION_ID - renew chat
		B@MESSAGE_ID
		D@MESSAGE_ID
		E@USERNAME@CONVERSATION_ID
	*/
	printf("Encode code string: %s\n", code);
	void *** table = NULL;
	char query_buf[1000];
	char ** parts = mx_strsplit(code, QUERY_DELIM);
	char code_num = parts[0][0];
	//int err = 0;
	char * db_query = NULL;
	char * str_timestamp = NULL;

	switch(code_num)
	{
		case SEND_MESSAGE:
			if (!validate_query(code, 3, "Message sending query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- message text
				//parts[2] -- sending time
				//parts[3] -- conversation id
				//handle a sending message to a specific conversation
				/* <-MESSAGES->
				message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            from_username TEXT NOT NULL, \
                            message_text TEXT NOT NULL, \
                            send_datetime TEXT NOT NULL, \
                            conversation_id TEXT NOT NULL, \
                            status TEXT NOT NULL DEFAULT 'unread')
				*/
				
				//S@TEXT@TIME@CONVERSATION_ID
				db_query = "INSERT INTO %s(from_username, message_text, send_datetime, conversation_id) VALUES('%s', '%s', %s, %s)";
				str_timestamp = mx_itoa(time(NULL));
				format_and_execute(param->db, db_query, 5, MESSAGES_TN, user, parts[1], str_timestamp , parts[3]);
				send(param->socket, "Y", 1, 0);
				free(str_timestamp);
			}
			break;
		case CREATE_CHAT:
			//parts[1] -- chat name
			//parts[2] -- username of chat member1
			//parts[3] -- username of chat member2
			// ...
			//parts[n] -- username of chat member (n - 1)
			//handle a creating new chat here(add users by ids to group_members table)
			/*
				CREATE TABLE IF NOT EXISTS %s(username TEXT NOT NULL, \
                conversation_id TEXT NOT NULL, \
                joined_datetime TEXT NOT NULL)", 1, GROUP_MEMBERS_TN
			*/
			db_query = "INSERT INTO %s(name) VALUES('%s')";
			format_and_execute(param->db, db_query, 2, CONVERSATIONS_TN, parts[1]);
			db_query = "SELECT conversation_id FROM %s WHERE conversation_id=(SELECT max(conversation_id) FROM %s)";
			table = get_db_data_table(param->db, db_query, 1, 2, CONVERSATIONS_TN, CONVERSATIONS_TN);
			printf("table content %s\n", table[0][0]);
			char *conversation_id = mx_strdup(table[0][0]);
			int n = 2;
			while((parts + 1)[n++] != NULL) {
				db_query = "INSERT INTO %s(username, conversation_id, joined_datetime) VALUES('%s', %s, '%lu')";
				format_and_execute(param->db, db_query, 4, GROUP_MEMBERS_TN, parts[n], conversation_id, (unsigned long)time(NULL));
			}
			if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
			break;
		case RENEW_CHAT:
			if (!validate_query(code, 1, "Chat renewing query is wrong, incorrent delimiter count!\n"))
			{
				/*F@CONVERSATION_ID - renew chat
				 	<-MESSAGES->
					message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                    from_username TEXT NOT NULL, \
                    message_text TEXT NOT NULL, \
                    send_datetime TEXT NOT NULL, \
                    conversation_name TEXT NOT NULL, \
                    status TEXT NOT NULL DEFAULT 'unread')
				*/
				//parts[1] -- username, for which chat have to be renewed
				//parts[2] --  of chat 
				//take all the messages unread from the chat by conversation id
				db_query = "SELECT message_id, from_username, message_text, send_datetime, conversation_name, status(SELECT max(message_id) FROM (SELECT * FROM %s WHERE status == 'unread' AND conversation_id == %s)) AS max_id \
							FROM %s \
							WHERE (max_id - %d < 1 AND message_id <= max_id) OR \
      						(max_id - %d > 1 AND message_id > max_id - %d AND message_id <= max_id)";
				table = get_db_data_table(param->db, db_query, 4, 6, MESSAGES_TN, parts[1], MESSAGES_TN, LOAD_MESSAGE_COUNT, LOAD_MESSAGE_COUNT, LOAD_MESSAGE_COUNT);
				for (int i = 0; table[i]; i++)
				{
					for (int j = 0; table[j]; j++)
					{
						printf("%s-", table[i][j]);
					}	
					printf("\n");
				}
				printf("Successfully renewed chat\n");
				send(param->socket, "Y", 1, 0);
			}
			break;
		case EDIT_MESSAGE:
			if (!validate_query(code, 1, "Message edition query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- message_id in table messages to be updated content to
				//send a request to db to UPDATE a content of message WHERE message_id=message_id
				//then update the whole chat or just edit it in GUI
				if (!validate_query(code, 1, "Message edition query is wrong, incorrent delimiter count!\n"))
				{
					format_and_execute(param->db, "UPDATE %s SET message_text='%s' WHERE message_id=%s", MESSAGES_TN, parts[2], parts[1]);
				}
			}
			break;
		case DELETE_MESSAGE:
			if (!validate_query(code, 1, "Message deleting query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- message_id in table messages to be deleted
				//send a request to db to DELETE record in table messages with corresponding message_id
				//then update the whole chat or just delete it in GUI
				format_and_execute(param->db, "DELETE FROM %s WHERE message_id=%s", MESSAGES_TN, parts[1]);
			}
			break;
		case EXIT_CONVERSATION:
			if (!validate_query(code, 2, "Conversation exit query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- username of person wants to leave a convestion
				//parts[2] -- id of conversation user want to leave
				//send a request to database to DELETE a user with username from table group_members WHERE conversation_id=conversation_id
				//then renew chat list
			}
			break;
	}
	mx_del_strarr(&parts);
}

void* client_thread(void* vparam) 
{
    t_thread_param *param = (t_thread_param*) vparam;
    printf("---Start-recving-from-new-client---\n");
    bool online = true;
    bool logined = false;
    char *user = NULL;
    char buffer[MESSAGE_MAX_LEN]; //!!!
    while (user == NULL) 
	{
		int status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
        if (status <= 0) {
            online = false;
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
        if(buffer[0] == LOGIN || buffer[0] == SIGNUP) {
            user = encode_login(buffer, param, &online);
        } else {
            //send that u need to login first
        }
		memset(buffer, '\0', MESSAGE_MAX_LEN);
    }
    while (online) {
		int status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
        if (status <= 0) {
            online = false;
            //OFLINE
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
		encode(buffer, param, &online, user);
		memset(buffer, '\0', MESSAGE_MAX_LEN);
    }
    close(param->socket);
    free(param);
    pthread_exit(0);
}



