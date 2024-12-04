#include "command_shell.h"

int main(int argc, char *argv[])
{
    char user_input[1024];
    char cwd[1024];

    while (strcmp(user_input, "exit") != 0 && strcmp(user_input, "quit") != 0)
    {
        printf("Enter a command: ");
        fgets(user_input, 1024, stdin);
        user_input[strlen(user_input) - 1] = '\0';

        if (strcmp(user_input, "pwd") == 0)
        {
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                fprintf(stdout, "Current working dir: %s\n", cwd);
            }
            else
            {
                perror("getcwd() error");
                return 1;
            }
        }
        else if (strcmp(user_input, "cd..") == 0)
        {
            chdir("..");
        }
        else if (strcmp(user_input, "cd") == 0)
        {
            chdir(getenv("HOME"));
        }
        else if (strncmp(user_input, "set" , 3) == 0)
        {
            char *env_var = strtok(user_input, " ");
            char *env_val = strtok(NULL, " ");
            setenv(env_var, env_val, 1);
        }


        else
        {
            system(user_input);
        }
        printf(getcwd(cwd, sizeof(cwd)) != NULL ? "Current working dir: %s\n" : "getcwd() error\n", cwd);
    }

    // char cwd[1024];

    // if (getcwd(cwd, sizeof(cwd)) != NULL)
    // {
    //     fprintf(stdout, "Current working dir: %s\n", cwd);
    // }
    // else
    // {
    //     perror("getcwd() error");
    //     return 1;
    // }

    printf("Exiting shell...\n");

    return 0;

    // int fds[2];
    // pipe(fds);
    // pid_t pid = fork();

    // if (pid == 0)
    // {
    //     // Child process
    //     dup2(fds[0], STDIN_FILENO);
    //     close(fds[0]);
    //     close(fds[1]);
    //     char *args[] = {(char *)"sort", NULL};
    //     if (execvp(argv[0], argv) < 0)
    //         exit(0);
    // }
    // close(fds[0]);
    // const char *words[] = {"pear", "peach", "apple", "banana", "apple"};
    // size_t numwords = sizeof(words) / sizeof(words[0]);
    // for (size_t i = 0; i < numwords; i++)
    // {
    //     dprintf(fds[1], "%s\n", words[i]);
    // }

    // close(fds[1]);

    // int status;
    // pid_t wpid = waitpid(pid, &status, 0);
    // return wpid == pid && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}