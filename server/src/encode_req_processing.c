#include "../inc/header.h"

bool db_login(t_thread_param * param, char ** parts, char ** user)
{
	char * db_query = "SELECT password FROM %s WHERE username='%s%s'";
	char ** table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, USERS_TN, QUERY_DELIM, parts[1]);
	bool online = true;
	if (!table || !(*table)) // NO SUCH USERNAME IN DATABASE
	{
		if (u_send(param, ERROR_CODE, 2) <= 0) online = false;
		online = false;
	}
	else
	{
		if (!strcmp(table[0], parts[2]))
		{
			*user = mx_strjoin(QUERY_DELIM, parts[1]);
			online = true;
			if (u_send(param, OK_CODE, 2) <= 0) online = false;
		}
		else
		{
			u_send(param, ERROR_CODE, 2);
			online = false;
		}
	}
	delete_table(&table);
	return online;
}


bool db_signup(t_thread_param * param, char ** parts)
{
	bool online = true;
	char * db_query = "SELECT password FROM %s WHERE username='%s%s'";
	char ** table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, USERS_TN, QUERY_DELIM, parts[1]);

	if (!table || !(*table))
	{
		db_query = "INSERT INTO %s(username, password) VALUES('%s%s', '%s')";
		format_and_execute(param->db, db_query, USERS_TN, QUERY_DELIM, parts[1], parts[2]);
		online = true;
		if (u_send(param, OK_CODE, 2) <= 0) online = false;
	}
	else
	{
		u_send(param, ERROR_CODE, 2); 
		online = false;
	}
	delete_table(&table);
	return online;
}

/*
chats:
1) when chat is created(request receive), data is inserted to db in way they are, status=ACTIVE, loaded_by remains empty
2) when a client send request for new chats, 
	server has to send a records where his username is in chat_members AND (he didn't load a chat yet).
	This way client is able to get not only recently created chats, but that ones that were deleted by him or someone else.
	Moreover, the users, which were members of deleted chat will get a signal to delete it from chat list.
3) when chat is deleted, user, who deletes the chat will cleanup the 'loaded_by' field and set status to 'DELETED'. When the GET_CHATS request is received
	an SQL query will take this record and send it. 
	The record won't be taken twice becase after sending user`s username will be added to
	'loaded_by' field, that makes SQL query condition false.
4) 
*/

void leave_chat(t_thread_param * param, char * user, char * chat_id)
{
	char * db_query = "SELECT chat_members FROM %s WHERE chat_id=%s";
	char ** table = get_db_data_table(param->db, db_query, 1, DB_ROWS_MAX, CHATS_TN, chat_id);
	if (!table || !*table)
	{
		printf("[ERROR] Data not found\n");
	}
	else
	{
		db_query = "UPDATE %s SET loaded_by='', status='%s' WHERE chat_id=%s";
		format_and_execute(param->db, db_query, CHATS_TN, DELETED_STATUS, chat_id);
		db_query = "UPDATE %s SET loaded_by='', status='%s' WHERE chat_id=%s";
		format_and_execute(param->db, db_query, MESSAGES_TN, DELETED_STATUS, chat_id);
	}
	delete_table(&table);
}

bool update_data_status(t_thread_param * param, char ** table, char * user, char action)
{
  bool online = s_to_c_info_exchange(param, table);
  if (!online)
  {
	return false;
  }
  char error_buf[1000];
  char text_buf[1000];
  char * db_query = NULL;
  if (table && *table)
  {
    for (int i = 0; table[i]; i++)
    {
	  
      if (tokenize(table[i], QUERY_DELIM[0], text_buf, 2)) // message or chat id
      {
		printf("action: %s\n", text_buf);
        switch(action)
        {
			case GET_CHATS:
				db_query = "UPDATE %s SET loaded_by=loaded_by || '%s' WHERE chat_id=%s";
				printf(db_query,CHATS_TN, user, text_buf);
				format_and_execute(param->db, db_query, CHATS_TN, user, text_buf);
				break;
			case GET_MESSAGES:
				db_query = "UPDATE %s SET loaded_by=loaded_by || '%s', status=(CASE(status) WHEN '%s' THEN '%s' ELSE status END)\
							WHERE message_id=%s";
				format_and_execute(param->db, db_query, MESSAGES_TN, user, EDITED_STATUS, ACTIVE_STATUS, text_buf);
				break;
        }
        memset(text_buf, '\0', strlen(text_buf));
      }
    }
  }
  else
  {
    sprintf(error_buf, "%s%sNo data", NO_DATA_CODE, QUERY_DELIM);
    if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) online = false;
  }
  return online;
}

