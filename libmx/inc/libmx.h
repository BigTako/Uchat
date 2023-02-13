#ifndef LIBMX_H
#define LIBMX_H

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <malloc/malloc.h>
//#include <malloc.h>

//MY FUNCTION PACK
bool mx_islower(int c);
bool mx_isupper(int c);
bool mx_isalpha(int c);
bool mx_isdigit(int c);
int mx_strncmp(const char *s1, const char *s2, int n);
char *mx_strchr(const char *s, int c);
bool mx_isspace(char c);
void mx_printerr(const char *s);
bool mx_isdigit(int c);
bool mx_isalpha(int c);

//UTILS PACK
void mx_printchar(char c);
void mx_printstr(const char *s);
void mx_printerr(const char *s);
void mx_print_strarr(char **arr, const char *delim);
void mx_printint(int n);
double mx_pow(double n, unsigned int pow);
int mx_sqrt(int x);
char *mx_itoa(int number);
void mx_foreach(int *arr, int size, void (*f)(int));

//STRING PACK
int mx_strlen(const char *s);
void mx_swap_char(char *s1, char *s2);
void mx_str_reverse(char *s);
void mx_strdel(char **str);
void mx_del_strarr(char ***arr);
char *mx_strdup(const char *s1);
char *mx_strndup(const char *s1, size_t n);
char *mx_strcpy(char *dst, const char *src);
char *mx_strchr(const char *s, int c);
char *mx_strncpy(char *dst, const char *src, int len);
unsigned long mx_strcmp(const unsigned char *s1, const unsigned char *s2);
char *mx_strcat(char *restrict s1, const char *restrict s2);
char *mx_strstr(const char *haystack, const char *needle);
int mx_get_substr_index(const char *str, const char *sub);
int mx_count_substr(const char *str, const char *sub);
int mx_count_words(const char *str, char c);
char *mx_strnew(const int size);
char *mx_strtrim(const char *str);
char *mx_del_extra_spaces(const char *str);
char **mx_strsplit(const char *s, char c);
char *mx_strjoin(const char *s1, const char *s2);
char *mx_file_to_str(const char *file);
char *mx_replace_substr(const char *str, const char *sub, const char *replace);
int mx_atoi(const char* str, int * err);
int mx_get_char_index(const char *str, char c);
void mx_delstr_arr_arr(char *** arr1, char ***arr2, char *** arr3, char *** arr4);
char * mx_strstr_front(const char *haystack, const char *needle);
int char_count(char * str, char c);

//LIST PACK
typedef struct s_list {
    void *data;
    struct s_list*next;
}           t_list;

t_list *mx_create_node(void *data);
void mx_push_front(t_list **list, void *data);
void mx_push_back(t_list **list, void *data);
void mx_pop_front(t_list **head);
void mx_pop_back(t_list **head);
int mx_list_size(t_list *list);
t_list *mx_sort_list(t_list *lst, bool (*cmp)(void *, void *));
void * valueOf(t_list * list, int i);
int indexOf(t_list * list, void * value);
void mx_clear_list(t_list **list, void(*free_content)(void * ptr));
#endif
