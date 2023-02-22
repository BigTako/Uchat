#include "../inc/header.h"

void ** get_db_data_table(sqlite3 * db, char * template, int colums, int rows, ...)
{
	char query_buf[4096];
 	va_list ptr;
  	va_start(ptr, HOLDERS(template));
  	vsprintf(query_buf, template, ptr);
	printf("Query: %s\n", query_buf);
	void ** table = get_db_data_vector(db, query_buf, colums, rows);
  	// Ending argument list traversal
  	va_end(ptr);
	return table;
}

code create_query_delim_separated(int count, ...)
{
	code query = mx_strnew(100000);
    va_list ptr;
    // Initializing argument to the
    // list pointer
    va_start(ptr, count);
    for (int i = 0; i < count; i++)
	{
		query = mx_strcat(query, va_arg(ptr, unsigned char *));
		if (i != count - 1)
		{
			query = (code)mx_strcat(query, QUERY_DELIM);
		}
	}
	va_end(ptr);
	return query;
}

int validate_query(char * code, int delims_count, char * err_message)
{
	if (char_count(code, QUERY_DELIM[0]) != delims_count)
	{
		printf("%s\n", err_message);
		return 1;
	}
	return 0;
}

int format_and_execute (sqlite3 * db, char * template, ...)
{
	char query_buf[10000];
	va_list ptr;
	printf("Format execute query: %s holders: %d\n", template, HOLDERS(template));
	va_start(ptr, HOLDERS(template));
	vsprintf(query_buf, template, ptr);
	printf("Query: %s holders: %d\n", query_buf, HOLDERS(template));
	int status = sqlite_execute(db, query_buf);
	// Ending argument list traversal
	va_end(ptr);
	return status;
}
