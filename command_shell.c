#include "command_shell.h"

int main(int argc, char *argv[])
{
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
    char *path = getenv("PATH");
    char *token;
    char *delimeter = ":";

    if (path == NULL)
    {
        printf("PATH not found\n");
        return 1;
    }

    char *path_copy = strdup(path);
    token = strtok(path_copy, delimeter);

    while (token != NULL)
    {
        printf("PATH: %s\n", token);
        token = strtok(NULL, delimeter);
    }

    free(path_copy);

    return 0;
}