#include "../inc/header.h"

void *** get_db_data_table(sqlite3 * db, code template, int colums, int count, ...)
{
	char query_buf[4096];
 	va_list ptr;
  	va_start(ptr, count);
  	vsprintf(query_buf, template, ptr);
	printf("Query: %s\n", query_buf);
	void *** table = get_db_data_vector(db, query_buf, colums);
  	// Ending argument list traversal
  	va_end(ptr);
	return table;
}

char *userID_from_name(char * username, sqlite3 *db) {
	/*void *** table = NULL;
	sprintf(query, "SELECT user_id FROM %s WHERE username='%s'", USERS_TN, username);
	table = get_db_data_vector(db, query, 1);*/
	char * query = "SELECT user_id FROM %s WHERE username='%s'";
	char *user_id = NULL;
	void *** table = get_db_data_table(db, query, 1, 2, USERS_TN, username);
	if (*table)
	{
		user_id = mx_strdup(table[0][0]);	
		printf("user_id: %s\n", user_id);
	}
	delete_table(&table);
	return user_id;
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
			query = mx_strcat(query, &delim);
		}
	}
    // Ending argument list traversal
	va_end(ptr);
	return query;
}

int validate_query(char * code, int delims_count, char * err_message)
{
	if (char_count(code, QUERY_DELIM) != delims_count)
	{
		printf("%s\n", err_message);
		return 1;
	}
	return 0;
}

void format_and_execute (sqlite3 * db, char * template, int count, ...)
{
  char query_buf[10000];
  va_list ptr;
  va_start(ptr, count);
  vsprintf(query_buf, template, ptr);
  sqlite_execute(db, query_buf);
  // Ending argument list traversal
  va_end(ptr);
}
