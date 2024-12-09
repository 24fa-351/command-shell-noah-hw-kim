#include "command_shell.h"

// extern char **environ;

char **copy_absolute_path()
{
    char *path_env = getenv("PATH");
    if (path_env == NULL)
    {
        printf("no path\n");
        return NULL;
    }

    char *path_env_copy = strdup(path_env);
    if (path_env_copy == NULL)
    {
        printf("strdup failed\n");
        return NULL;
    }

    char *token = strtok(path_env_copy, ":");
    char **paths = malloc(MAX_LENGTH * sizeof(char *));
    int path_ct = 0;
    while (token != NULL)
    {
        paths[path_ct++] = strdup(token);
        token = strtok(NULL, ":");
    }
    paths[path_ct] = NULL;
    free(path_env_copy);
    return paths;
}

char *trim_spaces(char *str)
{
    while (isspace((unsigned char)*str))
        str++;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';

    return str;
}

// splits string by spaces; adds a NULL int to the array after the last word
// result[0] is the command, result[1] is the arguments, result[2] is NULL
char **split_command(char *s)
{
    // for (int i = 0; s[i] != '\0'; i++)
    // {
    //     printf("s[%d]: %c\n", i, s[i]);
    // }

    char **result = malloc(3 * sizeof(char *));
    char *space_pos = strchr(s, ' ');

    if (space_pos != NULL)
    {
        int cmd_len = space_pos - s;
        result[0] = strndup(s, cmd_len);
        result[1] = strdup(space_pos + 1);
        result[1] = trim_spaces(result[1]);
        if (strlen(result[1]) == 0)
        {
            free(result[1]);
            result[1] = NULL;
        }
    }
    else
    {
        result[0] = strdup(s);
        result[1] = NULL;
    }
    result[2] = NULL;

    result[0] = trim_spaces(result[0]);

    // printf("result[0]: %s\n", result[0]);
    // printf("result[1]: %s\n", result[1]);

    return result;
}

void handle_cd_command(char *user_input)
{
    char **tokens = split_command(user_input);
    char *dir = (tokens[1] == NULL) ? NULL : tokens[1];
    if (dir == NULL)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        chdir(dir);
    }
    deallocate_tokens(tokens);
}

char *get_env_value(const char *var)
{
    char *env_val = getenv(var);
    return env_val ? env_val : "";
}

void set_env_variable(char *var, char *value)
{
    setenv(var, value, 1); // Set the variable, 1 means overwrite if it exists
}

void unset_env_variable(char *var)
{
    unsetenv(var);
}

void handle_env_functions(char *user_input)
{
    char **tokens = split_command(user_input);

    if (strncmp(user_input, "set", 3) == 0)
    {
        if (tokens[1] == NULL)
        {
            printf("Usage: set <key> <value>\n");
        }
        else
        {
            char **key_value = split_command(tokens[1]);
            if (key_value[0] != NULL && key_value[1] != NULL)
            {
                set_env_variable(key_value[0], key_value[1]);
            }
            else
            {
                printf("Usage: set <key> <value>\n");
            }
            deallocate_tokens(key_value);
        }
    }
    else if (strncmp(user_input, "get", 3) == 0)
    {
        if (tokens[1] == NULL)
        {
            printf("Usage: get <key>\n");
        }
        else
        {
            char *value = get_env_value(tokens[1]);
            if (strlen(value) == 0)
            {
                printf("Key not found\n");
            }
            else
            {
                printf("%s\n", value);
            }
        }
    }
    else if (strncmp(user_input, "unset", 5) == 0)
    {
        if (tokens[1] == NULL)
        {
            printf("Usage: unset <key>\n");
        }
        else
        {
            unset_env_variable(tokens[1]);
        }
    }
    deallocate_tokens(tokens);
}

char *substitute_env_variables(const char *input)
{
    if (input == NULL)
    {
        return NULL;
    }

    char *result = malloc(MAX_LENGTH);
    result[0] = '\0';

    const char *pos = input;
    while (*pos != '\0')
    {
        if (*pos == '$' && *(pos + 1) != '\0')
        {
            pos++;
            char var_name[MAX_LENGTH];
            int var_len = 0;
            while (*pos != '\0' && (isalnum(*pos) || *pos == '_'))
            {
                var_name[var_len++] = *pos++;
            }
            var_name[var_len] = '\0';
            char *var_value = getenv(var_name);
            if (var_value != NULL)
            {
                strcat(result, var_value);
            }
        }
        else
        {
            strncat(result, pos, 1);
            pos++;
        }
    }

    return result;
}

