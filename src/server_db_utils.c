#include "../inc/header.h"

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

int validate_query(char * code, int delims_count, char * err_message)
{
	if (char_count(code, QUERY_DELIM) != delims_count)
	{
		printf("%s\n", err_message);
		return 1;
	}
	return 0;
}

