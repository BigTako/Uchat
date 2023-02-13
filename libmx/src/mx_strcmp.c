#include "../inc/libmx.h"
unsigned long mx_strcmp(const unsigned char *s1, const unsigned char *s2)
{ 
	int i = 0;
	while (s1[i] != '\0'
		&& s2[i] != '\0')
	{
			if (s1[i] != s2[i]) return s1[i] - s2[i];
		 	i++;
	}
	return s1[i] - s2[i];
}


