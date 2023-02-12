#include "../inc/libmx.h"


char*mx_strstr(const char *haystack, const char *needle)
{ 
    int size_n = mx_strlen(needle);      
    int size_h = mx_strlen(haystack);
    for (int count = 0; count< size_h; haystack++, count++) 
    {
        char *temporary = mx_strchr(haystack, haystack[0]);
        if(mx_strncmp(temporary,needle,size_n)==0)
        {
            return (char*)haystack;
        }
    }
    return NULL;
}



