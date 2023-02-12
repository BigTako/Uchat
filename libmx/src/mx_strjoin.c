#include "../inc/libmx.h"

char *mx_strjoin(char const *s1, char const *s2)
{
	if (!s1 
		&& !s2) return NULL;
	else if (!s2) return mx_strdup(s1);
	else if (!s1) return mx_strdup(s2);
	char * joined = mx_strnew(mx_strlen(s1) + mx_strlen(s2) + 1);
	joined = mx_strcat(joined, s1);
	joined = mx_strcat(joined, s2);
	return joined;
}


