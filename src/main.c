#include "../inc/header.h"

void encode(char * code, sqlite3 * db)
{
	if (!code || !db) return;
	
	/*
		<---CODES--->
		L@NAME@PASSWORD - login
		R@NAME@PASSWORD - register
		S@TEXT@TIME@CONVERSATION_ID - send message
		C@NAME@USERNAME1@USERNAME2@... - create new chat 
		F@USERNAME@CONVERSATION_ID - renew chat
		B@MESSAGE_ID
		D@MESSAGE_ID
		E@USERNAME@CONVERSATION_ID
	*/

	void *** table = NULL;
	char query_buf[1000];
	char * sql_username_str = NULL;
	char ** parts = mx_strsplit(code, QUERY_DELIM);
	char code_num = parts[0][0];
	int err = 0;

	switch(code_num)
	{
		case LOGIN: // loging
			if (!validate_query(code, 2, "Loging query is wrong, incorrent delimiter count!\n"))
			{
				sql_username_str = to_sql_string(parts[1]);
				sprintf(query_buf, "SELECT password FROM %s WHERE username=%s", USERS_TN, sql_username_str);
				free(sql_username_str);
				table = get_db_data_vector(db, query_buf, 1);

				if (!table || !(*table))
				{
					printf("[ERROR] Invalid username or password\n");
				}
				else
				{
					err = login(table[0][0], parts[2]);
					if (!err)
					{
						printf("[INFO] correct loging\n");
					}
					else
					{
						printf("[ERROR] Invalid username or password\n");		
					}
				}
				delete_table(&table);
			}
			break;		
		case SIGNUP:
			if (!validate_query(code, 2, "SignUp query is wrong, incorrent delimiter count!\n"))
			{
				sql_username_str = to_sql_string(parts[1]);
				sprintf(query_buf, "SELECT password FROM %s WHERE username=%s", USERS_TN, sql_username_str);
				free(sql_username_str);
				table = get_db_data_vector(db, query_buf, 1);

				if (!table || !(*table))
				{
					create_account(db, parts[1], parts[2]);
					printf("[INFO] Successfuly signed up\n");
				}
				else
				{
					printf("[ERROR] Account with such username is already exists\n");
				}
				delete_table(&table);
			}
			break;
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
			}
			break;
		case DELETE_MESSAGE:
			if (!validate_query(code, 1, "Message deleting query is wrong, incorrent delimiter count!\n"))
			{
				//parts[1] -- message_id in table messages to be deleted
				//send a request to db to DELETE record in table messages with corresponding message_id
				//then update the whole chat or just delete it in GUI
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

int main()
{
	//check input data for signs '=', ';'
	sqlite3 * db = NULL;
	sqlite3_open("database.db", &db);
	char username[10000];
	char password[10000];
	char action[2];

	format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            username TEXT NOT NULL UNIQUE, \
                            password TEXT NOT NULL)", 1, USERS_TN);

	format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(message_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
                            from_userid INTEGER NOT NULL, \
                            message_text TEXT NOT NULL, \
                            send_datetime TEXT NOT NULL, \
                            conversation_id TEXT NOT NULL, \
                            status TEXT NOT NULL DEFAULT 'unread')", 1, MESSAGES_TN);
  
  	//create table with group_members data
  	format_and_execute(db, "CREATE TABLE IF NOT EXISTS %s(user_id INTEGER NOT NULL, \
                            conversation_id TEXT NOT NULL, \
                            conversation_name TEXT NOT NULL DEFAULT 'group', \
                            joined_datetime TEXT NOT NULL, \
                            left_datetime TEXT NOT NULL)", 1, GROUP_MEMBERS_TN);

	printf("Enter action(L - login, R - signup): ");
	scanf("%s", action);
	
	printf("Enter username and password(space separated): ");
	scanf("%s %s", username, password);

	code network_query = create_network_query(3, action, username, password);
	printf("Query: %s\n", network_query);
	encode(network_query, db);
	free(network_query);

	sqlite3_close(db);
	return 0;
}


