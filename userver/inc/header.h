#ifndef HEADER_H
#define HEADER_H
#include "../libmx/inc/libmx.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>
#include <signal.h>


#define IEXIT 0
#define IPROG_FOUND 1
#define IPROG_NOT_FOUND 2
#define IPROG_ERROR 3
#define REDIR_NOT_DETECTED 4

#define ERR_FILE ".err.txt"
#define BRACETS "\'\"`"


#define CHAR_IN(str, chr) (mx_get_char_index(str, chr) != -1)
#define IS_BRACET(c) (c == '\'' || c == '\"' || c == '`')
#define FLAG_VALID(c) (c == 'n' || c == 'E' || c == 'e')

typedef int (*built_in_func_ptr)(char * params[], char * params_in_str, int ush_argc);

extern char ** environ;

//VALIDATION
bool key_valid(char * key);
void print_e_message(char * command, char * message, char * filename);

//FILE UTILS
char get_file_type(char * filename);

//COMMANDS
int execute_bin_to_file(char * bin_path, char * res_filename);
int execute_builtin_to_file(char * params[], char * params_in_str, int ush_argc, built_in_func_ptr func);
char * get_comres_filename(char * command);
char * read_result_from_file(char * filename);
int execute_binary(char * bin_path);
int exec_builtin(char * params[], char * params_in_str);
int execute_command(char * ush_str_argv, bool to_file);

//cd
int cd(char * params[], char * params_in_str, int ush_argc);

//export
int export(char * params[], char * params_in_str, int ush_argc);

//unset
int unset(char * params[], char * params_in_str, int ush_argc);

//echo
int echo(char * params[], char * params_in_str);

//result output
void print_file_error();
void print_res_from_file(char * com_filename, int status);

//pipes
int process_pipes(char * data, int out_fd);


#endif


