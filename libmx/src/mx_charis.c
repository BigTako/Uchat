#include "../inc/libmx.h"

bool mx_isspace(char c)
{
	return c == 32
			|| (c > 8 
				&& c < 14);
}

bool mx_isalpha(int c)
{
	return (c > 64 
		&& c < 91) 
			|| (c > 96 
				&& c < 123);
}

bool mx_isdigit(int c)
{
	return c > 47 
		&& c < 58;
}

bool mx_islower(int c)
{
    return c > 96 
    		&& c < 123;
}

bool mx_isupper(int c)
{
    return c > 64 
    		&& c < 91;
}

