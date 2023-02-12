#include "../inc/libmx.h"

void mx_delstr_arr_arr(char *** arr1, char ***arr2, char *** arr3, char *** arr4)
{
	if (arr1 && *arr1) mx_del_strarr(arr1);
	if (arr2 && *arr2) mx_del_strarr(arr2);
	if (arr3 && *arr3) mx_del_strarr(arr3);
	if (arr4 && *arr4) mx_del_strarr(arr4);
}

