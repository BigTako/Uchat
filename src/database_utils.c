#include "../inc/header.h"

int execute_query(sqlite3 * db, char * query)
{
	char * err = NULL;
	int rc = sqlite3_exec(db, (const char *)query, NULL, NULL, &err);
	if (rc != SQLITE_OK)
	{
		mx_printerr(err);
		mx_printerr("\n");
		free(err);
		return 1;
	}
	return 0;
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

void *** get_db_data_vector(sqlite3 * db, char * selection_query, int cols_count)
{
	void *** table = malloc(((int)DB_ROWS_MAX)*sizeof(void**));
	sqlite3_stmt * stmt;
	sqlite3_prepare_v2(db, (const char *)selection_query, -1, &stmt, NULL);
	int data_type;
	int row_indx = 0;
	while (sqlite3_step(stmt) != SQLITE_DONE)
	{
		table[row_indx] = malloc((cols_count+1)*sizeof(void *));
		for (int i = 0; i < cols_count; i++)
		{
			data_type = sqlite3_column_type(stmt, i);
			if (data_type == SQLITE_INTEGER)
			{
				table[row_indx][i] = mx_itoa(sqlite3_column_int(stmt, i));
			}
			else if (data_type == SQLITE_TEXT)
			{
				table[row_indx][i] = mx_strdup(sqlite3_column_text(stmt, i));
			}
		}
		table[row_indx][cols_count] = NULL;
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

