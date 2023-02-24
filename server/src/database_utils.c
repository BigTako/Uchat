#include "../inc/header.h"

int count_placeholders(char * str, char c)
{
	int i = 0, count = 0;
	for (; str[i]; i++)
	{
  		count += (str[i] == c && mx_isalpha(str[i + 1]));
	}
	return count;
}

char * execute_query(sqlite3 * db, char * query)
{
	char * err = NULL;
	int rc = sqlite3_exec(db, (const char *)query, NULL, NULL, &err);
	if (rc != SQLITE_OK)
	{
		return err;
	}
	return NULL;
}

t_list * get_db_data_list(sqlite3 * db, char * selection_query, int cols_count)
{
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, (const char *)selection_query, -1, &stmt, NULL);
	int data_type;
	t_list * table = NULL;
	t_list * row = NULL;
	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		for (int i = 0; i < cols_count; i++)
		{
			data_type = sqlite3_column_type(stmt, i);
			if (data_type == SQLITE_INTEGER)
			{
				mx_push_back(&row, mx_itoa(sqlite3_column_int(stmt, i)));
			}
			else if (data_type == SQLITE_TEXT)
			{
				mx_push_back(&row, mx_strdup((const char *)sqlite3_column_text(stmt, i)));
			}
		}
		mx_push_back(&table, row);
		row = NULL;
	}
	sqlite3_finalize(stmt);
	return table;
}

void ** get_db_data_vector(sqlite3 * db, char * selection_query, int cols_count, int rows_count)
{
	void ** table = malloc(((int)DB_ROWS_MAX)*sizeof(void*));
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, (const char *)selection_query, -1, &stmt, NULL);

	printf("1\n");
	int row_indx = 0;
	while (sqlite3_step(stmt) != SQLITE_DONE && row_indx != rows_count)
	{
		printf("able to find something\n");
		table[row_indx] = mx_strnew(100000);
		for (int i = 0; i < cols_count; i++)
		{
			if (!sqlite3_column_text(stmt, i))
			{
				table[row_indx + 1] = NULL;
				delete_table(&table);
				return NULL;			
			}
			table[row_indx] = strcat(table[row_indx], sqlite3_column_text(stmt, i));
			if (i != cols_count - 1)
			{
				table[row_indx] = strcat(table[row_indx], QUERY_DELIM);
			}
		}
		row_indx++;
	}
	table[row_indx] = NULL;
	sqlite3_finalize(stmt);
	return table;
}

char * format_query(char * template, t_list * values)
{
	char * values_str = mx_strnew(100000);
	char * result = mx_strnew(100000);
	for (t_list * i = values; i; i = i->next)
	{
		values_str = mx_strcat(values_str, i->data);
		if (i->next)
		{
			values_str = mx_strcat(values_str, ",");	
		}
	}
	printf("Format: %s\n", template);
	snprintf(result, 10000, template, values_str);
	free(values_str);
	return result;
}

void clear_inner_list(void * ptr)
{
	t_list * list = ptr;
	if(!list) return;
	t_list * temp = list;
	t_list * copy = temp;
	while (temp)
	{
		copy = temp->next;
		free(temp->data);
		free(temp);
		temp = copy;
	}
	list = temp;
}

void delete_table(void *** table)
{
	if (!table) return;
	for (int i = 0; (*table)[i]; i++)
	{
		free((*table)[i]);	
	}
	free((*table));
	*table = NULL;
}

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




