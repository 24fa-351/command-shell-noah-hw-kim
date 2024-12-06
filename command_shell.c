#include "command_shell.h"

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

// splits string by spaces; adds a NULL int to the array after the last word
char **split_command(char *s)
{
    char **result = malloc(3 * sizeof(char *));
    char *space_pos = strchr(s, ' ');

    if (space_pos != NULL)
    {
        int cmd_len = space_pos - s;
        result[0] = strndup(s, cmd_len);
        result[1] = strdup(space_pos + 1);
    }
    else
    {
        result[0] = strdup(s);
        result[1] = NULL;
    }
    result[2] = NULL;
    return result;
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

char *find_cmd(char *cmd, char **paths)
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

char *expand_variables(const char *input)
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
        if (*pos == '$')
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
        if (strcmp(user_input, "exit") == 0 || strcmp(user_input, "quit") == 0)
        {
            break;
        }

        // handle cd command
        if (strncmp(user_input, "cd", 2) == 0)
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

            continue;
        }

        // handle set command
        if (strncmp(user_input, "set", 3) == 0)
        {
            char **tokens = split_command(user_input);
            if (tokens[1] != NULL)
            {
                char **key_value = split_command(tokens[1]);
                if (key_value[0] != NULL && key_value[1] != NULL)
                {
                    setenv(key_value[0], key_value[1], 1);
                }
                else
                {
                    printf("Usage: set <key> <value>\n");
                }
                deallocate_tokens(key_value);
            }
            else
            {
                printf("Usage: set <key> <value>\n");
            }
            deallocate_tokens(tokens);
            continue;
        }

        // handle unset command
        if (strncmp(user_input, "unset", 5) == 0)
        {
            char **tokens = split_command(user_input);
            if (tokens[1] == NULL)
            {
                printf("Usage: unset <key>\n");
            }
            else
            {
                unsetenv(tokens[1]);
            }
            deallocate_tokens(tokens);
            continue;
        }

        // handle get command
        if (strncmp(user_input, "get", 3) == 0)
        {
            char **tokens = split_command(user_input);
            if (tokens[1] == NULL)
            {
                printf("Usage: get <key>\n");
            }
            else
            {
                char *value = getenv(tokens[1]);
                if (value == NULL)
                {
                    printf("Key not found\n");
                }
                else
                {
                    printf("%s\n", value);
                }
            }
            deallocate_tokens(tokens);
            continue;
        }

        // handle other commands
        char **tokens = split_command(user_input);
        char *cmd = tokens[0];
        char *args = tokens[1];
        char *expanded_args = expand_variables(args);
        char *absolute_path = find_cmd(cmd, paths);
        if (absolute_path == NULL)
        {
            printf("Command not found\n");
            deallocate_tokens(tokens);
            free(expanded_args);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            char *cmd_args[] = {absolute_path, expanded_args, NULL};
            if (execve(absolute_path, cmd_args, NULL) < 0)
            {
                perror("execve failed");
            }
            free(absolute_path);
            free(expanded_args);
            exit(1);
        }
        else if (pid < 0)
        {
            perror("fork failed");
        }
        else
        {
            waitpid(pid, NULL, 0);
        }
        deallocate_tokens(tokens);
        free(expanded_args);
    }

    printf("Exiting shell...\n");
    deallocate_tokens(paths);

    return 0;
}
