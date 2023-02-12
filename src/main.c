#include "../inc/header.h"

static void clear_inner_list(void * ptr)
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

void delete_table(void **** table)
{
	for (int i = 0; (*table)[i]; i++)
	{
		for (int j = 0; (*table)[i][j]; j++)
		{
			free((*table)[i][j]);
		}
		free((*table)[i]);	
	}
	free((*table));
	*table = NULL;
}

int main()
{
	//check input data for signs '=', ';'
	sqlite3 * db = NULL;
	sqlite3_open("database.db", &db);
	
	execute_query(db, "CREATE TABLE IF NOT EXISTS messages(message_id INTEGER PRIMARY KEY AUTOINCREMENT,\
												  from_userid INTEGER NOT NULL, \
												  message_text TEXT NOT NULL, \
												  send_datetime TEXT NOT NULL, \
												  conversation_id TEXT NOT NULL)");
	
	char buf[10000];
	int charcheck = sprintf(buf, "INSERT INTO messages VALUES(%d, %d, %s, %s, %s)", 1, 2, "'message'", "'2022'", "'asdfsdfsdfa'");
	execute_query(db, buf);
	printf("Query: %s\n", buf);

	int cols_count = 5;
	void *** table = get_db_data_vector(db, "SELECT * FROM messages", cols_count);
	char * sheesh = NULL;
	int id = 0;
	for (int i = 0; table[i]; i++)
	{
		for (int j = 0; table[i][j]; j++)
		{
			sheesh = table[i][j];
			printf("%s", sheesh);
			if (table[i][j + 1]) printf("-");
		}	
		printf("\n");
	}

	/*for (t_list * i = table; i; i = i->next)
	{
		for (t_list * j = i->data; j; j = j->next)
		{
			printf("%s", j->data);
			if (j->next) printf("-");	
		}
		printf("\n");	
	}*/
	sqlite3_close(db);
	delete_table(&table);
	return 0;
}


