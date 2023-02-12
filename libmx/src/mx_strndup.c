#include "../inc/libmx.h"

char * mx_strndup( const char * s1, size_t n)
{
    char *dup = NULL;
    size_t len = mx_strlen(s1);

    if (n < len) {
        len = n;
    }
    dup = mx_strnew(len);
    mx_strncpy(dup, s1, len);
    return dup;
}


