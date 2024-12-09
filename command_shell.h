#ifndef COMMAND_SHELL_H
#define COMMAND_SHELL_H

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_LENGTH 1024

char **copy_absolute_path();
char *trim_spaces(char *str);
char **split_command(char *input);
void handle_cd_command(char *user_input);
char *get_env_value(const char *var);
void set_env_variable(char *var, char *value);
void unset_env_variable(char *var);
void handle_env_functions(char *user_input);
char *substitute_env_variables(const char *input);
void handle_other_cmd(char *substituted_user_input, char **paths);
char **parse_cmd(char *input, char **input_file, char **output_file, int *is_pipem, char **cmd1, char **cmd2);
char *locate_command(char *cmd, char **paths);
void execute_command(char **args, char *input_file, char *output_file, char **paths);
void handle_pipe(char *cmd1, char *cmd2, char **paths);

void deallocate_tokens(char **tokens);

#endif // COMMAND_SHELL_H