char **parse_cmd(char *substituted_user_input, char **input_file, char **output_file, int *is_pipe, char **cmd1, char **cmd2)
{
    char *in_pos = strchr(substituted_user_input, '<');
    char *out_pos = strchr(substituted_user_input, '>');
    char *pipe_pos = strchr(substituted_user_input, '|');

    if (pipe_pos)
    {
        *is_pipe = 1;
        *pipe_pos = '\0';
        pipe_pos++;
        while (*pipe_pos == ' ')
        {
            pipe_pos++;
        }
        *cmd2 = strdup(pipe_pos);
    }

    if (in_pos)
    {
        *in_pos = '\0';
        in_pos++;
        while (*in_pos == ' ')
        {
            in_pos++;
        }
        *input_file = strdup(in_pos);
        char *end = *input_file + strlen(*input_file) - 1;
        while (end > *input_file && *end == ' ')
        {
            *end = '\0';
            end--;
        }
    }

    if (out_pos)
    {
        *out_pos = '\0';
        out_pos++;
        while (*out_pos == ' ')
        {
            out_pos++;
        }
        *output_file = strdup(out_pos);
        char *end = *output_file + strlen(*output_file) - 1;
        while (end > *output_file && *end == ' ')
        {
            *end = '\0';
            end--;
        }
    }

    // char **example = split_command(substituted_user_input);
    // printf("example[0]: %s\n", example[0]);
    // printf("example[1]: %s\n", example[1]);
    // printf("example[2]: %s\n", example[2]);

    *cmd1 = strdup(substituted_user_input);
    return split_command(substituted_user_input);
}

char *locate_command(char *cmd, char **paths)
{
    for (int i = 0; paths[i] != NULL; i++)
    {
        char full_path[MAX_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], cmd);
        if (access(full_path, X_OK) == 0)
        {
            return strdup(full_path);
        }
    }
    return NULL;
}

void execute_command(char **parsed_cmd, char *input_file, char *output_file, char **paths)
{
    int in_fd = 0, out_fd = 1;

    if (input_file)
    {
        in_fd = open(input_file, O_RDONLY);
        if (in_fd < 0)
        {
            perror("Error opening input file");
            return;
        }
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }

    if (output_file)
    {
        out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0)
        {
            perror("Error opening output file");
            return;
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }

    char *absolute_path = locate_command(parsed_cmd[0], paths);
    if (absolute_path == NULL)
    {
        printf("Command not found\n");
        return;
    }

    // printf("first args: %s\n", parsed_cmd[0]);
    // printf("second args: %s\n", parsed_cmd[1]);
    // printf("third args: %s\n", parsed_cmd[2]);

    execve(absolute_path, parsed_cmd, NULL);
    perror("execv failed");
}

void handle_pipe(char *cmd1, char *cmd2, char **paths)
{
    printf("cmd1: %s\n", cmd1);
    printf("cmd2: %s\n", cmd2);

    int pipe_fd[2];
    pid_t pid1, pid2;

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe failed");
        return;
    }

    pid1 = fork();
    if (pid1 == 0)
    {
        // First command
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 failed");
            exit(1);
        }
        close(pipe_fd[1]);

        char **args1 = split_command(cmd1);
        fprintf(stdout, "args1[0]: %s\n", args1[0]);
        fprintf(stdout, "args1[1]: %s\n", args1[1]);

        execute_command(args1, NULL, NULL, paths);
        perror("execv failed");
        exit(0);
    }

    pid2 = fork();
    if (pid2 == 0)
    {
        // Second command
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);

        char **args2 = split_command(cmd2);
        printf("args2[0]: %s\n", args2[0]);
        printf("args2[1]: %s\n", args2[1]);

        execute_command(args2, NULL, NULL, paths);
        perror("execv failed");
        exit(0);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void handle_other_cmd(char *substituted_user_input, char **paths)
{
    int is_pipe = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    char *cmd1 = NULL;
    char *cmd2 = NULL;

    char **parsed_cmd = parse_cmd(substituted_user_input, &input_file, &output_file, &is_pipe, &cmd1, &cmd2);

    if (is_pipe)
    {
        if (cmd1 && cmd2)
        {
            cmd1 = trim_spaces(cmd1);
            cmd2 = trim_spaces(cmd2);
            handle_pipe(cmd1, cmd2, paths);
        }
        else
        {
            printf("Invalid pipe command\n");
        }
        // char *cmd1 = strtok(substituted_user_input, "|");
        // char *cmd2 = strtok(NULL, "|");
        // printf("cmd1: %s\n", cmd1);
        // printf("cmd2: %s\n", cmd2);
        // handle_pipe(cmd1, cmd2, paths);
    }
    else
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            execute_command(parsed_cmd, input_file, output_file, paths);
            exit(0);
        }
        wait(NULL);
    }
}

