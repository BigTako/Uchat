#ifndef HEADER_H
#define HEADER_H
#include "../libmx/inc/libmx.h"
#include "../Sqlite3/sqlite3.h"

#define DB_ROWS_MAX 10000

//DATABASE UTILS
char * mx_strstr_front(const char *haystack, const char *needle);
int execute_query(sqlite3 * db, char * query);
t_list * get_db_data_list(sqlite3 * db, char * selection_query, int cols_count);
void *** get_db_data_vector(sqlite3 * db, char * selection_query, int cols_count);
char * format_query(char * template, t_list * values);
#endif


