#include "../inc/header.h"

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
	char *userID = NULL;
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
						userID = userID_from_name(parts[1],param->db);
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
					userID = userID_from_name(parts[1], param->db);
					if (send(param->socket, "Y", 1, 0) <= 0) *online = false;
					*online = true;
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
	return userID;
}

void encode(char * code, t_thread_param *param, char *userID)
{
	if (!code || !param->db) return;
	
	/*
		<---CODES--->
		S@TEXT@TIME@CONVERSATION_ID - send message
		C@NAME@USERNAME1@USERNAME2@... - create new chat 
		F@USERNAME@CONVERSATION_ID - renew chat
		B@MESSAGE_ID
		D@MESSAGE_ID
		E@USERNAME@CONVERSATION_ID
	*/

	//void *** table = NULL;
	//char query_buf[1000];
	char ** parts = mx_strsplit(code, QUERY_DELIM);
	char code_num = parts[0][0];
	//int err = 0;
	//char * db_query = NULL;

	switch(code_num)
	{
		case SEND_MESSAGE:
			if (!validate_query(code, 3, "Message sending query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- message text
				//parts[2] -- sending time
				//parts[3] -- conversation id
				//handle a sending message to a specific conversation
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
				CREATE TABLE IF NOT EXISTS %s(user_id INTEGER NOT NULL, \
                conversation_id TEXT NOT NULL, \
                conversation_name TEXT NOT NULL DEFAULT 'group', \
                joined_datetime TEXT NOT NULL, \
                left_datetime TEXT NOT NULL)", 1, GROUP_MEMBERS_TN
			*/
			//db_query = "INSERT INTO %s(conversation_name,joined_datetime,left_datetime) VALUES('%s','%s','%s')";
			//format_and_execute(param->db, db_query, GROUP_MEMBERS_TN, parts[1], );
			break;
		case RENEW_CHAT:
			if (!validate_query(code, 2, "Chat renewing query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- username, for which chat have to be renewed
				//parts[2] -- id of chat 
				//take all the messages unread from the chat by conversation id
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
    char *userID = NULL;
    char buffer[MESSAGE_MAX_LEN]; //!!!
    while (userID == NULL) {
        int status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
        if (status <= 0) {
            online = false;
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
        if(buffer[0] == LOGIN || buffer[0] == SIGNUP) {
            userID = encode_login(buffer, param, &online);
        } else {
            //send that u need to login first
        }
    }
    while (online) {
        int status = recv(param->socket, buffer, MESSAGE_MAX_LEN, 0);
        if (status <= 0) {
            online = false;
            //OFLINE
            break;
        }
        printf(">>>Got a message from unknow client: %s\n", buffer);
		encode(buffer, param, userID);
    }
    close(param->socket);
    free(param);
    pthread_exit(0);
}