void deallocate_tokens(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

int main(int argc, char *argv[])
{
    char user_input[MAX_LENGTH];
    char cwd[MAX_LENGTH];
    char **paths = copy_absolute_path();

    while (1)
    {
        // display the current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            fprintf(stdout, "%s$ ", cwd);
        }
        else
        {
            perror("getcwd() error");
            return 1;
        }

        fgets(user_input, MAX_LENGTH, stdin);
        user_input[strlen(user_input) - 1] = '\0';

        // terminate the shell
        if ((strlen(user_input)) == 0 || strcmp(user_input, "exit") == 0 || strcmp(user_input, "quit") == 0)
        {
            break;
        }

        // handle cd command
        if (strncmp(user_input, "cd", 2) == 0)
        {
            handle_cd_command(user_input);
            continue;
        }

        // handle set, get, unset commands
        if (strncmp(user_input, "set", 3) == 0 || strncmp(user_input, "get", 3) == 0 || strncmp(user_input, "unset", 5) == 0)
        {
            handle_env_functions(user_input);
            continue;
        }

        // substitute environment variables
        char *substituted_user_input = substitute_env_variables(user_input);

        // handle other commands
        handle_other_cmd(substituted_user_input, paths);

        // // handle other commands
        // char **tokens = split_command(user_input);
        // char *cmd = tokens[0];
        // char *args = tokens[1];
        // char *expanded_args = expand_variables(args);
        // char *absolute_path = find_cmd(cmd, paths);

        // if (absolute_path == NULL)
        // {
        //     printf("Command not found\n");
        //     deallocate_tokens(tokens);
        //     free(expanded_args);
        //     continue;
        // }

        // pid_t pid = fork();
        // if (pid == 0)
        // {
        //     char *cmd_args[] = {absolute_path, expanded_args, NULL};
        //     if (execve(absolute_path, cmd_args, NULL) < 0)
        //     {
        //         perror("execve failed");
        //     }
        //     free(absolute_path);
        //     exit(1);
        // }
        // else if (pid < 0)
        // {
        //     perror("fork failed");
        // }
        // else
        // {
        //     wait(NULL);
        // }
        // deallocate_tokens(tokens);
        // free(expanded_args);
        // free(absolute_path);
    }

    printf("Exiting shell...\n");
    deallocate_tokens(paths);

    return 0;
}

// char *find_absolute_path(char *cmd)
// {
//     printf("finding absolute path for %s\n", cmd);

//     char *path_env = getenv("PATH");
//     if (path_env == NULL)
//     {
//         printf("no path\n");
//         return NULL;
//     }

//     char *path_env_copy = strdup(path_env);
//     if (path_env_copy == NULL)
//     {
//         printf("strdup failed\n");
//         return NULL;
//     }

//     char *token = strtok(path_env_copy, ":");
//     while (token != NULL)
//     {
//         char full_path[MAX_LENGTH];
//         snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);

//         if (access(full_path, X_OK) == 0)
//         {
//             free(path_env_copy);
//             return strdup(full_path);
//         }
//         token = strtok(NULL, ":");
//     }
//     free(path_env_copy);
//     return NULL;
// }