bool user_exists(t_thread_param * param, char * user)
{
	char * db_query = "SELECT 1 FROM %s WHERE username='%s%s'";
	char ** table = get_db_data_table(param->db, db_query, 1, 1, USERS_TN, QUERY_DELIM, user);
	if (table && *table)
	{
		delete_table(&table);
		return true;	
	}
	return false;
}

bool create_db_chat_record(t_thread_param *param, char * chat_name, char * members)
{
	int newchat_id = 1;
	char * db_query = NULL;
	char error_buf[1000];
	char query_buff[1000];
	char ** table = get_db_data_table(param->db, "SELECT max(chat_id) FROM %s", 1, 1, CHATS_TN);
	bool online = true;
	if (table && *table)
	{
		printf("[INFO] Got max chats id: %s\n", table[0]);
		newchat_id += atoi(table[0]) + 1;
	}

	db_query = "INSERT INTO %s VALUES(%d, '%s', '%s', '', '%s')";
	if (format_and_execute(param->db, db_query, CHATS_TN, newchat_id, chat_name, members, ACTIVE_STATUS) != 0)
	{
		if (u_send(param, ERROR_CODE, 2) <= 0) online = false;
	}
	else
	{
		//query: Y@chat_status@chat_id@chat_name@LM_from_username@LM_message_text@LM_message_status@chat_members
		if (u_send(param, OK_CODE, 2) <= 0) online = false;
	}
	
	delete_table(&table);
	return online;
}

bool create_new_chat_record(t_thread_param * param, char * user, char * members_str, char ** parts)
{
	int members_int = mx_null_arr_len(parts + 2) + 1;
	char * another_user = NULL;
	char error_buf[1000];
	char query_buff[1000];
	char * db_query = NULL;
	char ** table = NULL;
	bool online = true;
	if (char_count(members_str, QUERY_DELIM[0]) == members_int)
	{
		if (members_int == 2) // dialog
		{
			if (!strcmp(user, parts[2])) another_user = parts[3];    
			else another_user = parts[2];

			if (!user_exists(param, another_user))
			{
				sprintf(error_buf, "%s%sNo such user", ERROR_CODE, QUERY_DELIM);
				if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) online = false;
				return online;
			}
			else
			{
				db_query = "SELECT chat_id FROM %s WHERE chat_members LIKE '%%%s%%' AND chat_members LIKE '%%%s%%'";
				table = get_db_data_table(param->db, db_query, 1, 1, CHATS_TN, user, another_user);
				if (table && *table) // chat exists
				{
					sprintf(query_buff, "%s%s%s", RECORD_EXISTS_CODE, QUERY_DELIM, table[0]);
					if (u_send(param, query_buff, strlen(query_buff) + 1) <= 0) online = false;
					delete_table(&table);
					return online;
				}
			}
		}
		online = create_db_chat_record(param, parts[1], members_str);
	}
	else
	{
		sprintf(error_buf, "%s%sFailed to create a chat", ERROR_CODE, QUERY_DELIM);
		if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) online = false;
	}
	return online;
}

bool get_collocutor_info(t_thread_param * param, char * chat_id, char * user)
{
	bool online = true;
	char text_buf[1000];
	char error_buf[1000];

	char * db_query = "SELECT username, online, avatar \
							FROM %s u \
							INNER JOIN %s c \
							WHERE c.chat_id=%s AND REPLACE(c.chat_members, '%s', '')=username";
	char ** table = get_db_data_table(param->db, db_query, 3, 1, USERS_TN, CHATS_TN, chat_id, user);
	if (table && *table)
	{
		sprintf(text_buf, "%s%s%s", OK_CODE, QUERY_DELIM, table[0]);
		if (u_send(param, text_buf, strlen(text_buf) + 1) <= 0) online = false;
	}
	else
	{
		sprintf(error_buf, "%s%sNo data", NO_DATA_CODE, QUERY_DELIM);
		if (u_send(param, error_buf, strlen(error_buf) + 1) <= 0) online = false;
	}
	delete_table(&table);
	return online;
}


