#include "../inc/libmx.h"

int mx_atoi(const char* str, int * err)
{
    int num = 0;
    int ten_pow;
    int num_len = mx_strlen(str);
    for (int q = 0; q < num_len; q++)
    {
        if (!mx_isdigit(str[q]))
        {
            *err = 1;
            return 0;
        }
        ten_pow = 1;
	    for (int j = 0; j < num_len - q - 1; j++)
        {
            ten_pow *= 10;
        } 
        num += (str[q] - 48) * ten_pow; 
    }
    return num;
}

