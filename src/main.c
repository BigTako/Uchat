#include "../inc/header.h"
#include <stdarg.h>
#define QUERY_DELIM '@'


typedef unsigned char * code;

int sqlite_execute(sqlite3 * db, char * query)
{
	char * db_error = execute_query(db, query);
	if (db_error)
	{
		printf("[ERROR] %s\n", db_error);
		free(db_error);
		return 1;
	}
	return 0;
}

char * to_sql_string(char * str)
{
	if (!str) return NULL;
	char * result = mx_strnew(mx_strlen(str) + 2);
	sprintf(result, "'%s'", str);
	return result;
}

code sha256_string(char *string)
{
    code hash = malloc(65*sizeof(char));
    unsigned char out[32];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(out, &sha256);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
	{
        sprintf(hash + (i * 2), "%02x", out[i]);
    }
    return hash;
}

void create_account(sqlite3 * db, char * username, char * password)
{
	char query_buf[10000];
	char * passwd_hash = sha256_string(password);
	char * sql_str_password = to_sql_string(passwd_hash);
	char * sql_str_username = to_sql_string(username);
	sprintf(query_buf, "INSERT INTO %s(username, password) VALUES(%s, %s)", USERS_TN, sql_str_username, sql_str_password);
	sqlite_execute(db, query_buf);
	free(passwd_hash);
	free(sql_str_username);
	free(sql_str_password);
}

int login(code db_hash, code input_password)
{
	code input_pas_hash = sha256_string(input_password);
	if (!strcmp(input_pas_hash, db_hash)) // alex hash password and input password hash are the same
	{
		free(input_pas_hash);
		return 0;
	}
	else
	{
		free(input_pas_hash);
		return 1;
	}
}

int char_count(char * str, char c)
{
	int i = 0, count = 0;
	for (; str[i]; i++)
	{
  		count += (str[i] == c);
	}
	return count;
}

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
			query = mx_strcat(query, "@");
		}
	}
    // Ending argument list traversal
	va_end(ptr);
	return query;
}

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
			if (char_count(code, QUERY_DELIM) != 2)
			{
				printf("Loging query is wrong, incorrent delimiter count!\n");
				mx_del_strarr(&parts);
				return;
			}
			
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
			break;		
		case SIGNUP:
			if (char_count(code, QUERY_DELIM) != 2)
			{
				printf("SignUp query is wrong, incorrent delimiter count!\n");
				mx_del_strarr(&parts);
				return;
			}
			
			
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
			break;
		case SEND_MESSAGE:
			if (char_count(code, QUERY_DELIM) != 3)
			{
				printf("Message sending query is wrong, incorrent delimiter count!\n");
				mx_del_strarr(&parts);
				return;
			}
			//parts[1] -- message text
			//parts[2] -- sending time
			//parts[3] -- conversation id
			//handle a sending message to a specific conversation
			break;
		case CREATE_CHAT:
			if (char_count(code, QUERY_DELIM) != 3)
			{
				printf("Message sending query is wrong, incorrent delimiter count!\n");
				mx_del_strarr(&parts);
				return;
			}
			//parts[1] -- chat name
			//parts[2] -- username of chat member1
			//parts[3] -- username of chat member2
			// ...
			//parts[n] -- username of chat member (n - 1)
			//handle a crejating new chat here(add users by ids to group_members table)

			break;
		case RENEW_CHAT:
			if (char_count(code, QUERY_DELIM) != 2)
			{
				printf("Chat renewing query is wrong, incorrent delimiter count!\n");
				mx_del_strarr(&parts);
				return;
			}

			//parts[1] -- username, for which chat have to be renewed
			//parts[2] -- id of chat 
			//take all the messages unread from the chat by conversation id
			break;
	}
	mx_del_strarr(&parts);
}

int main()
{
	//check input data for signs '=', ';'
	sqlite3 * db = NULL;
	sqlite3_open("database.db", &db);
	char query_buf[10000];
	char username[10000];
	char password[10000];
	int cols_count = 3;
	char action[2];

	sprintf(query_buf, "CREATE TABLE IF NOT EXISTS %s(user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
														username TEXT NOT NULL UNIQUE, \
														password TEXT NOT NULL)", USERS_TN);
	
	sqlite_execute(db, query_buf);
	memset(query_buf, '\0', 10000);

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


