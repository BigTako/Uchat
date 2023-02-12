#include "../inc/libmx.h"
char*mx_strcat(char *restrict s1, const char *restrict s2)
{
    int l1 = mx_strlen(s1);
    int l2 = mx_strlen(s2);
    s1[l2 + l1 + 1] = '\0';
    for (int i = 0; i < l2 + l1 + 1; i++)
        s1[l1 + i] = s2[i];
    return s1;
}

