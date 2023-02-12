#include "../inc/libmx.h"

char *mx_strtrim(const char *str)
{
	if (!str || mx_strlen(str) == 0)
	{
		return NULL;
	} 
	int begginning = 0;
	int ending = mx_strlen(str);
	for (; mx_isspace(str[begginning]); begginning++);
	for (; mx_isspace(str[ending - 1]); ending--);
	char *result = mx_strnew(ending - begginning + 1);
	result = mx_strncpy(result, str + begginning, ending - begginning);
	return result;
}


