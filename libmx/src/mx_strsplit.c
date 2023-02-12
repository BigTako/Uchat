#include "../inc/libmx.h"

static int count_of_letters(const char *str, char c) 
{
	int i = 0;
	while (str[i] != c && str[i])
		i++;
	return i;
}

char **mx_strsplit(const char *s, char c) {
    int wl = 0;
    int i = 0;
    char **arr = NULL;
    if (!s)
    {
        return NULL;
    }
    arr = (char **)malloc((mx_count_words(s, c) + 1) * sizeof(char *));
    while ((*s) && (*s != '\0'))
    {
        if (*s != c) {
            wl = count_of_letters(s, c);
            arr[i] = mx_strndup(s, wl);
            s += wl;
            i++;
            continue;
        }
        s++;
    }
    arr[i] = NULL;
    return arr;
}


